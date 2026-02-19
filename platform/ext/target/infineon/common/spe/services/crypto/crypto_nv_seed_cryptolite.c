/*
 * Copyright (c) 2026 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#include "cy_cryptolite_trng.h"
#include "cy_device.h"
#include "tfm_plat_crypto_nv_seed.h"

int tfm_plat_crypto_nv_seed_read(unsigned char *buf, size_t buf_len)
{
    cy_en_cryptolite_status_t status;
    uint32_t random_u32;

    status = Cy_Cryptolite_Trng_Init(CRYPTOLITE, NULL);
    if (status != CY_CRYPTOLITE_SUCCESS) {
        return TFM_CRYPTO_NV_SEED_FAILED;
    }

    status = Cy_Cryptolite_Trng_Enable(CRYPTOLITE);
    if (status != CY_CRYPTOLITE_SUCCESS) {
        (void)Cy_Cryptolite_Trng_DeInit(CRYPTOLITE);
        return TFM_CRYPTO_NV_SEED_FAILED;
    }

    while (buf_len > 0u) {
        /* Use TRNG to generate a single random 32-bit number */
        status = Cy_Cryptolite_Trng_ReadData(CRYPTOLITE, &random_u32);
        if (status != CY_CRYPTOLITE_SUCCESS) {
            (void)Cy_Cryptolite_Trng_Disable(CRYPTOLITE);
            (void)Cy_Cryptolite_Trng_DeInit(CRYPTOLITE);
            return TFM_CRYPTO_NV_SEED_FAILED;
        }

        size_t chunk_size = (buf_len > 4u) ? 4u : buf_len;
        (void)memcpy(buf, &random_u32, chunk_size);
        buf_len -= chunk_size;
        buf += chunk_size;
    }

    (void)Cy_Cryptolite_Trng_Disable(CRYPTOLITE);
    (void)Cy_Cryptolite_Trng_DeInit(CRYPTOLITE);

    return TFM_CRYPTO_NV_SEED_SUCCESS;
}
