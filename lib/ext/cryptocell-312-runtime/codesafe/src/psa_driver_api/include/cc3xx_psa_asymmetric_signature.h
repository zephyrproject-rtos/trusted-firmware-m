/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_PSA_ASYMMETRIC_SIGNATURE_H
#define CC3XX_PSA_ASYMMETRIC_SIGNATURE_H

#include "psa/crypto.h"

psa_status_t cc3xx_sign_message(const psa_key_attributes_t *attributes,
                                const uint8_t *key, size_t key_length,
                                psa_algorithm_t alg, const uint8_t *input,
                                size_t input_length, uint8_t *signature,
                                size_t signature_size,
                                size_t *signature_length);

psa_status_t cc3xx_verify_message(const psa_key_attributes_t *attributes,
                                  const uint8_t *key, size_t key_length,
                                  psa_algorithm_t alg, const uint8_t *input,
                                  size_t input_length, const uint8_t *signature,
                                  size_t signature_length);

psa_status_t cc3xx_sign_hash(const psa_key_attributes_t *attributes,
                             const uint8_t *key, size_t key_length,
                             psa_algorithm_t alg, const uint8_t *input,
                             size_t input_length, uint8_t *signature,
                             size_t signature_size, size_t *signature_length);

psa_status_t cc3xx_verify_hash(const psa_key_attributes_t *attributes,
                               const uint8_t *key, size_t key_length,
                               psa_algorithm_t alg, const uint8_t *hash,
                               size_t hash_length, const uint8_t *signature,
                               size_t signature_length);

#endif /* CC3XX_PSA_ASYMMETRIC_SIGNATURE_H */