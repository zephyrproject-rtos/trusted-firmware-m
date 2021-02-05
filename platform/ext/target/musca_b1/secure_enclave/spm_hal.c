/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
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
#include "tfm_multi_core.h"
#include "platform_multicore.h"

#include "cmsis.h"

/* Declared in target_cfg.c */
extern const struct memory_region_limits memory_regions;

enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
        uint32_t partition_idx,
        const struct platform_data_t *platform_data)
{
    /* Nothing to do, there is no isolation HW in this platform to be
     * configured by Secure Enclave */
    (void) partition_idx;
    (void) platform_data;
    return TFM_PLAT_ERR_SUCCESS;
}

void tfm_spm_hal_boot_ns_cpu(uintptr_t start_addr)
{
    uint32_t translated_address = 0;

    platform_init_mailbox_hw();

    translated_address = (uint32_t) start_addr
            + 0x1A200000 /* eFlash 1 base from SSE-200's point of view */
            - 0x38000000 /* eFlash 1 base from SE's point of view */;

    platform_mailbox_send_msg_data(translated_address);
}

void tfm_spm_hal_wait_for_ns_cpu_ready(void)
{
    uint32_t data = 0;

    while (data != IPC_SYNC_MAGIC) {
        platform_mailbox_wait_for_notify();
        platform_mailbox_fetch_msg_data(&data);
    }
}

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line,
                                                        uint32_t priority)
{
    uint32_t quantized_priority = priority >> (8U - __NVIC_PRIO_BITS);
    NVIC_SetPriority(irq_line, quantized_priority);

    return TFM_PLAT_ERR_SUCCESS;
}

void tfm_spm_hal_get_mem_security_attr(const void *p, size_t s,
                                       struct security_attr_info_t *p_attr)
{
    /* Check static memory layout to get memory attributes */
    tfm_get_mem_region_security_attr(p, s, p_attr);
}

void tfm_spm_hal_get_secure_access_attr(const void *p, size_t s,
                                        struct mem_attr_info_t *p_attr)
{
    /* Check static memory layout to get memory attributes */
    tfm_get_secure_mem_region_attr(p, s, p_attr);

}

void tfm_spm_hal_get_ns_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr)
{
    /* Check static memory layout to get memory attributes */
    tfm_get_ns_mem_region_attr(p, s, p_attr);
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
