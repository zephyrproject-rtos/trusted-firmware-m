/*
 * Copyright (c) 2021-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_RNG_H
#define CC3XX_RNG_H

#include "cc3xx_error.h"

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum cc3xx_rng_quality_t {
    CC3XX_RNG_CRYPTOGRAPHICALLY_SECURE,
    CC3XX_RNG_FAST,
};

/**
 * @brief                        Get random bytes from the CC3XX TRNG.
 *
 * @note                         This function may take a variable amount of
 *                               time to execute. This function may take a
 *                               considerable amount of time to execute if the
 *                               current TRNG entropy pool is depleted and more
 *                               entropy needs generating.
 *
 * @param[out] buf               Buffer to fill with random bytes.
 * @param[in] length             Size of the buffer.
 * @param[in] quality            The TRNG quality used to generate random data.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_rng_get_random(uint8_t* buf, size_t length,
                                          enum cc3xx_rng_quality_t quality);

/**
 * @brief                        Get a random unsigned integer from the CC3XX
 *                               TRNG. The value is uniformly distributed
 *                               between 0 and bound - 1.
 *
 * @note                         This function may take a variable amount of
 *                               time to execute.
 *
 * @param[in]  bound             A value N such that 0 <= output < N
 * @param[out] uint              A pointer to the uint32_t to output into.
 * @param[in]  quality           The TRNG quality used to generate random data.
 *
 * @return                       CC3XX_ERR_SUCCESS on success, another
 *                               cc3xx_err_t on error.
 */
cc3xx_err_t cc3xx_lowlevel_rng_get_random_uint(uint32_t bound, uint32_t *uint,
                                               enum cc3xx_rng_quality_t quality);

/**
 * @brief                        Initialize a random permutation of a buffer containing
 *                               integers from 0 to len - 1.
 *                               The buffer may be used for DPA mitigations.
 *                               This function gets it's randomness from the TRNG.
 *
 * @note                         This function may take a variable amount of
 *                               time to execute.
 *
 * @param[out] permutation_buf   The buffer to fill with the random permutation.
 * @param[in]  len               Size of the buffer.
 * @param[in]  quality           The TRNG quality used to generate random data.
 *
 */
void cc3xx_lowlevel_rng_get_random_permutation(uint8_t *permutation_buf, size_t len,
                                               enum cc3xx_rng_quality_t quality);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_RNG_H */
