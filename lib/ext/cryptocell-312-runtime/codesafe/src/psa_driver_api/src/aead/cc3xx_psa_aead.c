/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "aes_driver.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include <psa/crypto.h>

#include "cc3xx_psa_aead.h"
#include "internal/cc3xx_psa_aead_ccm.h"
#include "internal/cc3xx_psa_aead_chacha20_poly1305.h"
#include "internal/cc3xx_psa_aead_gcm.h"

/*
 * NOTE: The cc3xx chachapoly impl assumes 256-bit keys.
 * Figure out if this is a limitation- if so we must
 * remember to restrict this in the relevant .json file.
 */

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

    /* TODO: What's the correct shortcut for "any CCM"/"any GCM" (for valid tag
     * sizes)? */
    switch (alg) {
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 6):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 10):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 12):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 14):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 16):
        status = cc3xx_psa_aead_encrypt_ccm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, plaintext,
            plaintext_length, ciphertext, ciphertext_size, ciphertext_length);
        break;

    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 4):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 8):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 12):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 13):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 14):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 15):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 16):
        status = cc3xx_psa_aead_encrypt_gcm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, plaintext,
            plaintext_length, ciphertext, ciphertext_size, ciphertext_length);
        break;

    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 16):
        status = cc3xx_psa_aead_encrypt_chacha20_poly1305(
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

    switch (alg) {
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 4):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 6):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 8):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 10):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 12):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 14):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CCM, 16):
        status = cc3xx_psa_aead_decrypt_ccm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, ciphertext,
            ciphertext_length, plaintext, plaintext_size, plaintext_length);
        break;

    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 4):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 8):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 12):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 13):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 14):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 15):
    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 16):
        status = cc3xx_psa_aead_decrypt_gcm(
            attributes, key_buffer, key_buffer_size, alg, nonce, nonce_length,
            additional_data, additional_data_length, ciphertext,
            ciphertext_length, plaintext, plaintext_size, plaintext_length);
        break;

    case PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_CHACHA20_POLY1305, 16):
        status = cc3xx_psa_aead_decrypt_chacha20_poly1305(
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
