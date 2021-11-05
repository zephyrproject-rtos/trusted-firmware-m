/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** \file cc3xx_internal_ccm.h
 *
 * This file contains the declarations of the internal functions to
 * perform AEAD using the AES-CCM algorithm
 *
 */

#ifndef CC3XX_INTERNAL_CCM_H
#define CC3XX_INTERNAL_CCM_H

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Encrypt and create auth tag with AES-CCM
 */
psa_status_t cc3xx_encrypt_ccm(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *plaintext,
    size_t plaintext_length, uint8_t *ciphertext, size_t ciphertext_size,
    size_t *ciphertext_length);

/**
 * \brief Decrypt and validate auth tag with AES-CCM
 */
psa_status_t cc3xx_decrypt_ccm(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *ciphertext,
    size_t ciphertext_length, uint8_t *plaintext, size_t plaintext_size,
    size_t *plaintext_length);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_INTERNAL_CCM_H */
