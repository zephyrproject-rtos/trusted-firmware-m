/*
 * t_cose_psa_crypto_hash.c
 *
 * Copyright 2019, Laurence Lundblade
 *
 * Copyright (c) 2019, Arm Limited.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#include "t_cose_crypto.h"
#include "psa_crypto.h"

/**
 * \brief Context for PSA hash adaptation.
 *
 * Hash context for PSA hash implementation. This is fit into and cast
 * to/from struct \ref t_cose_crypto_hash.
 */
struct t_cose_psa_crypto_hash {
    psa_status_t         status;
    psa_hash_operation_t operation;
};

/**
 * \brief Check some of the sizes for hash implementation.
 *
 * \return  Value from \ref t_cose_err_t error if sizes are not correct.
 *
 * It makes sure the constants in the header file match the local
 * implementation.  This gets evaluated at compile time and will
 * optimize out to nothing when all checks pass.
 */
static inline enum t_cose_err_t check_hash_sizes(void)
{
    if (T_COSE_CRYPTO_SHA256_SIZE != PSA_HASH_SIZE(PSA_ALG_SHA_256)) {
        return T_COSE_ERR_HASH_GENERAL_FAIL;
    }

    return T_COSE_SUCCESS;
}

/**
 * \brief Convert COSE algorithm ID to a PSA algorithm ID
 *
 * \param[in] cose_hash_alg_id   The COSE-based ID for the
 *
 * \return PSA-based hash algorithm ID, or MD4 in the case of error.
 *
 */
static inline psa_algorithm_t cose_hash_alg_id_to_psa(int32_t cose_hash_alg_id)
{
    psa_algorithm_t return_value;

    switch (cose_hash_alg_id) {
    case COSE_ALG_SHA256_PROPRIETARY:
        return_value = PSA_ALG_SHA_256;
        break;
    default:
        return_value = PSA_ALG_MD4;
        break;
    }

    return return_value;
}

enum t_cose_err_t
t_cose_crypto_hash_start(struct t_cose_crypto_hash *hash_ctx,
                         int32_t cose_hash_alg_id)
{
    enum t_cose_err_t cose_ret = T_COSE_SUCCESS;
    psa_status_t psa_ret;
    struct t_cose_psa_crypto_hash *psa_hash_ctx;

    /* These next 3 lines optimize to nothing except when there is
     * failure.
     */
    cose_ret = check_hash_sizes();
    if (cose_ret) {
        return cose_ret;
    }

    /* This mostly turns into a compile-time check. If it succeeds,
     * then it optimizes out to nothing. If it fails it will
     * short-circuit this function to always create an error.
     */
    if (sizeof(struct t_cose_crypto_hash) <
       sizeof(struct t_cose_psa_crypto_hash)) {
        return T_COSE_ERR_HASH_GENERAL_FAIL;
    }
    psa_hash_ctx = (struct t_cose_psa_crypto_hash *)hash_ctx;

    psa_ret = psa_hash_setup(&psa_hash_ctx->operation,
                             cose_hash_alg_id_to_psa(cose_hash_alg_id));

    if (psa_ret == PSA_SUCCESS) {
        psa_hash_ctx->status = PSA_SUCCESS;
        cose_ret = T_COSE_SUCCESS;
    } else if (psa_ret == PSA_ERROR_NOT_SUPPORTED) {
        cose_ret = T_COSE_ERR_UNSUPPORTED_HASH;
    } else {
        cose_ret = T_COSE_ERR_HASH_GENERAL_FAIL;
    }

    return cose_ret;
}

void t_cose_crypto_hash_update(struct t_cose_crypto_hash *hash_ctx,
                               struct q_useful_buf_c data_to_hash)
{
    struct t_cose_psa_crypto_hash *psa_hash_ctx;

    /* This mostly turns into a compile-time check. If it succeeds,
     * then it optimizes out to nothing. If it fails it will
     * short-circuit this function to always create an error.
     */
    if (sizeof(struct t_cose_crypto_hash) <
       sizeof(struct t_cose_psa_crypto_hash)) {
        return;
    }
    psa_hash_ctx = (struct t_cose_psa_crypto_hash *)hash_ctx;

    if (psa_hash_ctx->status == PSA_SUCCESS) {
        if (data_to_hash.ptr != NULL) {
            psa_hash_ctx->status = psa_hash_update(&psa_hash_ctx->operation,
                                                   data_to_hash.ptr,
                                                   data_to_hash.len);
        } else {
            /* No data was passed in to be hashed indicating the mode of use is
             * the computation of the size of hash. This mode is hashing is used
             * by t_cose when it is requested to compute the size of the signed
             * data it might compute, which is in turn used to compute the
             * size of a would be token. When computing the size, the size
             * like this, there is nothing to do in update()
             */
        }
    }
}

enum t_cose_err_t
t_cose_crypto_hash_finish(struct t_cose_crypto_hash *hash_ctx,
                          struct q_useful_buf buffer_to_hold_result,
                          struct q_useful_buf_c *hash_result)
{
    enum t_cose_err_t cose_ret = T_COSE_SUCCESS;
    psa_status_t psa_ret;
    struct t_cose_psa_crypto_hash *psa_hash_ctx;

    /* This mostly turns into a compile-time check. If it succeeds,
     * then it optimizes out to nothing. If it fails it will
     * short-circuit this function to always create an error.
     */
    if (sizeof(struct t_cose_crypto_hash) <
       sizeof(struct t_cose_psa_crypto_hash)) {
        return T_COSE_ERR_HASH_GENERAL_FAIL;
    }
    psa_hash_ctx = (struct t_cose_psa_crypto_hash *)hash_ctx;

    if (psa_hash_ctx->status == PSA_SUCCESS) {
        psa_ret = psa_hash_finish(&psa_hash_ctx->operation,
                                  buffer_to_hold_result.ptr,
                                  buffer_to_hold_result.len,
                                  &(hash_result->len));

        if (psa_ret == PSA_SUCCESS) {
            hash_result->ptr = buffer_to_hold_result.ptr;
            cose_ret = T_COSE_SUCCESS;
        } else if (psa_ret == PSA_ERROR_BUFFER_TOO_SMALL) {
            cose_ret = T_COSE_ERR_HASH_BUFFER_SIZE;
        } else {
            cose_ret = T_COSE_ERR_HASH_GENERAL_FAIL;
        }
    } else {
        cose_ret = T_COSE_ERR_HASH_GENERAL_FAIL;
    }

    return cose_ret;
}
