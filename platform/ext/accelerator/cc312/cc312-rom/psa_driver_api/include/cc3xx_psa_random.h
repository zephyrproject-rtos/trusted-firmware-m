/*
 * Copyright (c) 2023-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CC3XX_PSA_RANDOM_H__
#define __CC3XX_PSA_RANDOM_H__

/** @file cc3xx_psa_random.h
 *
 * This file contains the declaration of the entry points associated to the
 * random generation capability as described by the PSA Cryptoprocessor
 * Driver interface specification
 *
 */

#include "psa/crypto.h"
#include "cc3xx_crypto_primitives_private.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the RNG.
 *
 * @note  This function provides to seed itself on initialization requiring
 *        the expected amout of initial entropy from the TRNG as expected by
 *        the underlying DRBG mechanism configured at build time, before
 *        instantiating the DRBG. External initial_entropy_size property
 *        should be equal to 0.
 *
 * @param[out] context A pointer to a context holding state of the DRBG
 * @return psa_status_t
 */
psa_status_t cc3xx_init_random(cc3xx_random_context_t *context);

/**
 * @brief Injects entropy into the RNG state.
 *
 * @note  It is currently not supported as the driver expectation is to be
 *        treated as a full RNG that seeds (and re-seeds) itself when required,
 *        without expecting the core to provide entropy. If the design changes,
 *        this function might need to be implemented
 *
 * @param[in,out] context      A pointer to a context holding the state of the DRBG
 * @param[in]     entropy      Entropy to be injected into the DRBG state
 * @param[in]     entropy_size Size in bytes of the entropy to be injected
 * @return psa_status_t
 */
psa_status_t cc3xx_add_entropy(
    cc3xx_random_context_t *context,
    const uint8_t *entropy,
    size_t entropy_size);

/**
 * @brief Generates random numbers with a uniform distribution, according
 *        to the underlying DRBG mechanism configured at build time in the
 *        cc3xx_psa_config.h header
 *
 * @note  This function provides to reseed itself transparently from the
 *        calle when it detects that the underlying DRBG mechanism requires
 *        reseeding. External reseed_entropy_size property should be equal
 *        to 0.
 *
 * @param[in,out] context       A pointer to a context holding the state of the DRBG
 * @param[out]    output        Buffer containing the random data collected
 * @param[in]     output_size   Size in bytes of the buffer to fill
 * @param[out]    output_length Number of bytes effectively returned in \ref buffer
 * @return psa_status_t
 */
psa_status_t cc3xx_get_random(
    cc3xx_random_context_t *context,
    uint8_t *output,
    size_t output_size,
    size_t *output_length);

#ifdef __cplusplus
}
#endif

#endif /* __CC3XX_PSA_RANDOM_H__ */
