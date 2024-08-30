/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "trng.h"
#include "cc3xx_rng.h"
#include "tfm_plat_defs.h"
#include "device_definition.h"

int32_t bl1_trng_generate_random(uint8_t *output, size_t output_size)
{
    if (output_size == 0) {
        return TFM_PLAT_ERR_SUCCESS;
    }

    if (output == NULL) {
        return TFM_PLAT_ERR_ROM_TRNG_GENERATE_RANDOM_INVALID_INPUT;
    }

    return cc3xx_lowlevel_rng_get_random(output, output_size,
                                         CC3XX_RNG_CRYPTOGRAPHICALLY_SECURE);
}

unsigned char fih_delay_random_uchar(void) {
    /* Perform a hardware delay here, then return 0 so no software delay is
     * used.
     */
    kmu_random_delay(&KMU_DEV_S, KMU_DELAY_LIMIT_32_CYCLES);
    return 0;
}
