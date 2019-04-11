/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_crypto_api.h"
#include "crypto_engine.h"
#include "tfm_crypto_struct.h"

/* FixMe: Use PSA_CONNECTION_REFUSED when performing parameter
 *        integrity checks but this will have to be revised
 *        when the full set of error codes mandated by PSA FF
 *        is available.
 */

/**
 * \brief Release all resources associated with a hash operation.
 *
 * \param[in] operation  Frontend hash operation context
 * \param[in] ctx        Backend hash operation context
 *
 * \return Return values as described in \ref tfm_crypto_err_t
 */
static psa_status_t tfm_crypto_hash_release(uint32_t *handle,
                                            struct tfm_hash_operation_s *ctx)
{
    psa_status_t status = PSA_SUCCESS;

    /* Release resources in the engine */
    status = tfm_crypto_engine_hash_release(&(ctx->engine_ctx));
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Release the operation context */
    return tfm_crypto_operation_release(handle);
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_hash_setup(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_hash_operation_s *ctx = NULL;
    struct hash_engine_info engine_info;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((out_vec[0].len != sizeof(uint32_t)) ||
        (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;
    psa_algorithm_t alg = iov->alg;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->handle;

    if (PSA_ALG_IS_HASH(alg) == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Setup the engine for the requested algorithm */
    status = tfm_crypto_engine_hash_setup(alg, &engine_info);
    if (status != PSA_SUCCESS) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Allocate the operation context in the secure world */
    status = tfm_crypto_operation_alloc(TFM_CRYPTO_HASH_OPERATION,
                                        &handle,
                                        (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    *handle_out = handle;

    /* Bind the algorithm to the hash context */
    ctx->alg = alg;

    /* Start the engine */
    status = tfm_crypto_engine_hash_start(&(ctx->engine_ctx), &engine_info);
    if (status != PSA_SUCCESS) {
        /* Release the operation context, ignore if the operation fails. */
      (void)tfm_crypto_hash_release(&handle, ctx);
        return status;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_hash_update(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_hash_operation_s *ctx = NULL;

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *input = in_vec[1].base;
    size_t input_length = in_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         handle,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Process the input chunk with the engine */
    status = tfm_crypto_engine_hash_update(&(ctx->engine_ctx),
                                           input,
                                           input_length);
    if (status != PSA_SUCCESS) {
        if (tfm_crypto_hash_release(&handle, ctx) == PSA_SUCCESS) {
            *handle_out = handle;
        }
        return status;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_hash_finish(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_hash_operation_s *ctx = NULL;

    if ((in_len != 1) || (out_len != 2)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;
    uint8_t *hash = out_vec[1].base;
    size_t hash_size = out_vec[1].len;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->handle;

    /* Initialise hash_length to zero */
    out_vec[1].len = 0;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         handle,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (hash_size < PSA_HASH_SIZE(ctx->alg)) {
        if (tfm_crypto_hash_release(&handle, ctx) == PSA_SUCCESS) {
            *handle_out = handle;
        }
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Finalise the hash value using the engine */
    status = tfm_crypto_engine_hash_finish(&(ctx->engine_ctx), hash);
    if (status != PSA_SUCCESS) {
        if (tfm_crypto_hash_release(&handle, ctx) == PSA_SUCCESS) {
            *handle_out = handle;
        }
        return status;
    }

    /* Set the length of the hash that has been produced */
    out_vec[1].len = PSA_HASH_SIZE(ctx->alg);

    status = tfm_crypto_hash_release(&handle, ctx);
    if (status == PSA_SUCCESS) {
        *handle_out = handle;
    }
    return status;
}

static psa_status_t _psa_hash_finish(uint32_t *handle,
                                     uint8_t *hash,
                                     size_t hash_size,
                                     size_t *hash_length)
{
    psa_status_t status;
    struct tfm_crypto_pack_iovec iov = {
        .sfn_id = TFM_CRYPTO_HASH_FINISH_SFID,
        .handle = *handle,
    };

    psa_invec in_vec[] = {
        {.base = &iov, .len = sizeof(struct tfm_crypto_pack_iovec)},
    };
    psa_outvec out_vec[] = {
        {.base = handle, .len = sizeof(uint32_t)},
        {.base = hash, .len = hash_size},
    };

    status = tfm_crypto_hash_finish(in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                   out_vec, sizeof(out_vec)/sizeof(out_vec[0]));
    *hash_length = out_vec[1].len;

    return status;
}

psa_status_t tfm_crypto_hash_verify(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    uint8_t digest[PSA_HASH_MAX_SIZE] = {0};
    size_t digest_length;
    uint32_t idx, comp_mismatch = 0;

    if ((in_len != 2) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    uint32_t *handle_out = out_vec[0].base;
    const uint8_t *hash = in_vec[1].base;
    size_t hash_length = in_vec[1].len;

    /* Finalise the hash operation */
    status = _psa_hash_finish(handle_out,
                              digest,
                              PSA_HASH_MAX_SIZE,
                              &digest_length);

    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Check that the computed hash has the same legnth as the provided one */
    if (hash_length != digest_length) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    /* Verify that the computed hash matches the provided one */
    for (idx=0; idx<(uint32_t)digest_length; idx++) {
        if (digest[idx] != hash[idx]) {
            comp_mismatch = 1;
        }
    }

    if (comp_mismatch == 1) {
        return PSA_ERROR_INVALID_SIGNATURE;
    }

    return PSA_SUCCESS;
}

psa_status_t tfm_crypto_hash_abort(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    struct tfm_hash_operation_s *ctx = NULL;

    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(uint32_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;
    uint32_t handle = iov->handle;
    uint32_t *handle_out = out_vec[0].base;

    /* Init the handle in the operation with the one passed from the iov */
    *handle_out = iov->handle;

    /* Look up the corresponding operation context */
    status = tfm_crypto_operation_lookup(TFM_CRYPTO_HASH_OPERATION,
                                         handle,
                                         (void **)&ctx);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = tfm_crypto_hash_release(&handle, ctx);
    if (status == PSA_SUCCESS) {
        *handle_out = handle;
    }
    return status;
}
/*!@}*/
