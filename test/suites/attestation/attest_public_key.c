/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attest_public_key.h"
#include "psa_initial_attestation_api.h"
#include "psa_crypto.h"
#include <stdint.h>

/*!
 * \def ECC_CURVE_SECP256R1_PULBIC_KEY_LENGTH
 *
 * \brief Calculates the size of ECC public key is bytes based on the bit size
 *        of the curve
 */
#define ECC_CURVE_SECP256R1_PULBIC_KEY_LENGTH (1 + 2 * PSA_BITS_TO_BYTES(256))

/*!
 *   Byte string representation of ECC public key according to
 *   psa_export_public_key() in interface/include/psa_crypto.h
 */
struct ecc_public_key_t {
    const uint8_t a;
    uint8_t public_key[]; /* X-coordinate || Y-coordinate */
};

/*!
 * Public key part of the example ECC key pair for initial attestation. It is
 * used to validate the signature of the attestation token.
 *
 * Note: This key must be in sync with the corresponding private part,
 *       otherwise attestation test cases will fail.
 *
 * Example key is stored here:
 *  - platform/ext/common/tfm_initial_attestation_key_material.c
 *  - platform/ext/common/tfm_initial_attestation_key.pem
 *
 * Example key type is: NIST P-256 also known as secp256r1
 */
static const struct ecc_public_key_t attest_public_key = {
     /* Constant byte */
     0x04,
     /* X-coordinate */
     {0xDC, 0xF0, 0xD0, 0xF4, 0xBC, 0xD5, 0xE2, 0x6A,
      0x54, 0xEE, 0x36, 0xCA, 0xD6, 0x60, 0xD2, 0x83,
      0xD1, 0x2A, 0xBC, 0x5F, 0x73, 0x07, 0xDE, 0x58,
      0x68, 0x9E, 0x77, 0xCD, 0x60, 0x45, 0x2E, 0x75,
     /* Y-coordinate */
      0x8C, 0xBA, 0xDB, 0x5F, 0xE9, 0xF8, 0x9A, 0x71,
      0x07, 0xE5, 0xA2, 0xE8, 0xEA, 0x44, 0xEC, 0x1B,
      0x09, 0xB7, 0xDA, 0x2A, 0x1A, 0x82, 0xA0, 0x25,
      0x2A, 0x4C, 0x1C, 0x26, 0xEE, 0x1E, 0xD7, 0xCF},
};
/*!
 * \var ecc_curve
 *
 * \brief Key type of the example initial attestation key.
 *
 * Note: This must be updated if example key is replaced with other type.
 */
static psa_ecc_curve_t ecc_curve = PSA_ECC_CURVE_SECP256R1;

/*!
 * \var public_key_registered
 *
 * \brief Indicates whether the public part of the attestation key pair was
 *        registered to Crypto service or not.
 */
static uint32_t public_key_registered = 0;

enum psa_attest_err_t
attest_register_initial_attestation_public_key(psa_key_handle_t *public_key)
{
    psa_key_type_t attest_key_type;
    psa_status_t crypto_res;
    psa_key_policy_t policy = psa_key_policy_init();

    /* Public key should be unregistered at this point */
    if (public_key_registered != 0) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Allocate a transient key for the public key in the Crypto service */
    crypto_res = psa_allocate_key(public_key);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Setup the key policy for public key */
    psa_key_policy_set_usage(&policy,
                             PSA_KEY_USAGE_VERIFY,
                             PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    crypto_res = psa_set_key_policy(*public_key, &policy);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Set key type for public key */
    attest_key_type = PSA_KEY_TYPE_ECC_PUBLIC_KEY(ecc_curve);

    /* Register public key to Crypto service */
    crypto_res = psa_import_key(*public_key,
                                attest_key_type,
                                (const uint8_t *)&attest_public_key,
                                ECC_CURVE_SECP256R1_PULBIC_KEY_LENGTH);

    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }
    public_key_registered = 1;

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t
attest_unregister_initial_attestation_public_key(psa_key_handle_t public_key)
{
    psa_status_t crypto_res;

    if (public_key_registered != 1) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    crypto_res = psa_destroy_key(public_key);
    if (crypto_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }
    public_key_registered = 0;

    return PSA_ATTEST_ERR_SUCCESS;
}
