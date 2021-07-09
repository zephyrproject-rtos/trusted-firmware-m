/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HASH_COMMON_H
#define HASH_COMMON_H

#include "cc3xx_crypto_primitives.h"
#include "psa/crypto.h"

#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

psa_status_t cc3xx_hash_setup(cc3xx_hash_operation_t *operation,
                              psa_algorithm_t alg);

psa_status_t cc3xx_hash_clone(const cc3xx_hash_operation_t *source_operation,
                              cc3xx_hash_operation_t *target_operation);

psa_status_t cc3xx_hash_update(cc3xx_hash_operation_t *operation,
                               const uint8_t *input, size_t input_length);

psa_status_t cc3xx_hash_finish(cc3xx_hash_operation_t *operation, uint8_t *hash,
                               size_t hash_size, size_t *hash_length);

psa_status_t cc3xx_hash_abort(cc3xx_hash_operation_t *operation);

psa_status_t cc3xx_hash_compute(psa_algorithm_t alg, const uint8_t *input,
                                size_t input_length, uint8_t *hash,
                                size_t hash_size, size_t *hash_length);
#endif /*HASH_COMMON_H */
