/*
 * Copyright (c) 2021-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_PSA_KEY_GENERATION_H__
#define __CC3XX_PSA_KEY_GENERATION_H__

/** @file cc3xx_psa_key_generation.h
 *
 * This file contains the declaration of the entry points associated to the
 * key generation (i.e. random generation and extraction of public keys) as
 * described by the PSA Cryptoprocessor Driver interface specification
 *
 * @note FixMe: Full working implementation still need to be provided, these
 *       functions are currently just the prototypes
 */

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Generate a random key
 *
 * @param[in]  attributes        Attributes of the key to use
 * @param[out] key_buffer        Buffer to hold the generated key
 * @param[in]  key_buffer_size   Size in bytes of the key_buffer buffer
 * @param[out] key_buffer_length Size in bytes of the generated key
 *
 * @retval  PSA_SUCCESS on success. Error code from \ref psa_status_t on
 *          failure
 */
psa_status_t cc3xx_generate_key(const psa_key_attributes_t *attributes,
                                uint8_t *key_buffer, size_t key_buffer_size,
                                size_t *key_buffer_length);
/*!
 * @brief Export the public key from a private key.
 *
 * @param[in]  attributes      Attributes of the key to use
 * @param[in]  key_buffer      Buffer to hold the generated key
 * @param[in]  key_buffer_size Size in bytes of the key_buffer buffer
 * @param[out] data            Buffer to hold the extracted public key
 * @param[in]  data_size       Size in bytes of the data buffer
 * @param[out] data_length     Size in bytes of the extracted public key
 *
 * @retval  PSA_SUCCESS on success. Error code from \ref psa_status_t on
 *          failure
 */
psa_status_t cc3xx_export_public_key(const psa_key_attributes_t *attributes,
                                     const uint8_t *key_buffer,
                                     size_t key_buffer_size, uint8_t *data,
                                     size_t data_size, size_t *data_length);
#ifdef __cplusplus
}
#endif
#endif /* __CC3XX_PSA_KEY_GENERATION_H__ */
