/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** \file cc3xx_psa_aead.c
 *
 * This file contains the implementations of the entry points associated to the
 * aead capability (single-part and multipart) as described by the PSA
 * Cryptoprocessor Driver interface specification.
 *
 */

#include "aes_driver.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include <psa/crypto.h>

#include "cc3xx_psa_aead.h"
#include "cc3xx_internal_ccm.h"
#include "cc3xx_internal_chacha20_poly1305.h"
#include "cc3xx_internal_gcm.h"

static psa_status_t check_alg(psa_algorithm_t alg, psa_algorithm_t *ref_alg)
{
    psa_algorithm_t default_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);
    size_t tag_length = PSA_ALG_AEAD_GET_TAG_LENGTH(alg);
    size_t valid_tag_lengths[7];

    *ref_alg = default_alg;
    switch (default_alg) {
    case PSA_ALG_CCM:
        valid_tag_lengths[0] = 4;
        valid_tag_lengths[1] = 6;
        valid_tag_lengths[2] = 8;
        valid_tag_lengths[3] = 10;
        valid_tag_lengths[4] = 12;
        valid_tag_lengths[5] = 14;
        valid_tag_lengths[6] = 16;
        break;
    case PSA_ALG_GCM:
        valid_tag_lengths[0] = 4;
        valid_tag_lengths[1] = 8;
        valid_tag_lengths[2] = 12;
        valid_tag_lengths[3] = 13;
        valid_tag_lengths[4] = 14;
        valid_tag_lengths[5] = 15;
        valid_tag_lengths[6] = 16;
        break;
    case PSA_ALG_CHACHA20_POLY1305:
        if (tag_length != 16) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
    default:
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Cycle through all valid tag lengths for CCM or GCM */
    uint32_t i;
    for (i=0; i<7; i++) {
        if (tag_length == valid_tag_lengths[i]) {
            break;
        }
    }

    if (i == 7) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return PSA_SUCCESS;
}

psa_status_t
cc3xx_aead_encrypt(const psa_key_attributes_t *attributes,
                   const uint8_t *key_buffer, size_t key_buffer_size,
                   psa_algorithm_t alg, const uint8_t *nonce,
                   size_t nonce_length, const uint8_t *additional_data,
                   size_t additional_data_length, const uint8_t *plaintext,
                   size_t plaintext_length, uint8_t *ciphertext,
                   size_t ciphertext_size, size_t *ciphertext_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_algorithm_t ref_alg;

    /* Validate the algorithm first */
    status = check_alg(alg, &ref_alg);
    if (status != PSA_SUCCESS) {
        return status;
    }

    switch (ref_alg) {
    case PSA_ALG_CCM:
        status = cc3xx_encrypt_ccm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, plaintext,
            plaintext_length, ciphertext, ciphertext_size, ciphertext_length);
        break;

    case PSA_ALG_GCM:
        status = cc3xx_encrypt_gcm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, plaintext,
            plaintext_length, ciphertext, ciphertext_size, ciphertext_length);
        break;

    case PSA_ALG_CHACHA20_POLY1305:
        status = cc3xx_encrypt_chacha20_poly1305(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, plaintext,
            plaintext_length, ciphertext, ciphertext_size, ciphertext_length);
        break;

    default:
        status = PSA_ERROR_NOT_SUPPORTED;
        break;
    }

    return status;
}

psa_status_t cc3xx_aead_decrypt(const psa_key_attributes_t *attributes,
                                const uint8_t *key_buffer,
                                size_t key_buffer_size, psa_algorithm_t alg,
                                const uint8_t *nonce, size_t nonce_length,
                                const uint8_t *additional_data,
                                size_t additional_data_length,
                                const uint8_t *ciphertext,
                                size_t ciphertext_length, uint8_t *plaintext,
                                size_t plaintext_size, size_t *plaintext_length)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_algorithm_t ref_alg;

    /* Validate the algorithm first */
    status = check_alg(alg, &ref_alg);
    if (status != PSA_SUCCESS) {
        return status;
    }

    switch (ref_alg) {
    case PSA_ALG_CCM:
        status = cc3xx_decrypt_ccm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, ciphertext,
            ciphertext_length, plaintext, plaintext_size, plaintext_length);
        break;

    case PSA_ALG_GCM:
        status = cc3xx_decrypt_gcm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, ciphertext,
            ciphertext_length, plaintext, plaintext_size, plaintext_length);
        break;

    case PSA_ALG_CHACHA20_POLY1305:
        status = cc3xx_decrypt_chacha20_poly1305(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, ciphertext,
            ciphertext_length, plaintext, plaintext_size, plaintext_length);
        break;

    default:
        status = PSA_ERROR_NOT_SUPPORTED;
        break;
    }

    return status;
}
