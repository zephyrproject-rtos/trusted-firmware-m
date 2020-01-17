/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2018-2019, Laurence Lundblade.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

#include "attestation_key.h"
#include "platform/include/tfm_plat_crypto_keys.h"
#include "psa/crypto.h"

/* Only support HMAC as MAC algorithm in COSE_Mac0 so far */
#define SYMMETRIC_IAK_MAX_SIZE        PSA_MAC_MAX_SIZE

/* Symmetric IAK handle */
static psa_key_handle_t symmetric_iak_handle = 0;

static psa_status_t destroy_iak(psa_key_handle_t *iak_handle)
{
    psa_status_t res;

    res = psa_destroy_key(*iak_handle);

    *iak_handle = 0;
    return res;
}

enum psa_attest_err_t attest_register_initial_attestation_key(void)
{
    uint8_t key_buf[SYMMETRIC_IAK_MAX_SIZE];
    psa_algorithm_t key_alg;
    psa_key_handle_t key_handle;
    size_t key_len;
    enum tfm_plat_err_t plat_res;
    psa_status_t psa_res;
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;

    if (symmetric_iak_handle) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Get the symmetric initial attestation key for HMAC operation */
    plat_res = tfm_plat_get_symmetric_iak(key_buf, sizeof(key_buf),
                                          &key_len, &key_alg);
    /* In case the buffer size was not checked, although unlikely */
    if (sizeof(key_buf) < key_len) {
        /*
         * Something critical following key_buf may be overwritten.
         * Directly jump into fatal error handling.
         *
         * TODO: Should be replaced by a call to psa_panic() when it
         * becomes available.
         */
        while (1) {
            ;
        }
    }
    if (plat_res != TFM_PLAT_ERR_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /*
     * Verify if HMAC algorithm is valid.
     * According to COSE (RFC 8152), only SHA-256, SHA-384 and SHA-512 are
     * supported in HMAC.
     */
    if ((key_alg != PSA_ALG_HMAC(PSA_ALG_SHA_256)) &&
        (key_alg != PSA_ALG_HMAC(PSA_ALG_SHA_384)) &&
        (key_alg != PSA_ALG_HMAC(PSA_ALG_SHA_512))) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    /* Setup the key attributes */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN);
    psa_set_key_algorithm(&key_attributes, key_alg);
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_HMAC);

    /* Register the symmetric key to Crypto service */
    psa_res = psa_import_key(&key_attributes, key_buf, key_len, &key_handle);
    if (psa_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    symmetric_iak_handle = key_handle;

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t attest_unregister_initial_attestation_key(void)
{
    if (!symmetric_iak_handle) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    destroy_iak(&symmetric_iak_handle);

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t
attest_get_signing_key_handle(psa_key_handle_t *key_handle)
{
    if (!symmetric_iak_handle) {
        return PSA_ATTEST_ERR_GENERAL;
    }

    *key_handle = symmetric_iak_handle;

    return PSA_ATTEST_ERR_SUCCESS;
}
