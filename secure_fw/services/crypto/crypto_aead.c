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
#include "crypto_utils.h"

/*!
 * \defgroup public_psa Public functions, PSA
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_aead_encrypt(psa_key_slot_t key,
                                              psa_algorithm_t alg,
                                              const uint8_t *nonce,
                                              size_t nonce_length,
                                              const uint8_t *additional_data,
                                              size_t additional_data_length,
                                              const uint8_t *plaintext,
                                              size_t plaintext_length,
                                              uint8_t *ciphertext,
                                              size_t ciphertext_size,
                                              size_t *ciphertext_length)
{
    psa_status_t status = PSA_SUCCESS;
    enum tfm_crypto_err_t err;
    uint8_t key_data[TFM_CRYPTO_MAX_KEY_LENGTH];
    uint32_t key_size;
    psa_key_type_t key_type;

    if (PSA_ALG_IS_AEAD(alg) == 0) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_AEAD_TAG_SIZE(alg) == 0) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_AEAD_ENCRYPT_OUTPUT_SIZE(alg, plaintext_length) > ciphertext_size) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((nonce_length == 0) ||
        ((additional_data_length == 0) && (additional_data != NULL))) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate pointers */
    err = tfm_crypto_memory_check((void *)nonce,
                                  nonce_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((additional_data != NULL) || (additional_data_length != 0)) {
        err = tfm_crypto_memory_check((void *)additional_data,
                                      additional_data_length,
                                      TFM_MEMORY_ACCESS_RO);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    err = tfm_crypto_memory_check((void *)plaintext,
                                  plaintext_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((void *)ciphertext,
                                  ciphertext_size,
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((void *)ciphertext_length,
                                  sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Access the key data */
    err = tfm_crypto_get_key_information(key, &key_type, (size_t *)&key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Access the crypto service key module to retrieve key data */
    err = tfm_crypto_export_key(key,
                                &key_data[0],
                                TFM_CRYPTO_MAX_KEY_LENGTH,
                                (size_t *)&key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
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
                                            (uint32_t *)ciphertext_length);

    return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
}

enum tfm_crypto_err_t tfm_crypto_aead_decrypt(psa_key_slot_t key,
                                              psa_algorithm_t alg,
                                              const uint8_t *nonce,
                                              size_t nonce_length,
                                              const uint8_t *additional_data,
                                              size_t additional_data_length,
                                              const uint8_t *ciphertext,
                                              size_t ciphertext_length,
                                              uint8_t *plaintext,
                                              size_t plaintext_size,
                                              size_t *plaintext_length)
{
    psa_status_t status = PSA_SUCCESS;
    enum tfm_crypto_err_t err;
    uint8_t key_data[TFM_CRYPTO_MAX_KEY_LENGTH];
    uint32_t key_size;
    psa_key_type_t key_type;

    if (PSA_ALG_IS_AEAD(alg) == 0) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_AEAD_TAG_SIZE(alg) == 0) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if (PSA_AEAD_DECRYPT_OUTPUT_SIZE(alg, ciphertext_length) > plaintext_size) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((nonce_length == 0) ||
        ((additional_data_length == 0) && (additional_data != NULL))) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Validate pointers */
    err = tfm_crypto_memory_check((void *)nonce,
                                  nonce_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    if ((additional_data != NULL) || (additional_data_length != 0)) {
        err = tfm_crypto_memory_check((void *)additional_data,
                                      additional_data_length,
                                      TFM_MEMORY_ACCESS_RO);
        if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
            return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
        }
    }

    err = tfm_crypto_memory_check((void *)ciphertext,
                                  ciphertext_length,
                                  TFM_MEMORY_ACCESS_RO);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((void *)plaintext,
                                  plaintext_size,
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    err = tfm_crypto_memory_check((void *)plaintext_length,
                                  sizeof(size_t),
                                  TFM_MEMORY_ACCESS_RW);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Access the key data */
    err = tfm_crypto_get_key_information(key, &key_type, (size_t *)&key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Access the crypto service key module to retrieve key data */
    err = tfm_crypto_export_key(key,
                                &key_data[0],
                                TFM_CRYPTO_MAX_KEY_LENGTH,
                                (size_t *)&key_size);
    if (err != TFM_CRYPTO_ERR_PSA_SUCCESS) {
        return err;
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
                                            (uint32_t *)plaintext_length);

    return PSA_STATUS_TO_TFM_CRYPTO_ERR(status);
}
/*!@}*/
