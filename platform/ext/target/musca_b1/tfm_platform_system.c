/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform/include/tfm_platform_system.h"
#include "platform_description.h"
#include "target_cfg.h"

void tfm_platform_hal_system_reset(void)
{
    __disable_irq();
    mpc_revert_non_secure_to_secure_cfg();

    NVIC->ICPR[0] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[1] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[2] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[3] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[4] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[5] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[6] = UINT32_MAX;         /* Clear all pending interrupts */
    NVIC->ICPR[7] = UINT32_MAX;         /* Clear all pending interrupts */

    NVIC_SystemReset();
}

