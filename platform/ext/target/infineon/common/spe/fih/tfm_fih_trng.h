/*
 * Copyright (c) 2026 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TFM_FIH_TRNG_H
#define TFM_FIH_TRNG_H

#include "cy_device.h"
#include "tfm_fih_platform.h"

/*
 * Obtain a 32-bit number from the TRNG
 * Returns 0 on failure.
 */
uint32_t ifx_trng(void);

#endif /* TFM_FIH_TRNG_H */
