/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_ASYMMETRIC_ENCRYPTION_H
#define CC3XX_ASYMMETRIC_ENCRYPTION_H

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

psa_status_t cc3xx_asymmetric_encrypt(const psa_key_attributes_t *attributes,
                                      const uint8_t *key_buffer,
                                      size_t key_buffer_size,
                                      psa_algorithm_t alg,
                                      const uint8_t *input,
                                      size_t input_length,
                                      const uint8_t *salt, size_t salt_length,
                                      uint8_t *output, size_t output_size,
                                      size_t *output_length);

psa_status_t cc3xx_asymmetric_decrypt(const psa_key_attributes_t *attributes,
                                      const uint8_t *key_buffer,
                                      size_t key_buffer_size,
                                      psa_algorithm_t alg,
                                      const uint8_t *input,
                                      size_t input_length,
                                      const uint8_t *salt, size_t salt_length,
                                      uint8_t *output, size_t output_size,
                                      size_t *output_length);
#ifdef __cplusplus
}
#endif
#endif /* CC3XX_ASYMMETRIC_ENCRYPTION_H */
