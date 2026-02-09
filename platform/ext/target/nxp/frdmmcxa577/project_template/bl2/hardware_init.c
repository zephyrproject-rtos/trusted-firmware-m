/*
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*${header:start}*/
#include "pin_mux.h"
#include "fsl_clock.h"
#include "fsl_reset.h"
#include "fsl_crc.h"
#include "fsl_glikey.h"
#include "board.h"

#include "fsl_trng.h"
/*${header:end}*/

/*${function:start}*/

void BOARD_InitHardware(void)
{
    BOARD_InitDEBUG_UARTPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();
    CLOCK_EnableClock(kCLOCK_GateSGI0);
}

void SystemInitHook (void)
{
    /* Void implementation of the weak function. */
    extern void *__VECTOR_TABLE[];
    SCB->VTOR = (uint32_t) & (__VECTOR_TABLE[0]);

    /* Board specific HW init*/
    BOARD_InitHardware();
}