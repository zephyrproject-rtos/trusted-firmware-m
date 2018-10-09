/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <limits.h>

#include "tfm_crypto_defs.h"

#include "crypto_engine.h"

#include "psa_crypto.h"

#include "tfm_crypto_struct.h"

#include "tfm_crypto_api.h"
#include "crypto_utils.h"

static enum tfm_crypto_err_t tfm_crypto_cipher_setup(
                                           psa_cipher_operation_t *operation,
                                           psa_key_slot_t key,
                                           psa_algorithm_t alg,
                                           enum engine_cipher_mode_t c_mode)
{
    uint8_t key_data[TFM_CRYPTO_MAX_KEY_LENGTH];
    size_t key_size;
    psa_key_type_t key_type = PSA_KEY_TYPE_NONE;
    psa_status_t status = PSA_SUCCESS;
    enum tfm_crypto_err_t err;
    struct tfm_cipher_operation_s *ctx = NULL;
    struct cipher_engine_info engine_info;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (!PSA_ALG_IS_CIPHER(alg)) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Access the key module to retrieve key related information */
    err = tfm_crypto_get_key_information(key, &key_type, &key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Setup the algorithm on the crypto engine */
    status = tfm_crypto_engine_cipher_setup(alg,
                                            (const psa_key_type_t)key_type,
                                            (const uint32_t)key_size,
                                            c_mode,
                                            &engine_info);
    if (status != PSA_SUCCESS) {
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    /* Allocate the operation context in the secure world */
    err = tfm_crypto_operation_alloc(TFM_CRYPTO_CIPHER_OPERATION,
                                     &(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return err;
    }

    /* Set the proper cipher mode (encrypt/decrypt) in the operation context */
    ctx->cipher_mode = (uint8_t) c_mode;

    /* Bind the algorithm to the cipher operation */
    ctx->alg = alg;

    /* Start the crypto engine */
    status = tfm_crypto_engine_cipher_start(&(ctx->engine_ctx), &engine_info);
    if (status != PSA_SUCCESS) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    /* Access the crypto service key module to retrieve key data */
    err = tfm_crypto_export_key(key,
                                &key_data[0],
                                TFM_CRYPTO_MAX_KEY_LENGTH,
                                &key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return err;
    }

    /* Set the key on the engine */
    status = tfm_crypto_engine_cipher_set_key(&(ctx->engine_ctx),
                                              key_data,
                                              key_size,
                                              &engine_info);
    if (status != PSA_SUCCESS) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    /* Bind the key to the cipher operation */
    ctx->key = key;
    ctx->key_set = 1;

    /* Set padding mode on engine in case of CBC */
    if ((alg & ~PSA_ALG_BLOCK_CIPHER_PADDING_MASK) == PSA_ALG_CBC_BASE) {

        status = tfm_crypto_engine_cipher_set_padding_mode(&(ctx->engine_ctx),
                                                           &engine_info);
        if (status != PSA_SUCCESS) {
            /* Release the operation context */
            tfm_crypto_operation_release(&(operation->handle));
            return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
        }
    }

    /* FIXME: Check based on the algorithm, if we need to have an IV */
    ctx->iv_required = 1;
    ctx->block_size = PSA_BLOCK_CIPHER_BLOCK_SIZE(key_type);

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_cipher_set_iv(
                                              psa_cipher_operation_t *operation,
                                              const unsigned char *iv,
                                              size_t iv_length)
{
    psa_status_t status = PSA_SUCCESS;
    enum tfm_crypto_err_t err;
    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check((void *)iv, iv_length, TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    if ((iv_length != ctx->block_size) || (iv_length > TFM_CIPHER_IV_MAX_SIZE)){
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((ctx->iv_set == 1) || (ctx->iv_required == 0)) {
        return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
    }

    /* Set the IV on the crypto engine */
    status = tfm_crypto_engine_cipher_set_iv(&(ctx->engine_ctx),
                                             iv,
                                             iv_length);
    if (status != PSA_SUCCESS) {
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    ctx->iv_set = 1;
    ctx->iv_size = iv_length;

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_cipher_encrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg)
{
    return tfm_crypto_cipher_setup(operation,
                                   key,
                                   alg,
                                   ENGINE_CIPHER_MODE_ENCRYPT);
}

enum tfm_crypto_err_t tfm_crypto_cipher_decrypt_setup(
                                              psa_cipher_operation_t *operation,
                                              psa_key_slot_t key,
                                              psa_algorithm_t alg)
{
    return tfm_crypto_cipher_setup(operation,
                                   key,
                                   alg,
                                   ENGINE_CIPHER_MODE_DECRYPT);
}

enum tfm_crypto_err_t tfm_crypto_cipher_update(
                                              psa_cipher_operation_t *operation,
                                              const uint8_t *input,
                                              size_t input_length,
                                              unsigned char *output,
                                              size_t output_size,
                                              size_t *output_length)
{
    psa_status_t status = PSA_SUCCESS;
    enum tfm_crypto_err_t err;
    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check((void *)input,
                                  input_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(output,
                                  output_size,
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(output_length,
                                  sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* If the IV is required and it's not been set yet */
    if ((ctx->iv_required == 1) && (ctx->iv_set == 0)) {

        if (ctx->cipher_mode != ENGINE_CIPHER_MODE_DECRYPT) {
            return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
        }

        /* This call is used to set the IV on the object */
        err = tfm_crypto_cipher_set_iv(operation, input, input_length);

        *output_length = 0;

        return err;
    }

    /* If the key is not set, setup phase has not been completed */
    if (ctx->key_set == 0) {
        return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
    }

    /* Update the cipher output with the input chunk on the engine */
    status = tfm_crypto_engine_cipher_update(&(ctx->engine_ctx),
                                             input,
                                             input_length,
                                             output,
                                             (uint32_t *)output_length);
    if (status != PSA_SUCCESS) {
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_cipher_finish(
                                              psa_cipher_operation_t *operation,
                                              uint8_t *output,
                                              size_t output_size,
                                              size_t *output_length)
{
    psa_status_t status = PSA_SUCCESS;
    enum tfm_crypto_err_t err;
    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(output,
                                  output_size,
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(output_length,
                                  sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Finalise the operation on the crypto engine */
    status = tfm_crypto_engine_cipher_finish(&(ctx->engine_ctx),
                                             output,
                                             (uint32_t *)output_length);
    if (status != PSA_SUCCESS) {
        *output_length = 0;
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    /* Release resources on the crypto engine */
    status = tfm_crypto_engine_cipher_release(&(ctx->engine_ctx));
    if (status != PSA_SUCCESS) {
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    /* Release the operation context */
    err = tfm_crypto_operation_release(&(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_cipher_abort(psa_cipher_operation_t *operation)
{
    psa_status_t status = PSA_SUCCESS;
    enum tfm_crypto_err_t err;
    struct tfm_cipher_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_cipher_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_CIPHER_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Release resources on the engine */
    status = tfm_crypto_engine_cipher_release(&(ctx->engine_ctx));
    if (status != PSA_SUCCESS) {
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    /* Release the operation context */
    err = tfm_crypto_operation_release(&(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}
/*!@}*/
