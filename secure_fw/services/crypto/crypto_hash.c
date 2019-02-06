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

/**
 * \brief Release all resources associated with a hash operation.
 *
 * \param[in] operation  Frontend hash operation context
 * \param[in] ctx        Backend hash operation context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
static enum tfm_crypto_err_t tfm_crypto_hash_release(
                                               psa_hash_operation_t *operation,
                                               struct tfm_hash_operation_s *ctx)
{
    psa_status_t status;
    enum tfm_crypto_err_t err;

    /* Release resources in the engine */
    status = tfm_crypto_engine_hash_release(&(ctx->engine_ctx));
    if (status != PSA_SUCCESS) {
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    /* Release the operation context */
    err = tfm_crypto_operation_release(TFM_CRYPTO_HASH_OPERATION, operation);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_hash_setup(psa_hash_operation_t *operation,
                                            psa_algorithm_t alg)
{
    psa_status_t status = PSA_SUCCESS;
    enum tfm_crypto_err_t err;
    struct tfm_hash_operation_s *ctx = NULL;
    struct hash_engine_info engine_info;

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

    /* Setup the engine for the requested algorithm */
    status = tfm_crypto_engine_hash_setup(alg, &engine_info);
    if (status != PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_NOT_SUPPORTED;
    }

    /* Allocate the operation context in the secure world */
    err = tfm_crypto_operation_alloc(TFM_CRYPTO_HASH_OPERATION,
                                     operation,
                                     (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Bind the algorithm to the hash context */
    ctx->alg = alg;

    /* Start the engine */
    status = tfm_crypto_engine_hash_start(&(ctx->engine_ctx), &engine_info);
    if (status != PSA_SUCCESS) {
        /* Release the operation context */
        (void)tfm_crypto_hash_release(operation, ctx);
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_hash_update(psa_hash_operation_t *operation,
                                             const uint8_t *input,
                                             size_t input_length)
{
    psa_status_t status = PSA_SUCCESS;
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
                                      operation,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    /* Process the input chunk with the engine */
    status = tfm_crypto_engine_hash_update(&(ctx->engine_ctx),
                                           input,
                                           input_length);
    if (status != PSA_SUCCESS) {
        (void)tfm_crypto_hash_release(operation, ctx);
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_hash_finish(psa_hash_operation_t *operation,
                                             uint8_t *hash,
                                             size_t hash_size,
                                             size_t *hash_length)
{
    psa_status_t status = PSA_SUCCESS;
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
                                      operation,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    if (hash_size < PSA_HASH_SIZE(ctx->alg)) {
        (void)tfm_crypto_hash_release(operation, ctx);
        return TFM_CRYPTO_ERR_PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Finalise the hash value using the engine */
    status = tfm_crypto_engine_hash_finish(&(ctx->engine_ctx), hash);
    if (status != PSA_SUCCESS) {
        (void)tfm_crypto_hash_release(operation, ctx);
        return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
    }

    /* Set the length of the hash that has been produced */
    *hash_length = PSA_HASH_SIZE(ctx->alg);

    return tfm_crypto_hash_release(operation, ctx);
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
                                      operation,
                                      (void **)&ctx);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
    }

    return tfm_crypto_hash_release(operation, ctx);
}
/*!@}*/
