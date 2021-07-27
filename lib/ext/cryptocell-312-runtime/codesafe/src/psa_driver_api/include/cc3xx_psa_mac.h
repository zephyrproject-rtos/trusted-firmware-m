/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_PSA_MAC_H
#define CC3XX_PSA_MAC_H

#include "cc3xx_crypto_primitives.h"
#include "psa/crypto.h"

psa_status_t cc3xx_mac_sign_setup(cc3xx_mac_operation_t *operation,
                                  const psa_key_attributes_t *attributes,
                                  const uint8_t *key_buffer,
                                  size_t key_buffer_size, psa_algorithm_t alg);

psa_status_t cc3xx_mac_verify_setup(cc3xx_mac_operation_t *operation,
                                    const psa_key_attributes_t *attributes,
                                    const uint8_t *key_buffer,
                                    size_t key_buffer_size,
                                    psa_algorithm_t alg);

psa_status_t cc3xx_mac_update(cc3xx_mac_operation_t *operation,
                              const uint8_t *input, size_t input_length);

psa_status_t cc3xx_mac_sign_finish(cc3xx_mac_operation_t *operation,
                                   uint8_t *mac, size_t mac_size,
                                   size_t *mac_length);

psa_status_t cc3xx_mac_verify_finish(cc3xx_mac_operation_t *operation,
                                     const uint8_t *mac, size_t mac_length);

psa_status_t cc3xx_mac_abort(cc3xx_mac_operation_t *operation);

psa_status_t cc3xx_mac_compute(const psa_key_attributes_t *attributes,
                               const uint8_t *key_buffer,
                               size_t key_buffer_size, psa_algorithm_t alg,
                               const uint8_t *input, size_t input_length,
                               uint8_t *mac, size_t mac_size,
                               size_t *mac_length);

#endif /* CC3XX_PSA_MAC_H */
