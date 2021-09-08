/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 * Copyright (c) 2019-2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>

#include "tfm_spm_hal.h"
#include "device_definition.h"
#include "region_defs.h"
#include "target_cfg.h"

#include "cmsis.h"

/* Declared in target_cfg.c */
extern const struct memory_region_limits memory_regions;

enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
        bool privileged,
        const struct platform_data_t *platform_data)
{
    /* Nothing to do, there is no isolation HW in this platform to be
     * configured by Secure Enclave */
    (void) privileged;
    (void) platform_data;
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line)
{
    NVIC_SetPriority(irq_line, DEFAULT_IRQ_PRIORITY);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_enable(void)
{
    /* Nothing to do, mailbox interrupt enabled at mailbox initialization,
     * no other interrupt source used in Secure Enclave */
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

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_target_state_cfg(void)
{
    /* Nothing to do, target state of interrupts cannot be set on Armv6-m */
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_enable_fault_handlers(void)
{
    /* Nothing to do, fault handlers are not implemented on Armv6-m */
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_system_reset_cfg(void)
{
    /* Nothing to do, system reset do no require any initialization */
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_init_debug(void)
{
    /* Nothing to do, no initialization options for the debug subsystem on
     * Armv6-m */
    return TFM_PLAT_ERR_SUCCESS;
}

uint32_t tfm_spm_hal_get_ns_VTOR(void)
{
    return memory_regions.non_secure_code_start;
}

uint32_t tfm_spm_hal_get_ns_entry_point(void)
{
    return *((uint32_t *)(memory_regions.non_secure_code_start+ 4));
}
