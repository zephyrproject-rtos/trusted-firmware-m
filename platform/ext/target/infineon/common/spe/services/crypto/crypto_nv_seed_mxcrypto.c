/*
 * Copyright (c) 2026 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#include "cy_crypto_core_hw.h"
#include "cy_crypto_core_trng.h"
#include "cy_device.h"
#include "tfm_plat_crypto_nv_seed.h"

int tfm_plat_crypto_nv_seed_read(unsigned char *buf, size_t buf_len)
{
    cy_en_crypto_status_t status;
    uint32_t random;

    /* Crypto core can be disabled without acceleration */
    bool is_crypto_enabled = Cy_Crypto_Core_IsEnabled(CRYPTO);
    if (!is_crypto_enabled) {
        status = Cy_Crypto_Core_Enable(CRYPTO);
        if (status != CY_CRYPTO_SUCCESS) {
            return TFM_CRYPTO_NV_SEED_FAILED;
        }
    }

    while (buf_len > 0U) {
        /* Use TRNG to generate a single random 32-bit number */
        status = Cy_Crypto_Core_Trng_Ext(CRYPTO, 32, &random);
        if (status != CY_CRYPTO_SUCCESS) {
            (void)Cy_Crypto_Core_Trng_DeInit(CRYPTO);
            if (!is_crypto_enabled) {
                (void)Cy_Crypto_Core_Disable(CRYPTO);
            }
            return TFM_CRYPTO_NV_SEED_FAILED;
        }

        size_t chunk_size = (buf_len > 4U) ? 4U : buf_len;
        (void)memcpy(buf, &random, chunk_size);
        buf_len -= chunk_size;
        buf += chunk_size;
    }

    /* We're done with the TRNG */
    (void)Cy_Crypto_Core_Trng_DeInit(CRYPTO);
    if (!is_crypto_enabled) {
        (void)Cy_Crypto_Core_Disable(CRYPTO);
    }

    return TFM_CRYPTO_NV_SEED_SUCCESS;
}
