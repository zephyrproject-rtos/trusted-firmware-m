/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_PSA_CIPHER_H
#define CC3XX_PSA_CIPHER_H

#include "psa/crypto.h"

#include "cc3xx_crypto_primitives.h"
#include "cc3xx_internal_aes.h"
#include "cc3xx_internal_chacha20.h"

psa_status_t cc3xx_cipher_encrypt_setup(
        cc3xx_cipher_operation_t *operation,
        const psa_key_attributes_t *attributes,
        const uint8_t *key, size_t key_length,
        psa_algorithm_t alg);

psa_status_t cc3xx_cipher_decrypt_setup(
        cc3xx_cipher_operation_t *operation,
        const psa_key_attributes_t *attributes,
        const uint8_t *key, size_t key_length,
        psa_algorithm_t alg);

psa_status_t cc3xx_cipher_set_iv(
        cc3xx_cipher_operation_t *operation,
        const uint8_t *iv, size_t iv_length);

psa_status_t cc3xx_cipher_update(
        cc3xx_cipher_operation_t *operation,
        const uint8_t *input, size_t input_length,
        uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t cc3xx_cipher_finish(
        cc3xx_cipher_operation_t *operation,
        uint8_t *output, size_t output_size, size_t *output_length);

psa_status_t cc3xx_cipher_abort(
        cc3xx_cipher_operation_t *operation);

psa_status_t cc3xx_psa_cipher_encrypt(
        const psa_key_attributes_t *attributes,
        const uint8_t *key_buffer,
        size_t key_buffer_size,
        psa_algorithm_t alg,
        const uint8_t *input,
        size_t input_length,
        uint8_t *output,
        size_t output_size,
        size_t *output_length);

psa_status_t cc3xx_psa_cipher_decrypt(
        const psa_key_attributes_t *attributes,
        const uint8_t *key_buffer,
        size_t key_buffer_size,
        psa_algorithm_t alg,
        const uint8_t *input,
        size_t input_length,
        uint8_t *output,
        size_t output_size,
        size_t *output_length);

#endif /* CC3XX_PSA_CIPHER_H */
