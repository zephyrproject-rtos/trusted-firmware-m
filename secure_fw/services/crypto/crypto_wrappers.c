/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "tfm_crypto_defs.h"

#include "psa_crypto.h"

#include "crypto_psa_wrappers.h"

#include "tfm_crypto_api.h"

/*!
 * \defgroup public Public functions, TF-M compatible wrappers
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_cipher_update_wrapper(
                                              psa_cipher_operation_t *operation,
                                        struct psa_cipher_update_input *input_s,
                                      struct psa_cipher_update_output *output_s)
{
    /* Extract the following fields from the input and output structures */
    const uint8_t *input = input_s->input;
    size_t input_length = input_s->input_length;

    unsigned char *output = output_s->output;
    size_t output_size = output_s->output_size;
    size_t *output_length = output_s->output_length;

    return tfm_crypto_cipher_update(operation, input, input_length,
                                    output, output_size, output_length);
}

enum tfm_crypto_err_t tfm_crypto_aead_encrypt_wrapper(
                                        struct psa_aead_encrypt_input *input_s,
                                      struct psa_aead_encrypt_output *output_s)
{
    /* Extract the following fields from the input and output structures */
    psa_key_slot_t key = input_s->key;
    psa_algorithm_t alg = input_s->alg;
    const uint8_t *nonce = input_s->nonce;
    size_t nonce_length = input_s->nonce_length;
    const uint8_t *additional_data = input_s->additional_data;
    size_t additional_data_length = input_s->additional_data_length;
    const uint8_t *plaintext = input_s->plaintext;
    size_t plaintext_length = input_s->plaintext_length;

    uint8_t *ciphertext = output_s->ciphertext;
    size_t ciphertext_size = output_s->ciphertext_size;
    size_t *ciphertext_length = output_s->ciphertext_length;

    return tfm_crypto_aead_encrypt(key, alg, nonce, nonce_length,
                                   additional_data, additional_data_length,
                                   plaintext, plaintext_length, ciphertext,
                                   ciphertext_size, ciphertext_length);
}

enum tfm_crypto_err_t tfm_crypto_aead_decrypt_wrapper(
                                        struct psa_aead_decrypt_input *input_s,
                                      struct psa_aead_decrypt_output *output_s)
{
    /* Extract the following fields from the input and output structures */
    psa_key_slot_t key = input_s->key;
    psa_algorithm_t alg = input_s->alg;
    const uint8_t *nonce = input_s->nonce;
    size_t nonce_length = input_s->nonce_length;
    const uint8_t *additional_data = input_s->additional_data;
    size_t additional_data_length = input_s->additional_data_length;
    const uint8_t *ciphertext = input_s->ciphertext;
    size_t ciphertext_length = input_s->ciphertext_length;

    uint8_t *plaintext = output_s->plaintext;
    size_t plaintext_size = output_s->plaintext_size;
    size_t *plaintext_length = output_s->plaintext_length;

    return tfm_crypto_aead_decrypt(key, alg, nonce, nonce_length,
                                   additional_data, additional_data_length,
                                   ciphertext, ciphertext_length, plaintext,
                                   plaintext_size, plaintext_length);
}
/*!@}*/
