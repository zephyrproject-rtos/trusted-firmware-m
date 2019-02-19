/*
 * t_cose_psa_crypto.c
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
#include "attestation_key.h"
#include "tfm_plat_defs.h"
#include "tfm_plat_crypto_keys.h"
#include "tfm_memory_utils.h"
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

enum t_cose_err_t
t_cose_crypto_pub_key_sign(int32_t cose_alg_id,
                           int32_t key_select,
                           struct useful_buf_c hash_to_sign,
                           struct useful_buf signature_buffer,
                           struct useful_buf_c *signature)
{
    enum t_cose_err_t cose_ret = T_COSE_SUCCESS;
    enum psa_attest_err_t attest_ret;
    psa_status_t psa_ret;
    const size_t sig_size = t_cose_signature_size(cose_alg_id);

    /* Avoid compiler warning due to unused argument */
    (void)key_select;

    if (sig_size > signature_buffer.len) {
        return T_COSE_ERR_SIG_BUFFER_SIZE;
    }

    /* FixMe: Registration of key(s) should not be error by attestation service.
     *        Later crypto service is going to get the attestation key from
     *        platform layer.
     */
    attest_ret = attest_register_initial_attestation_key();
    if (attest_ret != PSA_ATTEST_ERR_SUCCESS) {
        cose_ret = T_COSE_ERR_FAIL;
        goto error;
    }

    psa_ret = psa_asymmetric_sign(ATTEST_PRIVATE_KEY_SLOT,
                                  0, /* FixMe: algorithm ID */
                                  hash_to_sign.ptr,
                                  hash_to_sign.len,
                                  signature_buffer.ptr, /* Sig buf */
                                  signature_buffer.len, /* Sig buf size */
                                  &(signature->len));   /* Sig length */

    if (psa_ret != PSA_SUCCESS) {
        cose_ret = T_COSE_ERR_FAIL;
        goto error;
    } else {
        signature->ptr = signature_buffer.ptr;
    }

    attest_ret = attest_unregister_initial_attestation_key();
    if (attest_ret != PSA_ATTEST_ERR_SUCCESS) {
        cose_ret = T_COSE_ERR_FAIL;
        goto error;
    }

error:
    return cose_ret;
}

enum t_cose_err_t
t_cose_crypto_get_ec_pub_key(int32_t key_select,
                             struct useful_buf_c kid,
                             int32_t *cose_curve_id,
                             struct useful_buf buf_to_hold_x_coord,
                             struct useful_buf buf_to_hold_y_coord,
                             struct useful_buf_c *x_coord,
                             struct useful_buf_c *y_coord)
{
    enum tfm_plat_err_t plat_res;
    enum ecc_curve_t cose_curve;
    struct ecc_key_t attest_key = {0};
    uint8_t  key_buf[ECC_P_256_KEY_SIZE];

    /* Avoid compiler warning due to unused argument */
    (void)key_select;

    /* Get the initial attestation key */
    plat_res = tfm_plat_get_initial_attest_key(key_buf, sizeof(key_buf),
                                               &attest_key, &cose_curve);

    /* Check the availability of the private key */
    if (plat_res != TFM_PLAT_ERR_SUCCESS ||
        attest_key.pubx_key == NULL ||
        attest_key.puby_key == NULL) {
        return T_COSE_ERR_KEY_BUFFER_SIZE;
    }

    *cose_curve_id = (int32_t)cose_curve;

    /* Check buffer size to avoid overflow */
    if (buf_to_hold_x_coord.len < attest_key.pubx_key_size) {
        return T_COSE_ERR_KEY_BUFFER_SIZE;
    }

    /* Copy the X coordinate of the public key to the buffer */
    tfm_memcpy(buf_to_hold_x_coord.ptr,
               (const void *)attest_key.pubx_key,
               attest_key.pubx_key_size);

    /* Update size */
    buf_to_hold_x_coord.len = attest_key.pubx_key_size;

    /* Check buffer size to avoid overflow */
    if (buf_to_hold_y_coord.len < attest_key.puby_key_size) {
        return T_COSE_ERR_KEY_BUFFER_SIZE;
    }

    /* Copy the Y coordinate of the public key to the buffer */
    tfm_memcpy(buf_to_hold_y_coord.ptr,
               (const void *)attest_key.puby_key,
               attest_key.puby_key_size);

    /* Update size */
    buf_to_hold_y_coord.len = attest_key.puby_key_size;

    x_coord->ptr = buf_to_hold_x_coord.ptr;
    x_coord->len = buf_to_hold_x_coord.len;
    y_coord->ptr = buf_to_hold_y_coord.ptr;
    y_coord->len = buf_to_hold_y_coord.len;

    return T_COSE_SUCCESS;
}

/**
 * \brief Check some of the sizes for hash implementation.
 *
 * \return  Value from \ref t_cose_err_t error if sizes are not correct.
 *
 * It makes sure the constants in the header file match the local
 * implementation.  This gets evaluated at compile time and will
 * optimize out to nothing when all checks pass.
 */
static inline enum t_cose_err_t check_hash_sizes()
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
        goto error;
    }

    /* This mostly turns into a compile-time check. If it succeeds,
     * then it optimizes out to nothing. If it fails it will
     * short-circuit this function to always create an error.
     */
    if (sizeof(struct t_cose_crypto_hash) <
       sizeof(struct t_cose_psa_crypto_hash)) {
        cose_ret = T_COSE_ERR_HASH_GENERAL_FAIL;
        goto error;
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

error:
    return cose_ret;
}

void t_cose_crypto_hash_update(struct t_cose_crypto_hash *hash_ctx,
                               struct useful_buf_c data_to_hash)
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
            /* Intentionally do nothing, just computing the size of the token */
        }
    }
}

enum t_cose_err_t
t_cose_crypto_hash_finish(struct t_cose_crypto_hash *hash_ctx,
                          struct useful_buf buffer_to_hold_result,
                          struct useful_buf_c *hash_result)
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
        cose_ret = T_COSE_ERR_HASH_GENERAL_FAIL;
        goto error;
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

error:
    return cose_ret;
}
