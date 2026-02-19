/*
 * Copyright (c) 2026 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cy_cryptolite_trng.h"
#include "tfm_fih_trng.h"

/*
 * Obtain a 32-bit number from the TRNG
 * Returns 0 on failure.
 */
uint32_t ifx_trng(void)
{
    uint32_t random = 0;
    cy_en_cryptolite_status_t status;

    status = Cy_Cryptolite_Trng_Init(CRYPTOLITE, NULL);
    if (status != CY_CRYPTOLITE_SUCCESS) {
        return 0;
    }

    status = Cy_Cryptolite_Trng_Enable(CRYPTOLITE);
    if (status != CY_CRYPTOLITE_SUCCESS) {
        (void)Cy_Cryptolite_Trng_DeInit(CRYPTOLITE);
        return 0;
    }

    /* Use TRNG to generate a single random 32-bit number */
    status = Cy_Cryptolite_Trng_ReadData(CRYPTOLITE, &random);
    if (status != CY_CRYPTOLITE_SUCCESS) {
        (void)Cy_Cryptolite_Trng_Disable(CRYPTOLITE);
        (void)Cy_Cryptolite_Trng_DeInit(CRYPTOLITE);
        return 0;
    }

    /* We're done with the TRNG and Crypto block */
    (void)Cy_Cryptolite_Trng_Disable(CRYPTOLITE);
    (void)Cy_Cryptolite_Trng_DeInit(CRYPTOLITE);

    return random;
}
