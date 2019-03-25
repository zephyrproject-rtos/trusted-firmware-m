/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_crypto_api.h"
#include "crypto_engine.h"
#include "tfm_crypto_struct.h"

/**
 * \def CRYPTO_CIPHER_MAX_KEY_LENGTH
 *
 * \brief Specifies the maximum key length supported by the
 *        Cipher operations in this implementation
 */
#ifndef CRYPTO_CIPHER_MAX_KEY_LENGTH
#define CRYPTO_CIPHER_MAX_KEY_LENGTH (32)
#endif

static psa_status_t _psa_get_key_information(psa_key_slot_t key,
                                             psa_key_type_t *type,
                                             size_t *bits)
{
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
    };
    psa_outvec out_vec[] = {
        {.base = type, .len = sizeof(psa_key_type_t)},
        {.base = bits, .len = sizeof(size_t)}
    };

    return tfm_crypto_get_key_information(
                 in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                 out_vec, sizeof(out_vec)/sizeof(out_vec[0]));
}

/**
 * \brief Release all resources associated with a cipher operation.
 *
 * \param[in] operation  Frontend cipher operation context
 * \param[in] ctx        Backend cipher operation context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
static psa_status_t tfm_crypto_cipher_release(
                                             psa_cipher_operation_t *operation,
                                             struct tfm_cipher_operation_s *ctx)
{
    psa_status_t status = PSA_SUCCESS;

    /* Release resources in the engine */
    status = tfm_crypto_engine_cipher_release(&(ctx->engine_ctx));
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Release the operation context */
    return tfm_crypto_operation_release(TFM_CRYPTO_CIPHER_OPERATION, operation);
}

static psa_status_t tfm_crypto_cipher_setup(psa_cipher_operation_t *operation,
                                            psa_key_slot_t key,
                                            psa_algorithm_t alg,
                                            enum engine_cipher_mode_t c_mode)
{
    uint8_t key_data[CRYPTO_CIPHER_MAX_KEY_LENGTH];
    size_t key_size;
    psa_key_type_t key_type = PSA_KEY_TYPE_NONE;
    psa_status_t status = PSA_SUCCESS;
    struct tfm_cipher_operation_s *ctx = NULL;
    struct cipher_engine_info engine_info;
    psa_key_usage_t usage;

