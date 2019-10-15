/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "t_cose_crypto.h"
#include "attestation_key.h"
#include "tfm_plat_crypto_keys.h"
#include "tfm_memory_utils.h"
#include "psa/crypto.h"

/* Avoid compiler warning due to unused argument */
#define ARG_UNUSED(arg) (void)(arg)

enum t_cose_err_t
t_cose_crypto_pub_key_sign(int32_t cose_alg_id,
                           int32_t key_select,
                           struct q_useful_buf_c hash_to_sign,
                           struct q_useful_buf signature_buffer,
                           struct q_useful_buf_c *signature)
{
    enum t_cose_err_t cose_ret = T_COSE_SUCCESS;
    enum psa_attest_err_t attest_ret;
    psa_status_t psa_ret;
    const size_t sig_size = t_cose_signature_size(cose_alg_id);
    psa_key_handle_t private_key;

    ARG_UNUSED(key_select);

    if (sig_size > signature_buffer.len) {
        return T_COSE_ERR_SIG_BUFFER_SIZE;
    }

    /* FixMe: Registration of key(s) should not be done by attestation service.
     *        Later Crypto service is going to get the attestation key from
     *        platform layer.
     */
    attest_ret =
        attest_get_initial_attestation_private_key_handle(&private_key);
    if (attest_ret != PSA_ATTEST_ERR_SUCCESS) {
        return T_COSE_ERR_FAIL;
    }

    psa_ret = psa_asymmetric_sign(private_key,
                                  PSA_ALG_ECDSA(PSA_ALG_SHA_256),
                                  hash_to_sign.ptr,
                                  hash_to_sign.len,
                                  signature_buffer.ptr, /* Sig buf */
                                  signature_buffer.len, /* Sig buf size */
                                  &(signature->len));   /* Sig length */

    if (psa_ret != PSA_SUCCESS) {
        cose_ret = T_COSE_ERR_FAIL;
    } else {
        signature->ptr = signature_buffer.ptr;
    }

    return cose_ret;
}

#ifdef INCLUDE_TEST_CODE_AND_KEY_ID /* Remove them from release build */

/**
 * \brief     Map PSA curve types to the curve type definition by RFC8152
 *            (COSE).
 *
 * \param[in] psa_curve PSA curve type definition \ref psa_ecc_curve_t.
 *
 * \return    Return COSE curve type according to \ref cose_ecc_curve_t. If
 *            mapping is not possible then return with -1.
 */
static inline enum cose_ecc_curve_t
cose_map_psa_elliptic_curve_type(psa_ecc_curve_t psa_curve)
{
    enum cose_ecc_curve_t cose_curve;

    /* FixMe: Mapping is not complete, missing ones: ED25519, ED448 */
    switch (psa_curve) {
    case PSA_ECC_CURVE_SECP256R1:
        cose_curve = P_256;
        break;
    case PSA_ECC_CURVE_SECP384R1:
        cose_curve = P_384;
        break;
    case PSA_ECC_CURVE_SECP521R1:
        cose_curve = P_521;
        break;
    case PSA_ECC_CURVE_CURVE25519:
        cose_curve = X25519;
        break;
    case PSA_ECC_CURVE_CURVE448:
        cose_curve = X448;
        break;
    default:
        cose_curve = -1;
    }

    return cose_curve;
}

enum t_cose_err_t
t_cose_crypto_get_ec_pub_key(int32_t key_select,
                             struct q_useful_buf_c kid,
                             int32_t *cose_curve_id,
                             struct q_useful_buf buf_to_hold_x_coord,
                             struct q_useful_buf buf_to_hold_y_coord,
                             struct q_useful_buf_c *x_coord,
                             struct q_useful_buf_c *y_coord)
{
    enum psa_attest_err_t attest_res;
    uint8_t *public_key;
    size_t key_len;
    uint32_t key_coord_len;
    psa_ecc_curve_t psa_curve;

    attest_res =
        attest_get_initial_attestation_public_key(&public_key, &key_len,
                                                  &psa_curve);
    if (attest_res != PSA_ATTEST_ERR_SUCCESS) {
        return T_COSE_ERR_FAIL;
    }

    *cose_curve_id = cose_map_psa_elliptic_curve_type(psa_curve);

    /* Key is made up of a 0x4 byte and two coordinates */
    key_coord_len = (key_len - 1) / 2;

    /* Place they key parts into the x and y buffers. Stars at index 1 to skip
     * the 0x4 byte.
     */
    *x_coord = q_useful_buf_copy_ptr(buf_to_hold_x_coord,
                                     &public_key[1],
                                     key_coord_len);

    *y_coord = q_useful_buf_copy_ptr(buf_to_hold_y_coord,
                                     &public_key[1 + key_coord_len],
                                     key_coord_len);

    if(q_useful_buf_c_is_null(*x_coord) || q_useful_buf_c_is_null(*y_coord)) {
        return T_COSE_ERR_KEY_BUFFER_SIZE;
    }

    return T_COSE_SUCCESS;
}
#endif /* INCLUDE_TEST_CODE_AND_KEY_ID */
