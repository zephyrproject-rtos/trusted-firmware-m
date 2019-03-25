/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <limits.h>

#include "tfm_crypto_defs.h"

#include "crypto_engine.h"

#include "psa_crypto.h"

#include "tfm_crypto_api.h"

/**
 * \def CRYPTO_AEAD_MAX_KEY_LENGTH
 *
 * \brief Specifies the maximum key length supported by the
 *        AEAD operations in this implementation
 */
#ifndef CRYPTO_AEAD_MAX_KEY_LENGTH
#define CRYPTO_AEAD_MAX_KEY_LENGTH (32)
#endif

static psa_status_t _psa_get_key_information(psa_key_slot_t key,
                                             psa_key_type_t *type,
                                             size_t *bits)
{
    psa_invec in_vec[] = {
        {.base = &key, .len = sizeof(psa_key_slot_t)},
    };
    psa_outvec out_vec[] = {
        {.base = type, .len = sizeof(psa_key_type_t)},
        {.base = bits, .len = sizeof(size_t)}
    };

    return tfm_crypto_get_key_information(
                 in_vec, sizeof(in_vec)/sizeof(in_vec[0]),
                 out_vec, sizeof(out_vec)/sizeof(out_vec[0]));
}

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
psa_status_t tfm_crypto_aead_encrypt(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    uint8_t key_data[CRYPTO_AEAD_MAX_KEY_LENGTH];
    uint32_t key_size;
    psa_key_type_t key_type;

    if ((in_len != 3) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len < (sizeof(psa_key_slot_t) + sizeof(psa_algorithm_t))) ||
        (in_vec[0].len > (TFM_CRYPTO_MAX_NONCE_LENGTH
                       + (sizeof(psa_key_slot_t) + sizeof(psa_algorithm_t))))) {
        return PSA_CONNECTION_REFUSED;
    }

    const struct tfm_crypto_aead_pack_input *input_s = in_vec[0].base;
    psa_key_slot_t key = input_s->key;
    psa_algorithm_t alg = input_s->alg;
    const uint8_t *nonce = input_s->nonce;
    size_t nonce_length = in_vec[0].len - sizeof(psa_key_slot_t)
                                        - sizeof(psa_algorithm_t);
    const uint8_t *additional_data = in_vec[1].base;
    size_t additional_data_length = in_vec[1].len;
    const uint8_t *plaintext = in_vec[2].base;
    size_t plaintext_length = in_vec[2].len;
    uint8_t *ciphertext = out_vec[0].base;
    size_t ciphertext_size = out_vec[0].len;

    /* Initialise ciphertext_length to zero */
    out_vec[0].len = 0;

    if (PSA_ALG_IS_AEAD(alg) == 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if (PSA_AEAD_TAG_SIZE(alg) == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_AEAD_ENCRYPT_OUTPUT_SIZE(alg, plaintext_length) > ciphertext_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if ((nonce_length == 0) ||
        ((additional_data_length == 0) && (additional_data != NULL))) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Access the key data */
    status = _psa_get_key_information(key, &key_type, (size_t *)&key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Support only AES based AEAD */
    if (key_type != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Access the crypto service key module to retrieve key data */
    status = tfm_crypto_get_key(key,
                                PSA_KEY_USAGE_ENCRYPT,
                                alg,
                                key_data,
                                CRYPTO_AEAD_MAX_KEY_LENGTH,
                                (size_t *)&key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Request AEAD encryption on the crypto engine */
    status = tfm_crypto_engine_aead_encrypt(key_type,
                                            alg,
                                            key_data,
                                            key_size,
                                            nonce,
                                            nonce_length,
                                            additional_data,
                                            additional_data_length,
                                            plaintext,
                                            plaintext_length,
                                            ciphertext,
                                            ciphertext_size,
                                            (uint32_t *)&(out_vec[0].len));
    if (status == PSA_SUCCESS) {
        /* The ciphertext_length needs to take into account the tag length */
        out_vec[0].len += PSA_AEAD_TAG_SIZE(alg);
    } else {
        /* In case of failure set the ciphertext_length to zero */
        out_vec[0].len = 0;
    }

    return status;
}

psa_status_t tfm_crypto_aead_decrypt(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
    psa_status_t status = PSA_SUCCESS;
    uint8_t key_data[CRYPTO_AEAD_MAX_KEY_LENGTH];
    uint32_t key_size;
    psa_key_type_t key_type;

    if ((in_vec[0].len < (sizeof(psa_key_slot_t) + sizeof(psa_algorithm_t))) ||
        (in_vec[0].len > (TFM_CRYPTO_MAX_NONCE_LENGTH
                       + (sizeof(psa_key_slot_t) + sizeof(psa_algorithm_t))))) {
        return PSA_CONNECTION_REFUSED;
    }

    const struct tfm_crypto_aead_pack_input *input_s = in_vec[0].base;
    psa_key_slot_t key = input_s->key;
    psa_algorithm_t alg = input_s->alg;
    const uint8_t *nonce = input_s->nonce;
    size_t nonce_length = in_vec[0].len - sizeof(psa_key_slot_t)
                                        - sizeof(psa_algorithm_t);
    const uint8_t *additional_data = in_vec[1].base;
    size_t additional_data_length = in_vec[1].len;
    const uint8_t *ciphertext = in_vec[2].base;
    size_t ciphertext_length = in_vec[2].len;
    uint8_t *plaintext = out_vec[0].base;
    size_t plaintext_size = out_vec[0].len;

    /* Initialise plaintext_length to zero */
    out_vec[0].len = 0;

    if (PSA_ALG_IS_AEAD(alg) == 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    if ((PSA_AEAD_TAG_SIZE(alg) == 0) ||
        (ciphertext_length < PSA_AEAD_TAG_SIZE(alg))) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_AEAD_DECRYPT_OUTPUT_SIZE(alg,ciphertext_length) > plaintext_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    if ((nonce_length == 0) ||
        ((additional_data_length == 0) && (additional_data != NULL))) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Access the key data */
    status = _psa_get_key_information(key, &key_type, (size_t *)&key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Support only AES based AEAD */
    if (key_type != PSA_KEY_TYPE_AES) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    /* Access the crypto service key module to retrieve key data */
    status = tfm_crypto_get_key(key,
                                PSA_KEY_USAGE_DECRYPT,
                                alg,
                                key_data,
                                CRYPTO_AEAD_MAX_KEY_LENGTH,
                                (size_t *)&key_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Request AEAD decryption on the crypto engine */
    status = tfm_crypto_engine_aead_decrypt(key_type,
                                            alg,
                                            key_data,
                                            key_size,
                                            nonce,
                                            nonce_length,
                                            additional_data,
                                            additional_data_length,
                                            ciphertext,
                                            ciphertext_length,
                                            plaintext,
                                            plaintext_size,
                                            (uint32_t *)&(out_vec[0].len));
    if (status != PSA_SUCCESS) {
        out_vec[0].len = 0;
    }

    return status;
}
/*!@}*/