    if (!PSA_ALG_IS_CIPHER(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Access the key module to retrieve key related information */
    status = _psa_get_key_information(key, &key_type, &key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Check if it's a raw data key type */
    if (key_type == PSA_KEY_TYPE_RAW_DATA) {
        return PSA_ERROR_NOT_PERMITTED;
    }

    /* Check compatibility between key and algorithm */
    if ((key_type == PSA_KEY_TYPE_ARC4) && (alg != PSA_ALG_ARC4)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Setup the algorithm on the crypto engine */
    status = tfm_crypto_engine_cipher_setup(alg,
                                            (const psa_key_type_t)key_type,
                                            (const uint32_t)key_size,
                                            c_mode,
                                            &engine_info);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Allocate the operation context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_CIPHER_OPERATION,
                                        operation,
                                        (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Set the proper cipher mode (encrypt/decrypt) in the operation context */
    ctx->cipher_mode = (uint8_t) c_mode;

    /* Bind the algorithm to the cipher operation */
    ctx->alg = alg;

    /* Start the crypto engine */
    status = tfm_crypto_engine_cipher_start(&(ctx->engine_ctx), &engine_info);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if this operation fails. */
        (void)tfm_crypto_cipher_release(operation, ctx);
        return status;
    }

    /* Set the key usage based on the cipher mode */
    usage = (c_mode == ENGINE_CIPHER_MODE_DECRYPT) ? PSA_KEY_USAGE_DECRYPT
                                                   : PSA_KEY_USAGE_ENCRYPT;

    /* Access the crypto service key module to retrieve key data */
    status = tfm_crypto_get_key(key,
                                usage,
                                alg,
                                key_data,
                                CRYPTO_CIPHER_MAX_KEY_LENGTH,
                                &key_size);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if this operation fails. */
        (void)tfm_crypto_cipher_release(operation, ctx);
        return status;
    }

    /* Set the key on the engine */
    status = tfm_crypto_engine_cipher_set_key(&(ctx->engine_ctx),
                                              key_data,
                                              key_size,
                                              &engine_info);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if this operation fails. */
        (void)tfm_crypto_cipher_release(operation, ctx);
        return status;
    }

    /* Bind the key to the cipher operation */
    ctx->key = key;
    ctx->key_set = 1;

    /* Set padding mode on engine in case of CBC */
    if ((alg & ~PSA_ALG_BLOCK_CIPHER_PADDING_MASK) == PSA_ALG_CBC_BASE) {

        status = tfm_crypto_engine_cipher_set_padding_mode(&(ctx->engine_ctx),
                                                           &engine_info);
        if (status != PSA_SUCCESS) {
            /* Release the operation context, ignore if this operation fails. */
            (void)tfm_crypto_cipher_release(operation, ctx);
            return status;
        }
    }

    /* FIXME: Check based on the algorithm, if we need to have an IV */
    ctx->iv_required = 1;
    ctx->block_size = PSA_BLOCK_CIPHER_BLOCK_SIZE(key_type);

    return PSA_SUCCESS;
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_cipher_set_iv(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_cipher_operation_s *ctx = NULL;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if (out_vec[0].len != sizeof(psa_cipher_operation_t)) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_cipher_operation_t *operation = out_vec[0].base;
    const unsigned char *iv = in_vec[0].base;
    size_t iv_length = in_vec[0].len;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                         operation,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if ((iv_length != ctx->block_size) || (iv_length > TFM_CIPHER_IV_MAX_SIZE)){
        (void)tfm_crypto_cipher_release(operation, ctx);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((ctx->iv_set == 1) || (ctx->iv_required == 0)) {
        (void)tfm_crypto_cipher_release(operation, ctx);
        return PSA_ERROR_BAD_STATE;
    }

    /* Set the IV on the crypto engine */
    status = tfm_crypto_engine_cipher_set_iv(&(ctx->engine_ctx),
                                             iv,
                                             iv_length);
    if (status != PSA_SUCCESS) {
        (void)tfm_crypto_cipher_release(operation, ctx);
        return status;
    }

    ctx->iv_set = 1;
    ctx->iv_size = iv_length;

    return PSA_SUCCESS;
}

static psa_status_t _psa_cipher_set_iv(psa_cipher_operation_t *operation,
                                       const unsigned char *iv,
                                       size_t iv_length)
{
    psa_status_t status;
    psa_invec in_vec[] = {
        {.base = iv, .len = iv_length},
    };
    psa_outvec out_vec[] = {
        {.base = operation, .len = sizeof(psa_cipher_operation_t)},
    };

    status = tfm_crypto_cipher_set_iv(in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                                   out_vec, sizeof(out_vec)/sizeof(out_vec[0]));
    return status;
}
psa_status_t tfm_crypto_cipher_encrypt_setup(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(psa_cipher_operation_t)) ||
        (in_vec[0].len != sizeof(psa_key_slot_t)) ||
        (in_vec[1].len != sizeof(psa_algorithm_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_cipher_operation_t *operation = out_vec[0].base;
    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    psa_algorithm_t alg = *((psa_algorithm_t *)in_vec[1].base);

    return tfm_crypto_cipher_setup(operation,
                                   key,
                                   alg,
                                   ENGINE_CIPHER_MODE_ENCRYPT);
}

psa_status_t tfm_crypto_cipher_decrypt_setup(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(psa_cipher_operation_t)) ||
        (in_vec[0].len != sizeof(psa_key_slot_t)) ||
        (in_vec[1].len != sizeof(psa_algorithm_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_cipher_operation_t *operation = out_vec[0].base;
    psa_key_slot_t key = *((psa_key_slot_t *)in_vec[0].base);
    psa_algorithm_t alg = *((psa_algorithm_t *)in_vec[1].base);

    return tfm_crypto_cipher_setup(operation,
                                   key,
                                   alg,
                                   ENGINE_CIPHER_MODE_DECRYPT);
}

psa_status_t tfm_crypto_cipher_update(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_cipher_operation_s *ctx = NULL;

    if ((in_len != 1) || (out_len != 2)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(psa_cipher_operation_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_cipher_operation_t *operation = out_vec[0].base;
    const uint8_t *input = in_vec[0].base;
    size_t input_length = in_vec[0].len;
    unsigned char *output = out_vec[1].base;
    size_t output_size = out_vec[1].len;

    /* Initialise the output_length to zero */
    out_vec[1].len = 0;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                         operation,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* If the IV is required and it's not been set yet */
    if ((ctx->iv_required == 1) && (ctx->iv_set == 0)) {

        if (ctx->cipher_mode != ENGINE_CIPHER_MODE_DECRYPT) {
            (void)tfm_crypto_cipher_release(operation, ctx);
            return PSA_ERROR_BAD_STATE;
        }

        /* This call is used to set the IV on the object */
        return _psa_cipher_set_iv(operation, input, input_length);
    }

    /* If the key is not set, setup phase has not been completed */
    if (ctx->key_set == 0) {
        (void)tfm_crypto_cipher_release(operation, ctx);
        return PSA_ERROR_BAD_STATE;
    }

    /* FIXME: The implementation currently expects to work only on blocks
     *        of input data whose length is equal to the block size
     */
    if (input_length > output_size) {
        (void)tfm_crypto_cipher_release(operation, ctx);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Update the cipher output with the input chunk on the engine */
    status = tfm_crypto_engine_cipher_update(&(ctx->engine_ctx),
                                             input,
                                             input_length,
                                             output,
                                             (uint32_t *)&(out_vec[1].len));
    if (status != PSA_SUCCESS) {
        (void)tfm_crypto_cipher_release(operation, ctx);
        return status;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_cipher_finish(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_cipher_operation_s *ctx = NULL;

    if ((in_len != 0) || (out_len != 2)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(psa_cipher_operation_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_cipher_operation_t *operation = out_vec[0].base;
    unsigned char *output = out_vec[1].base;
    size_t output_size = out_vec[1].len;

    /* Initialise the output_length to zero */
    out_vec[1].len = 0;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                         operation,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Check that the output buffer is large enough for up to one block size of
     * output data.
     */
    if (output_size < ctx->block_size) {
        (void)tfm_crypto_cipher_release(operation, ctx);
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Finalise the operation on the crypto engine */
    status = tfm_crypto_engine_cipher_finish(&(ctx->engine_ctx),
                                             output,
                                             (uint32_t *)&(out_vec[1].len));
    if (status != PSA_SUCCESS) {
        out_vec[1].len = 0;
        (void)tfm_crypto_cipher_release(operation, ctx);
        return status;
    }

    return tfm_crypto_cipher_release(operation, ctx);
}

psa_status_t tfm_crypto_cipher_abort(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_cipher_operation_s *ctx = NULL;

    if ((in_len != 0) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(psa_cipher_operation_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_cipher_operation_t *operation = out_vec[0].base;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                         operation,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return tfm_crypto_cipher_release(operation, ctx);
}
/*!@}*/
