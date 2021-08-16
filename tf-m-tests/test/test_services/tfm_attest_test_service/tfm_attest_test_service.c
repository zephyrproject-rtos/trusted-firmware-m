/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "psa/crypto.h"
#include "psa/error.h"
#include "psa/service.h"
#include "tfm_plat_defs.h"
#include "tfm_plat_crypto_keys.h"
#include "tfm_memory_utils.h"
#ifdef TFM_PSA_API
#include "psa/client.h"
#include "tfm_api.h"
#include "psa_manifest/tfm_attest_test_service.h"
#else /* TFM_PSA_API */
#include "tfm_veneers.h"
#endif /* TFM_PSA_API */

/*
 * NOTE
 *
 * This Initial Attestation test service partition provide a secure API to
 * enable tests to fetch Initial Attestation public key.
 *
 * This test service partition shall be only selected when flag
 * ATTEST_TEST_GET_PUBLIC_KEY is enabled. ATTEST_TEST_GET_PUBLIC_KEY is enabled
 * when Initial Attestation public key can only be fetched by test case in
 * runtime.
 *
 * For example, some develop boards are provisioned in runtime with a random
 * Initial Attestation key pair, whose public key is not pre-defined. The public
 * key is unkown to the attestation verifier. Therefore the public key data can
 * only be read for IAT verification in runtime.
 *
 * Integrating this test service will add the following into system
 * - A PRoT service to read Initial Attestation key and export the public part.
 * - Static data and stack belonging to the PRoT service.
 * - An interface for non-secure test and secure test to read the public key
 *   out.
 */

/*
 * NOTE
 *
 * This partition is for TF-M test purpose. Do NOT integrate it into any
 * production or actual usage scenario.
 */

#define ECC_P256_PUBLIC_KEY_SIZE PSA_KEY_EXPORT_ECC_PUBLIC_KEY_MAX_SIZE(256)

/**
 * The size of X and Y coordinate in 2 parameter style EC public
 * key. Format is as defined in [COSE (RFC 8152)]
 * (https://tools.ietf.org/html/rfc8152) and [SEC 1: Elliptic Curve
 * Cryptography](http://www.secg.org/sec1-v2.pdf).
 *
 * This size is well-known and documented in public standards.
 */
#define ECC_P256_COORD_SIZE PSA_BITS_TO_BYTES(256) /* 256 bits -> 32 bytes */

static uint8_t attestation_public_key[ECC_P256_PUBLIC_KEY_SIZE];
static size_t  attestation_public_key_len = 0;
static psa_ecc_family_t attestation_key_curve;

static bool is_public_key_exported = false;

static psa_status_t attest_export_public_key(void)
{
    enum tfm_plat_err_t plat_res;
    psa_ecc_family_t psa_curve;
    struct ecc_key_t attest_key = {0};
    uint8_t key_buf[3 * ECC_P256_COORD_SIZE]; /* priv + x_coord + y_coord */
    psa_key_handle_t key_handle;
    psa_key_attributes_t key_attributes = psa_key_attributes_init();
    psa_status_t res;

    /* Get the initial attestation key */
    plat_res = tfm_plat_get_initial_attest_key(key_buf, sizeof(key_buf),
                                               &attest_key, &psa_curve);

    /* Check the availability of the private key */
    if (plat_res != TFM_PLAT_ERR_SUCCESS || attest_key.priv_key == NULL) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    /* Setup the key policy for private key */
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_SIGN_HASH);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(psa_curve));

    /* Register private key to Crypto service */
    res = psa_import_key(&key_attributes, attest_key.priv_key,
                         attest_key.priv_key_size, &key_handle);
    if (res != PSA_SUCCESS) {
        return res;
    }

    res = psa_export_public_key(key_handle, attestation_public_key,
                                ECC_P256_PUBLIC_KEY_SIZE,
                                &attestation_public_key_len);
    if (res != PSA_SUCCESS) {
        goto done;
    }

    attestation_key_curve = psa_curve;

done:
    res = psa_destroy_key(key_handle);
    return res;
}

#ifdef TFM_PSA_API
static psa_status_t attest_test_get_public_key(const psa_msg_t *msg)
{
    psa_status_t err;

    /* Skip the export operation if the public key has been already exported. */
    if (!is_public_key_exported) {
        err = attest_export_public_key();
        if (err != PSA_SUCCESS) {
            return err;
        }

        is_public_key_exported = true;
    }

    if (msg->out_size[0] < attestation_public_key_len) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if ((msg->out_size[1] != sizeof(attestation_key_curve)) ||
        (msg->out_size[2] != sizeof(attestation_public_key_len))) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    psa_write(msg->handle, 0, attestation_public_key,
              attestation_public_key_len);
    psa_write(msg->handle, 1, &attestation_key_curve,
              sizeof(attestation_key_curve));
    psa_write(msg->handle, 2, &attestation_public_key_len,
              sizeof(attestation_public_key_len));

    return PSA_SUCCESS;
}

static void attest_signal_handle(psa_signal_t signal)
{
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(signal, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        status = attest_test_get_public_key(&msg);
        psa_reply(msg.handle, status);
        break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        psa_panic();
    }
}

psa_status_t tfm_attest_test_service_init(void)
{
    psa_signal_t signals;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_ATTEST_TEST_GET_PUBLIC_KEY_SIGNAL) {
            attest_signal_handle(TFM_ATTEST_TEST_GET_PUBLIC_KEY_SIGNAL);
        } else {
            psa_panic();
        }
    }

    return PSA_ERROR_SERVICE_FAILURE;
}
#else /* TFM_PSA_API */
psa_status_t tfm_attest_test_get_public_key(const psa_invec *in_vec,
                                            uint32_t num_invec,
                                            psa_outvec *out_vec,
                                            uint32_t num_outvec)
{
    uint8_t *key_buf;
    size_t key_len;
    psa_status_t res;

    (void)in_vec;

    if (num_invec != 0 || num_outvec != 3) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Skip the export operation if the public key has been already exported. */
    if (!is_public_key_exported) {
        res = attest_export_public_key();
        if (res != PSA_SUCCESS) {
            return res;
        }

        is_public_key_exported = true;
    }

    key_buf = out_vec[0].base;
    key_len = out_vec[0].len;

    if (out_vec[1].len != sizeof(attestation_key_curve) ||
        out_vec[2].len != sizeof(attestation_public_key_len)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (key_len < attestation_public_key_len) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    (void)tfm_memcpy(key_buf, attestation_public_key,
                     attestation_public_key_len);

    *(psa_ecc_family_t *)out_vec[1].base = attestation_key_curve;

    *(size_t *)out_vec[2].base = attestation_public_key_len;

    return PSA_SUCCESS;
}

psa_status_t tfm_attest_test_service_init(void)
{
    return PSA_SUCCESS;
}
#endif /* TFM_PSA_API */
