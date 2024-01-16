/*
 * Copyright (c) 2021-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_PSA_ENTROPY_H__
#define __CC3XX_PSA_ENTROPY_H__

/** @file cc3xx_psa_entropy.h
 *
 * This file contains the declaration of the entry points associated
 * to the entropy capability as described by the PSA Cryptoprocessor
 * Driver interface specification
 *
 */

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Extract entropy from the CC3XX entropy source (TRNG)
 *
 * @param[in]  flags         Flags to be passed to the TRNG source
 * @param[out] estimate_bits Estimation of the obtained entropy in bits
 * @param[out] output        Collected entropy from the TRNG source
 * @param[in]  output_size   Size of the output buffer
 *
 * @retval  PSA_SUCCESS on success. Error code from \ref psa_status_t on
 *          failure
 */
psa_status_t cc3xx_get_entropy(uint32_t flags, size_t *estimate_bits,
                               uint8_t *output, size_t output_size);
#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_PSA_ENTROPY_H__ */
