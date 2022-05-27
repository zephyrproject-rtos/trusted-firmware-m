/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdint.h>
#include <nrf.h>
#include "array.h"

#include <nrfx.h>

/* Static asserts to verify that asm_core_cm33 defines matches core_cm33 */
#include <core_cm33.h>
#include "asm_core_cm33.h"
NRFX_STATIC_ASSERT(_SCS_BASE_ADDR == SCS_BASE);
NRFX_STATIC_ASSERT(_SCS_MPU_CTRL == (uintptr_t)&MPU->CTRL);

/* This routine resets Cortex-M system control block components and core
 * registers.
 */
void hw_init_reset_on_boot(void)
{
	/* Disable interrupts */
	__disable_irq();

	/* Reset exception and interrupt mask state (PRIMASK handled by
	 * __enable_irq below)
	 */
	__set_FAULTMASK(0);
	__set_BASEPRI(0);

	/* Disable NVIC interrupts */
	for (int i = 0; i < ARRAY_SIZE(NVIC->ICER); i++) {
		NVIC->ICER[i] = 0xFFFFFFFF;
	}
	/* Clear pending NVIC interrupts */
	for (int i = 0; i < ARRAY_SIZE(NVIC->ICPR); i++) {
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}

	/* Restore Interrupts */
	__enable_irq();

	__DSB();
	__ISB();
}
