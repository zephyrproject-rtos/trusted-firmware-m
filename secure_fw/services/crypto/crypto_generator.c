/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

/* FixMe: Use PSA_ERROR_CONNECTION_REFUSED when performing parameter
 *        integrity checks but this will have to be revised
 *        when the full set of error codes mandated by PSA FF
 *        is available.
 */
#include "tfm_mbedcrypto_include.h"

/* Required for mbedtls_calloc in tfm_crypto_huk_derivation */
#include "mbedtls/platform.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"
#include "tfm_memory_utils.h"

#include "platform/include/tfm_plat_crypto_keys.h"

#ifdef TFM_PARTITION_TEST_SST
#include "psa_manifest/pid.h"
#endif /* TFM_PARTITION_TEST_SST */

/**
 * \brief Perform a key derivation operation from the hardware unique key (HUK).
 *
 * \note This function allows key derivation from the HUK to be implemented in
 *       a platform-defined way by calling the TF-M platform function
 *       tfm_plat_get_huk_derived_key.
 *
 * \param[in,out] generator     Generator object to set up
 * \param[in]     key_handle    Handle to the secret key
 * \param[in]     salt          Salt to use
 * \param[in]     salt_length   Size of the salt buffer in bytes
 * \param[in]     label         Label to use
 * \param[in]     label_length  Size of the label buffer in bytes
 * \param[in]     capacity      Maximum number of bytes that the generator will
 *                              be able to provide
 *
 * \return Return values as described in \ref psa_status_t
 */
static psa_status_t tfm_crypto_huk_derivation(psa_crypto_generator_t *generator,
                                              psa_key_handle_t key_handle,
                                              const uint8_t *salt,
                                              size_t salt_length,
                                              const uint8_t *label,
                                              size_t label_length,
                                              size_t capacity)
{
    psa_status_t status;
    enum tfm_plat_err_t plat_err;
    int32_t partition_id;
    uint8_t *partition_label;
    psa_key_policy_t key_policy;

    status = psa_get_key_policy(key_handle, &key_policy);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Check that the input key has the correct policy */
    if (key_policy.usage != PSA_KEY_USAGE_DERIVE ||
        key_policy.alg != TFM_CRYPTO_ALG_HUK_DERIVATION) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Concatenate the caller's partition ID with the supplied label to prevent
     * two different partitions from deriving the same key.
     */
    status = tfm_crypto_get_caller_id(&partition_id);
    if (status != PSA_SUCCESS) {
        return status;
    }

#ifdef TFM_PARTITION_TEST_SST
    /* The SST tests run some operations under the wrong partition ID - this
     * causes the key derivation to change.
     */
    if (partition_id == TFM_SP_SST_TEST) {
        partition_id = TFM_SP_STORAGE;
    }
#endif /* TFM_PARTITION_TEST_SST */

    partition_label = mbedtls_calloc(1, sizeof(partition_id) + label_length);
    if (partition_label == NULL) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    (void)tfm_memcpy(partition_label, &partition_id, sizeof(partition_id));
    (void)tfm_memcpy(partition_label + sizeof(partition_id), label,
                     label_length);

    /* Set up the generator object to contain the raw derived key material, so
     * that it can be directly extracted in psa_generator_import_key or
     * psa_generator_read.
     */
    generator->alg = PSA_ALG_SELECT_RAW;
    generator->ctx.buffer.data = mbedtls_calloc(1, capacity);
    if (generator->ctx.buffer.data == NULL) {
        mbedtls_free(partition_label);
        (void)psa_generator_abort(generator);
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }
    generator->ctx.buffer.size = capacity;
    generator->capacity = capacity;

    /* Derive key material from the HUK and output it to the generator buffer */
    plat_err = tfm_plat_get_huk_derived_key(partition_label,
                                            sizeof(partition_id) + label_length,
                                            salt, salt_length,
                                            generator->ctx.buffer.data,
                                            generator->ctx.buffer.size);
    mbedtls_free(partition_label);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        (void)psa_generator_abort(generator);
        return PSA_ERROR_HARDWARE_FAILURE;
    }

    return PSA_SUCCESS;
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_get_generator_capacity(psa_invec in_vec[],
                                               size_t in_len,
                                               psa_outvec out_vec[],
                                               size_t out_len)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(size_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    size_t *capacity = out_vec[0].base;
    psa_crypto_generator_t *generator = NULL;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_GENERATOR_OPERATION,
                                         handle,
                                         (void **)&generator);
    if (status != PSA_SUCCESS) {
        *capacity = 0;
        return status;
    }

    return psa_get_generator_capacity(generator, capacity);
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generator_read(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    uint8_t *output = out_vec[0].base;
    size_t output_length = out_vec[0].len;
    psa_crypto_generator_t *generator = NULL;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_GENERATOR_OPERATION,
                                         handle,
                                         (void **)&generator);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_generator_read(generator, output, output_length);
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generator_import_key(psa_invec in_vec[],
                                             size_t in_len,
                                             psa_outvec out_vec[],
                                             size_t out_len)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 2) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(size_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    psa_key_handle_t key_handle = iov->key_handle;
    psa_key_type_t type = iov->type;
    size_t bits = *(size_t *)(in_vec[1].base);
    psa_crypto_generator_t *generator = NULL;

    status = tfm_crypto_check_handle_owner(key_handle, NULL);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_GENERATOR_OPERATION,
                                         handle,
                                         (void **)&generator);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_generator_import_key(key_handle, type, bits, generator);
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generator_abort(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_crypto_generator_t *generator = NULL;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->op_handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_GENERATOR_OPERATION,
                                         handle,
                                         (void **)&generator);
    if (status != PSA_SUCCESS) {
        /* Operation does not exist, so abort has no effect */
        return PSA_SUCCESS;
    }

    *handle_out = handle;

    status = psa_generator_abort(generator);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
        return status;
    }

    status = tfm_crypto_operation_release(handle_out);

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_derivation(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if (!((in_len == 1) || (in_len == 2) || (in_len == 3)) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_key_handle_t key_handle = iov->key_handle;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *salt = NULL;
    size_t salt_length = 0;
    const uint8_t *label = NULL;
    size_t label_length = 0;
    size_t capacity = iov->capacity;
    psa_crypto_generator_t *generator = NULL;

    if (in_len > 1) {
        salt = in_vec[1].base;
        salt_length = in_vec[1].len;
    }

    if (in_len > 2) {
        label = in_vec[2].base;
        label_length = in_vec[2].len;
    }

    status = tfm_crypto_check_handle_owner(key_handle, NULL);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Allocate the generator context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_GENERATOR_OPERATION,
                                        &handle,
                                        (void **)&generator);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *handle_out = handle;

    /* If the caller requests that the TFM_CRYPTO_ALG_HUK_DERIVATION algorithm
     * is used for key derivation, then redirect the request to the TF-M HUK
     * derivation function, so that key derivation from the HUK can be
     * implemented in a platform-defined way.
     * FIXME: In the future, this should be replaced by the Mbed Crypto driver
     * model.
     */
    if (alg == TFM_CRYPTO_ALG_HUK_DERIVATION) {
        status = tfm_crypto_huk_derivation(generator, key_handle, salt,
                                           salt_length, label, label_length,
                                           capacity);
    } else {
        status = psa_key_derivation(generator, key_handle, alg, salt,
                                    salt_length, label, label_length, capacity);
    }
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
    }

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

