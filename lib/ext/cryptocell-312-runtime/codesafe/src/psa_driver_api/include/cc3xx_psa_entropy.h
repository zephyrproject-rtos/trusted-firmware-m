/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_PSA_ENTROPY_H
#define CC3XX_PSA_ENTROPY_H

#include "psa/crypto.h"

psa_status_t cc3xx_get_entropy(uint32_t flags, size_t *estimate_bits,
                               uint8_t *output, size_t output_size);

#endif /* CC3XX_PSA_ENTROPY_H */
