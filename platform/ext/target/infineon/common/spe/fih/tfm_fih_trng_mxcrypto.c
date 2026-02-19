/*
 * Copyright (c) 2026 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cy_crypto_core_trng.h"
#include "tfm_fih_trng.h"

/*
 * Obtain a 32-bit number from the TRNG
 * Returns 0 on failure.
 */
uint32_t ifx_trng(void)
{
    uint32_t random = 0;
    cy_en_crypto_status_t status;

    status = Cy_Crypto_Core_Enable(CRYPTO);
    if (status != CY_CRYPTO_SUCCESS) {
        return 0;
    }

    /* Use TRNG to generate a single random 32-bit number */
    status = Cy_Crypto_Core_Trng_Ext(CRYPTO, 32, &random);
    if (status != CY_CRYPTO_SUCCESS) {
        return 0;
    }

    /* We're done with the TRNG and Crypto block */
    Cy_Crypto_Core_Trng_DeInit(CRYPTO);
    (void)Cy_Crypto_Core_Disable(CRYPTO);

    return random;
}