psa_status_t tfm_crypto_key_agreement(psa_invec in_vec[],
                                      size_t in_len,
                                      psa_outvec out_vec[],
                                      size_t out_len)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    psa_status_t status;
    if ((in_len != 2) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    uint32_t handle = iov->op_handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_key_handle_t private_key = iov->key_handle;
    psa_algorithm_t alg = iov->alg;
    const uint8_t *peer_key = in_vec[1].base;
    size_t peer_key_length = in_vec[1].len;
    psa_crypto_generator_t *generator = NULL;

    status = tfm_crypto_check_handle_owner(private_key, NULL);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Allocate the generator context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_GENERATOR_OPERATION,
                                        &handle,
                                        (void **)&generator);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *handle_out = handle;

    status = psa_key_agreement(generator, private_key,
                               peer_key, peer_key_length, alg);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
        (void)tfm_crypto_operation_release(handle_out);
    }

    return status;
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generate_random(psa_invec in_vec[],
                                        size_t in_len,
                                        psa_outvec out_vec[],
                                        size_t out_len)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    uint8_t *output = out_vec[0].base;
    size_t output_size = out_vec[0].len;

    return psa_generate_random(output, output_size);
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}

psa_status_t tfm_crypto_generate_key(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
#if (TFM_CRYPTO_GENERATOR_MODULE_DISABLED != 0)
    return PSA_ERROR_NOT_SUPPORTED;
#else
    if (!((in_len == 2) || (in_len == 3)) || (out_len != 0)) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(size_t))) {
        return PSA_ERROR_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    psa_key_handle_t key_handle = iov->key_handle;
    psa_key_type_t type = iov->type;
    size_t bits = *((size_t *)(in_vec[1].base));
    const void *extra = NULL;
    size_t extra_size = 0;
    psa_status_t status;

    status = tfm_crypto_check_handle_owner(key_handle, NULL);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (in_len == 3) {
        extra = in_vec[2].base;
        extra_size = in_vec[2].len;
    }

    return psa_generate_key(key_handle, type, bits, extra, extra_size);
#endif /* TFM_CRYPTO_GENERATOR_MODULE_DISABLED */
}
/*!@}*/
