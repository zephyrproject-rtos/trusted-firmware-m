/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc_common.h"
#include "cc_pal_abort.h"
#include "cc_pal_mem.h"
#include "cc_pal_types.h"
#include "chacha20_alt.h"
#include "chacha_driver.h"
#include "poly.h"
#include <psa/crypto.h>

#include "cc3xx_internal_chacha20.h"
#include "cc3xx_internal_chacha20_poly1305.h"

static psa_status_t chacha20_poly1305_crypt(const unsigned char key[32],
                                            const unsigned char nonce[12],
                                            uint32_t counter, size_t data_len,
                                            const unsigned char *input,
                                            unsigned char *output)
{
    ChachaContext_t context = {0};
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    cc3xx_chacha20_init(&context);

    status = cc3xx_chacha20_setkey(&context, key);
    if (status != PSA_SUCCESS)
        goto cleanup;

    status = cc3xx_chacha20_starts(&context, nonce, counter);
    if (status != PSA_SUCCESS)
        goto cleanup;

    status = cc3xx_chacha20_update(&context, data_len, input, output);

cleanup:
    cc3xx_chacha20_free(&context);
    return status;
}

static psa_status_t chacha20_poly1305_crypt_and_tag(
    cryptoDirection_t direction, const uint8_t *key_buffer,
    size_t key_buffer_size, size_t length, const unsigned char nonce[12],
    const unsigned char *aad, size_t aad_len, const unsigned char *input,
    unsigned char *output, size_t *output_length, unsigned char tag[16])
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;

    int rc = -1;
    uint8_t chachaInState[CHACHA_BLOCK_SIZE_BYTES] = {0};
    uint8_t chachaOutState[CHACHA_BLOCK_SIZE_BYTES] = {0};
    mbedtls_poly_key polyKey = {0};
    mbedtls_poly_mac polyMac = {0};
    const uint8_t *pCipherData = NULL;

    /* TODO: Do we want this on the stack? */
    ChachaContext_t context = {0};

    /* TODO: Must verify that this is true, but cryptocell code seems to expect
     * this */
    if (key_buffer_size != 32) {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto cleanup;
    }
    cc3xx_chacha20_setkey(&context, key_buffer);

    if (direction == CRYPTO_DIRECTION_ENCRYPT) {
        pCipherData = output;
    } else if (direction == CRYPTO_DIRECTION_DECRYPT) {
        pCipherData = input;
    } else {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto cleanup;
    }

    // 1. Generate poly key
    rc = chacha20_poly1305_crypt((uint8_t *)context.keyBuf, nonce, 0,
                                 sizeof(chachaInState), chachaInState,
                                 chachaOutState);

    if (rc != 0) {
        status = PSA_ERROR_DATA_INVALID;
        goto cleanup;
    }
    // poly key defined as the first 32 bytes of chacha output.
    CC_PalMemCopy(polyKey, chachaOutState, sizeof(polyKey));

    // 2. Encryption pDataIn
    if (direction == CRYPTO_DIRECTION_ENCRYPT) {
        rc = chacha20_poly1305_crypt((uint8_t *)context.keyBuf, nonce, 1,
                                     length, input, output);

        if (rc != 0) {
            status = PSA_ERROR_DATA_INVALID;
            goto cleanup;
        }
    }

    // 3. Authentication
    rc = (int)PolyMacCalc(polyKey, aad, aad_len, pCipherData, length, polyMac,
                          true);
    if (rc != 0) {
        status = PSA_ERROR_DATA_INVALID;
        goto cleanup;
    }

    CC_PalMemCopy(tag, polyMac, sizeof(polyMac));

    if (direction == CRYPTO_DIRECTION_DECRYPT) {
        rc = chacha20_poly1305_crypt((uint8_t *)context.keyBuf, nonce, 1,
                                     length, input, output);

        if (rc != 0) {
            status = PSA_ERROR_DATA_INVALID;
            goto cleanup;
        }
    }
cleanup:
    cc3xx_chacha20_free(&context);

    if (rc == 0) {
        status = PSA_SUCCESS;

        if (direction == CRYPTO_DIRECTION_ENCRYPT) {
            /*
             * Since tag must be 16, we hardcode it.
             * The tag is appended to the ciphertext and the length is expected
             * to be the combined effort.
             */
            *output_length = length + 16;
        } else {
            /* The plaintext is only the length of the decrypted ciphertext, no
             * tag included. */
            *output_length = length;
        }
    } else if (output != NULL) {
        /*
         * If the output is not NULL and the rc was not 0, something bad
         * happened. We clear the output, and the status should have been set to
         * non-success.
         */
        CC_PalMemSetZero(output, length);
    }

    return status;
}

/* TODO: Figure out best strategy for cc310 vs cc312 */
psa_status_t cc3xx_encrypt_chacha20_poly1305(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *plaintext,
    size_t plaintext_length, uint8_t *ciphertext, size_t ciphertext_size,
    size_t *ciphertext_length)
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;

    /*
     * The tag should be directly behind the ciphertext,
     * and the ciphertext is as long as the plaintext input.
     */
    uint8_t *tag = ciphertext + plaintext_length;

    if (nonce_length != CHACHA_IV_96_SIZE_BYTES){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = chacha20_poly1305_crypt_and_tag(
        CRYPTO_DIRECTION_ENCRYPT, key_buffer, key_buffer_size, plaintext_length,
        nonce, additional_data, additional_data_length, plaintext, ciphertext,
        ciphertext_length, tag);
    return status;
}

psa_status_t cc3xx_decrypt_chacha20_poly1305(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *ciphertext,
    size_t ciphertext_length, uint8_t *plaintext, size_t plaintext_size,
    size_t *plaintext_length)
{
    int diff, i;
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;

    uint8_t local_tag_buffer[PSA_AEAD_TAG_MAX_SIZE];

    psa_key_type_t key_type = psa_get_key_type(attributes);
    psa_key_bits_t key_bits = psa_get_key_bits(attributes);
    psa_algorithm_t key_alg = psa_get_key_algorithm(attributes);

    size_t tag_length = PSA_AEAD_TAG_LENGTH(key_type, key_bits, key_alg);

    size_t ciphertext_length_without_tag = ciphertext_length - tag_length;
    const uint8_t *tag = ciphertext + ciphertext_length_without_tag;

    CC_PalMemCopy(local_tag_buffer, tag, tag_length);

    if (nonce_length != CHACHA_IV_96_SIZE_BYTES){
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    status = chacha20_poly1305_crypt_and_tag(
        CRYPTO_DIRECTION_DECRYPT, key_buffer, key_buffer_size,
        ciphertext_length_without_tag, nonce, additional_data,
        additional_data_length, ciphertext, plaintext, plaintext_length,
        local_tag_buffer);

    /* Check tag in "constant-time" */
    for (diff = 0, i = 0; i < sizeof(tag_length); i++)
        diff |= tag[i] ^ local_tag_buffer[i];

    if (diff != 0) {
        /* TODO: Do we really want to zeroize the entire userbuffer, not the
         * affected parts? */
        CC_PalMemSetZero(plaintext, plaintext_size);
        return (PSA_ERROR_INVALID_SIGNATURE);
    }

    return status;
}
