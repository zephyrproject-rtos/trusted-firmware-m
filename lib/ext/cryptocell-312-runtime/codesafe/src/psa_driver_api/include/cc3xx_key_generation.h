/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_KEY_GENERATION_H
#define CC3XX_KEY_GENERATION_H

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

psa_status_t cc3xx_generate_key(const psa_key_attributes_t *attributes,
                                uint8_t *key_buffer, size_t key_buffer_size,
                                size_t *key_buffer_length);

psa_status_t cc3xx_export_public_key(const psa_key_attributes_t *attributes,
                                     const uint8_t *key_buffer,
                                     size_t key_buffer_size, uint8_t *data,
                                     size_t data_size, size_t *data_length);
#ifdef __cplusplus
}
#endif
#endif /* CC3XX_KEY_GENERATION_H */
