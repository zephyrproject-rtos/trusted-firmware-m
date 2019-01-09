/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attestation_key.h"
#include <stdint.h>
#include <stddef.h>
#include "tfm_crypto_defs.h"
#include "psa_crypto.h"
#include "psa_crypto_platform.h"
#include "psa_initial_attestation_api.h"
#include "platform/include/tfm_plat_defs.h"
#include "platform/include/tfm_plat_crypto_keys.h"

/*!
 * \var private_key_registered
 *
 * \brief Indicates whether the private part of the attestation key pair was
 *        registered to crypto service or not.
 */
static uint32_t private_key_registered = 0;

/*!
 * \var public_key_registered
 *
 * \brief Indicates whether the public part of the attestation key pair was
 *        registered to crypto service or not.
 */
static uint32_t public_key_registered = 0;

/**
 * \brief Map the curve type definition by RFC8152 (COSE) to PSA curve types
 *
 * \param[in]  cose_curve  COSE curve type definition \ref ecc_curve_t.
 *
 * \return    Return PSA curve type according to \ref psa_ecc_curve_t. If
 *            mapping is not possible then return with USHRT_MAX.
 */
static inline psa_ecc_curve_t
attest_map_elliptic_curve_type(enum ecc_curve_t cose_curve)
{
    psa_ecc_curve_t psa_curve;

    /*FixMe: Mapping is not complete, missing ones: ED25519, ED448 */
    switch (cose_curve) {
    case P_256:
        psa_curve = PSA_ECC_CURVE_SECP256R1;
        break;
    case P_384:
        psa_curve = PSA_ECC_CURVE_SECP384R1;
        break;
    case P_521:
        psa_curve = PSA_ECC_CURVE_SECP521R1;
        break;
    case X25519:
        psa_curve = PSA_ECC_CURVE_CURVE25519;
        break;
    case X448:
        psa_curve = PSA_ECC_CURVE_CURVE448;
        break;
    default:
        psa_curve = USHRT_MAX;
    }

    return psa_curve;
}

enum psa_attest_err_t
attest_register_initial_attestation_key(void)
{
    enum tfm_plat_err_t plat_res;
    psa_ecc_curve_t psa_curve;
    enum ecc_curve_t cose_curve;
    struct ecc_key_t attest_key = {0};
    uint8_t  key_buf[ECC_P_256_KEY_SIZE];
    psa_key_type_t attest_key_type;
    size_t public_key_size;
    psa_status_t crypto_res;
    psa_key_policy_t policy;

    /* Key(s) should be unregistered at this point */
    if (private_key_registered != 0 || public_key_registered != 0) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Get the initial attestation key */
    plat_res = tfm_plat_get_initial_attest_key(key_buf, sizeof(key_buf),
                                               &attest_key, &cose_curve);

    /* Check the availability of the private key */
    if (plat_res != TFM_PLAT_ERR_SUCCESS || attest_key.priv_key == NULL) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Mapping of COSE curve type to PSA curve types */
    psa_curve = attest_map_elliptic_curve_type(cose_curve);
    if (psa_curve == USHRT_MAX) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Setup the key policy for private key */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, PSA_KEY_USAGE_SIGN, 0); /* FixMe: alg */
    crypto_res = psa_set_key_policy((psa_key_slot_t)ATTEST_PRIVATE_KEY_SLOT,
                                    &policy);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Set key type for private key */
    /* FixMe: Use PSA_KEY_TYPE_ECC_KEYPAIR(psa_curve) when ECC key type will be
     * supported in Crypto service
     */
    attest_key_type = PSA_KEY_TYPE_RAW_DATA;

    /* Register private key to crypto service */
    crypto_res = psa_import_key((psa_key_slot_t)ATTEST_PRIVATE_KEY_SLOT,
                                attest_key_type,
                                attest_key.priv_key,
                                attest_key.priv_key_size);

    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }
    private_key_registered = 1;

    /* Check whether public key is available, not mandatory */
    if (attest_key.pubx_key == NULL) {
        return PSA_ATTEST_ERR_SUCCESS;
    }

    /* Setup the key policy for public key */
    psa_key_policy_init(&policy);
    psa_key_policy_set_usage(&policy, PSA_KEY_USAGE_VERIFY, 0); /* FixMe: alg */
    crypto_res = psa_set_key_policy((psa_key_slot_t)ATTEST_PUBLIC_KEY_SLOT,
                                    &policy);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Set key type for public key */
    /* FixMe: Use PSA_KEY_TYPE_ECC_PUBLIC_KEY(psa_curve) when ECC key type will
     * be supported in Crypto service
     */
    attest_key_type = PSA_KEY_TYPE_RAW_DATA;

    /* Register public key to crypto service */
    public_key_size = attest_key.pubx_key_size + attest_key.puby_key_size;

    crypto_res = psa_import_key((psa_key_slot_t)ATTEST_PUBLIC_KEY_SLOT,
                                attest_key_type,
                                attest_key.pubx_key,
                                public_key_size);

    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }
    public_key_registered = 1;

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t
attest_unregister_initial_attestation_key(void)
{
    psa_status_t crypto_res;

    /* Only private key is mandatory */
    if (private_key_registered != 1) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    crypto_res = psa_destroy_key((psa_key_slot_t)ATTEST_PRIVATE_KEY_SLOT);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }
    private_key_registered = 0;

    if (public_key_registered) {
        crypto_res = psa_destroy_key((psa_key_slot_t)ATTEST_PUBLIC_KEY_SLOT);
        if (crypto_res != PSA_SUCCESS) {
            return PSA_ATTEST_ERR_GENERAL;
        }
        public_key_registered = 0;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}
