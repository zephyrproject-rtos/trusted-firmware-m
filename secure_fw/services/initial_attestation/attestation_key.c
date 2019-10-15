/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attestation_key.h"
#include <stdint.h>
#include <stddef.h>
#include "psa/initial_attestation.h"
#include "platform/include/tfm_plat_defs.h"
#include "platform/include/tfm_plat_crypto_keys.h"

/* 0 is defined as an invalid handle in the PSA spec, so it is used here to
 * indicate that the key isn't loaded.
 */
#define ATTEST_KEY_HANDLE_NOT_LOADED 0

/**
 * Global key handle for the attestation key. Used to prevent keys having to be
 * reloaded multiple times during a single token operation.
 */
static psa_key_handle_t attestation_key_handle = ATTEST_KEY_HANDLE_NOT_LOADED;

/**
 * The public key is kept loaded as it is both not required to be secret (and
 * hence can be kept in attestation memory) and immutable.
 */
static uint8_t  attestation_public_key[ECC_P_256_KEY_SIZE];
static size_t   attestation_public_key_len = 0;
static psa_ecc_curve_t attestation_key_curve;

/**
 * \brief     Map the curve type definition by RFC8152 (COSE) to PSA curve
 *            types.
 *
 * \param[in] cose_curve  COSE curve type definition \ref ecc_curve_t.
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
attest_register_initial_attestation_key()
{
    enum tfm_plat_err_t plat_res;
    psa_ecc_curve_t psa_curve;
    enum ecc_curve_t cose_curve;
    struct ecc_key_t attest_key = {0};
    uint8_t  key_buf[ECC_P_256_KEY_SIZE];
    psa_key_type_t attest_key_type;
    psa_key_handle_t key_handle;
    psa_status_t crypto_res;
    psa_key_policy_t policy = psa_key_policy_init();

    if (attestation_key_handle != ATTEST_KEY_HANDLE_NOT_LOADED) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Allocate a transient key for the private key in the Crypto service */
    crypto_res = psa_allocate_key(&key_handle);
    if (crypto_res != PSA_SUCCESS) {
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
    psa_key_policy_set_usage(&policy,
                             PSA_KEY_USAGE_SIGN,
                             PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    crypto_res = psa_set_key_policy(key_handle, &policy);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Set key type for private key */
    attest_key_type = PSA_KEY_TYPE_ECC_KEYPAIR(psa_curve);

    /* Register private key to Crypto service */
    crypto_res = psa_import_key(key_handle,
                                attest_key_type,
                                attest_key.priv_key,
                                attest_key.priv_key_size);

    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    attestation_key_handle = key_handle;

    /* If the public key length is 0 then it hasn't been loaded */
    if (attestation_public_key_len == 0) {
        crypto_res = psa_export_public_key(key_handle, attestation_public_key,
                                           ECC_P_256_KEY_SIZE,
                                           &attestation_public_key_len);
        if (crypto_res != PSA_SUCCESS) {
            return PSA_ATTEST_ERR_GENERAL;
        }

        attestation_key_curve = psa_curve;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t
attest_unregister_initial_attestation_key()
{
    psa_status_t crypto_res;

    if (attestation_key_handle == ATTEST_KEY_HANDLE_NOT_LOADED) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    crypto_res = psa_destroy_key(attestation_key_handle);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    attestation_key_handle = ATTEST_KEY_HANDLE_NOT_LOADED;

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t
attest_get_initial_attestation_private_key_handle(psa_key_handle_t *handle)
{
    if (attestation_key_handle == ATTEST_KEY_HANDLE_NOT_LOADED) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    *handle = attestation_key_handle;

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t
attest_get_initial_attestation_public_key(uint8_t **public_key,
                                          size_t *public_key_len,
                                          psa_ecc_curve_t *public_key_curve)
{

    /* If the public key length is 0 then it hasn't been loaded */
    if (attestation_public_key_len == 0) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    *public_key       = attestation_public_key;
    *public_key_len   = attestation_public_key_len;
    *public_key_curve = attestation_key_curve;

    return PSA_ATTEST_ERR_SUCCESS;
}
