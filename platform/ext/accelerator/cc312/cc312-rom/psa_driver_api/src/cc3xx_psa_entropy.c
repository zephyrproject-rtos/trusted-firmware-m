/*
 * Copyright (c) 2021-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/** @file cc3xx_psa_entropy.c
 *
 * This file contains the implementation of the entry points associated
 * to the entropy capability as described by the PSA Cryptoprocessor
 * Driver interface specification
 *
 */

#include "cc3xx_psa_entropy.h"
#include "cc3xx_rng.h"
#include "cc3xx_misc.h"

/** @defgroup psa_entropy PSA driver entry points for entropy collection
 *
 *  Entry points for entropy collection from the TRNG source as described by the
 *  PSA Cryptoprocessor Driver interface specification. The TRNG
 *  operates by sampling the output of a fast free-running ring oscillator
 *
 *  @{
 */
psa_status_t cc3xx_get_entropy(uint32_t flags, size_t *estimate_bits,
                               uint8_t *output, size_t output_size)
{
    cc3xx_err_t err;

    CC3XX_ASSERT(output != NULL);
    CC3XX_ASSERT(estimate_bits != NULL);
    CC3XX_ASSERT(output_size != 0);

    *estimate_bits = 0;

    err = cc3xx_lowlevel_rng_get_random(output, output_size);
    if (err != CC3XX_ERR_SUCCESS) {
        return cc3xx_to_psa_err(err);
    }

    *estimate_bits = output_size * 8;

    return PSA_SUCCESS;
}
/** @} */ // end of psa_entropy

/*
 * FixMe: This function is required to integrate into Mbed TLS as the PSA
 * subsystem does not yet support entropy entry points. See the header
 * entropy_poll.h for details. This needs to be revised once Mbed TLS adds
 * support for entropy.
 */
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    (void)data;
    return cc3xx_get_entropy(0, olen, output, len);
}
