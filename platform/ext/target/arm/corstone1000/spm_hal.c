/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "tfm_spm_hal.h"

#include "device_definition.h"
#include "region_defs.h"
#include "tfm_multi_core.h"
#include "tfm_platform_core_api.h"
#include "tfm_hal_isolation.h"

#include "platform_base_address.h"

#include "flash_common.h"

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line)
{
    NVIC_SetPriority(irq_line, DEFAULT_IRQ_PRIORITY);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_enable(void)
{
    /* Nothing to do, no interrupt needs to be enabled in Secure Enclave */
    return TFM_PLAT_ERR_SUCCESS;
}

void tfm_spm_hal_clear_pending_irq(IRQn_Type irq_line)
{
    NVIC_ClearPendingIRQ(irq_line);
}

void tfm_spm_hal_enable_irq(IRQn_Type irq_line)
{
    NVIC_EnableIRQ(irq_line);
}

void tfm_spm_hal_disable_irq(IRQn_Type irq_line)
{
    NVIC_DisableIRQ(irq_line);
}

enum irq_target_state_t tfm_spm_hal_set_irq_target_state(
                                          IRQn_Type irq_line,
                                          enum irq_target_state_t target_state)
{
    /* Nothing to do, target state of interrupts cannot be set on Armv6-m */
    (void)irq_line;
    (void)target_state;

    return TFM_IRQ_TARGET_STATE_SECURE;
}

uint32_t tfm_spm_hal_get_ns_entry_point(void)
{
    /* Boot of Host processor not implemented yet. */
    return 0;
}

uint32_t tfm_spm_hal_get_ns_VTOR(void)
{
    /* Boot of Host processor not implemented yet. */
    return 0;
}
