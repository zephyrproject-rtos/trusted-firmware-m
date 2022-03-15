/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** \file cc3xx_internal_chacha20_poly1305.h
 *
 * This file contains the declarations of the internal functions to
 * perform AEAD using the Chacha20-Poly1305 algorithm
 *
 */

#ifndef CC3XX_INTERNAL_CHACHA20_POLY1305_H
#define CC3XX_INTERNAL_CHACHA20_POLY1305_H

#include "psa/crypto.h"
#include "chacha_driver.h"
#include "poly.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief A context type to implement multipart APIs on the Chacha20-Poly1305
 *        algorithm. The low level driver does not implement it so we let the
 *        driver interface implement it by separately including Chacha20 context
 *        and Poly1305 state defined in the low level driver and combining them.
 *        As this is an internal detail of the driver implementation, follow
 *        the same guidelines of the other contexts in terms of naming.
 */
typedef struct ChachaPolyContext_t {
    ChachaContext_t chacha;        /*!< Context of the underlying Chacha20 */
    PolyState_t poly;              /*!< Context of the underlying Poly1305 */
    size_t ad_len;             /*!< Length of the data to be authenticated */
    size_t plaintext_len;          /*!< Length of the data to be encrypted */
    size_t curr_ad_len;         /*!< Size of the data authenticated so far */
    size_t curr_plaintext_len;      /*!< Size of the data encrypted so far */
    bool bAuthenticateInput; /*!< True when input is used for AEAD authent */
} ChachaPolyContext_t;

/**
 * \brief Encrypt and create auth tag with Chacha20-Poly1305
 */
psa_status_t cc3xx_encrypt_chacha20_poly1305(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *plaintext,
    size_t plaintext_length, uint8_t *ciphertext, size_t ciphertext_size,
    size_t *ciphertext_length);

/**
 * \brief Decrypt and validate auth tag with Chacha20-Poly1305
 */
psa_status_t cc3xx_decrypt_chacha20_poly1305(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *nonce,
    size_t nonce_length, const uint8_t *additional_data,
    size_t additional_data_length, const uint8_t *ciphertext,
    size_t ciphertext_length, uint8_t *plaintext, size_t plaintext_size,
    size_t *plaintext_length);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_INTERNAL_CHACHA20_POLY1305_H */
