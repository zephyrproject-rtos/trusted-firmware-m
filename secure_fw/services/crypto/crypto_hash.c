/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <limits.h>

#include "tfm_crypto_defs.h"

/* Pre include Mbed TLS headers */
#define LIB_PREFIX_NAME __tfm_crypto__
#include "mbedtls_global_symbols.h"

/* Include the Mbed TLS configuration file, the way Mbed TLS does it
 * in each of its header files. */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "platform/ext/common/tfm_mbedtls_config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "psa_crypto.h"

#include "tfm_crypto_struct.h"

#include "tfm_crypto_api.h"
#include "crypto_utils.h"

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_hash_setup(psa_hash_operation_t *operation,
                                            psa_algorithm_t alg)
{
    int ret;
    enum tfm_crypto_err_t err;
    const mbedtls_md_info_t *info = NULL;
    mbedtls_md_type_t type = MBEDTLS_MD_NONE;

    struct tfm_hash_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_hash_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_ALG_IS_HASH(alg) == 0) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Mbed TLS message digest setup */
    switch(alg) {
    case PSA_ALG_MD2:
#if defined(MBEDTLS_MD2_C)
        type = MBEDTLS_MD_MD2;
#endif
        break;
    case PSA_ALG_MD4:
#if defined(MBEDTLS_MD4_C)
        type = MBEDTLS_MD_MD4;
#endif
        break;
    case PSA_ALG_MD5:
#if defined(MBEDTLS_MD5_C)
        type = MBEDTLS_MD_MD5;
#endif
        break;
    case PSA_ALG_RIPEMD160:
#if defined(MBEDTLS_RIPEMD160_C)
        type = MBEDTLS_MD_RIPEMD160;
#endif
        break;
    case PSA_ALG_SHA_1:
#if defined(MBEDTLS_SHA1_C)
        type = MBEDTLS_MD_SHA1;
#endif
        break;
    case PSA_ALG_SHA_224:
#if defined(MBEDTLS_SHA256_C)
        type = MBEDTLS_MD_SHA224;
#endif
        break;
    case PSA_ALG_SHA_256:
#if defined(MBEDTLS_SHA256_C)
        type = MBEDTLS_MD_SHA256;
#endif
        break;
    case PSA_ALG_SHA_384:
#if defined(MBEDTLS_SHA512_C)
        type = MBEDTLS_MD_SHA384;
#endif
        break;
    case PSA_ALG_SHA_512:
#if defined(MBEDTLS_SHA512_C)
        type = MBEDTLS_MD_SHA512;
#endif
        break;
    default:
        break;
    }

    /* Mbed TLS currently does not support SHA3: PSA_ALG_SHA3_224,
     * PSA_ALG_SHA3_256, PSA_ALG_SHA3_384, PSA_ALG_SHA3_512;
     * Mbed TLS currently does not support SHA-512 Truncated modes:
     * PSA_ALG_SHA_512_224, PSA_ALG_SHA_512_256
     */
    if (type == MBEDTLS_MD_NONE) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    /* Allocate the operation context in the Secure world */
    err = tfm_crypto_operation_alloc(TFM_CRYPTO_HASH_OPERATION,
                                     &(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return err;
    }

    /* Bind the algorithm to the hash context */
    ctx->alg = alg;

    /* Mbed TLS message digest init */
    mbedtls_md_init(&ctx->md);
    info = mbedtls_md_info_from_type(type);
    ret = mbedtls_md_setup(&(ctx->md), info, 0); /* 0: not HMAC */
    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    /* Start the message digest context */
    ret = mbedtls_md_starts(&(ctx->md));
    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_hash_update(psa_hash_operation_t *operation,
                                             const uint8_t *input,
                                             size_t input_length)
{
    int ret;
    enum tfm_crypto_err_t err;
    struct tfm_hash_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_hash_operation_t),
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

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Process the input chunk */
    ret = mbedtls_md_update(&(ctx->md), input, input_length);
    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_hash_finish(psa_hash_operation_t *operation,
                                             uint8_t *hash,
                                             size_t hash_size,
                                             size_t *hash_length)
{
    int ret;
    enum tfm_crypto_err_t err;
    struct tfm_hash_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_hash_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(hash,
                                  hash_size,
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }
    err = tfm_crypto_memory_check(hash_length,
                                  sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    if (hash_size < PSA_HASH_SIZE(ctx->alg)) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Finalise the hash value */
    ret = mbedtls_md_finish(&(ctx->md), hash);
    if (ret != 0) {
        /* Release the operation context */
        tfm_crypto_operation_release(&(operation->handle));
        return TFM_CRYPTO_ERR_PSA_ERROR_COMMUNICATION_FAILURE;
    }

    /* Set the length of the hash that has been produced */
    *hash_length = PSA_HASH_SIZE(ctx->alg);

    /* Clear the Mbed TLS message digest context */
    mbedtls_md_free(&(ctx->md));

    /* Release the operation context */
    err = tfm_crypto_operation_release(&(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_hash_verify(psa_hash_operation_t *operation,
                                             const uint8_t *hash,
                                             size_t hash_length)
{
    enum tfm_crypto_err_t err;
    uint8_t digest[PSA_HASH_MAX_SIZE] = {0};
    size_t digest_length;
    uint32_t idx, comp_mismatch = 0;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_hash_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Finalise the hash operation */
    err = tfm_crypto_hash_finish(operation,
                                 digest,
                                 PSA_HASH_MAX_SIZE,
                                 &digest_length);

    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Verify that the computed hash matches the provided one */
    for (idx=0; idx<(uint32_t)digest_length; idx++) {
        if (digest[idx] != hash[idx]) {
            comp_mismatch = 1;
        }
    }

    if (comp_mismatch == 1) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_SIGNATURE;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_hash_abort(psa_hash_operation_t *operation)
{
    enum tfm_crypto_err_t err;
    struct tfm_hash_operation_s *ctx = NULL;

    /* Validate pointers */
    err = tfm_crypto_memory_check(operation,
                                  sizeof(psa_hash_operation_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Look up the corresponding operation context */
    err = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                      operation->handle,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Clear the Mbed TLS message digest context */
    mbedtls_md_free(&(ctx->md));

    /* Release the operation context */
    err = tfm_crypto_operation_release(&(operation->handle));
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}
/*!@}*/
