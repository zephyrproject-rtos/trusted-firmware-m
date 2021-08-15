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
#include "log/tfm_log.h"
#include "tfm_multi_core.h"
#include "tfm_platform_core_api.h"
#include "tfm_hal_isolation.h"

#include "platform_base_address.h"

#include "flash_common.h"

#define HOST_SYS_RST_CTRL_OFFSET 0x0
#define HOST_CPU_CORE0_WAKEUP_OFFSET 0x308
#define HOST_CPU_PE0_CONFIG_OFFSET 0x010
#define AA64nAA32_MASK (1 << 3)

void tfm_spm_hal_boot_ns_cpu(uintptr_t start_addr)
{
    /* Switch the shared flash to XiP mode for the host */
    Select_XIP_Mode_For_Shared_Flash();

    volatile uint32_t *bir_base = (uint32_t *)DIPHDA_HOST_BIR_BASE;

    /* Program Boot Instruction Register to jump to BL2 (TF-A) base address
     * at 0x02354000, corresponding assembler instructions are as below.
     * 0x58000040     ldr       x0, Label
     * 0xD61F0000     br        x0
     * 0x02354000     Label:    .dword 0x020d3000
     */
    bir_base[0] = 0x58000040;
    bir_base[1] = 0xD61F0000;
    bir_base[2] = 0x02354000;

    volatile uint32_t *reset_ctl_reg = (uint32_t *)(DIPHDA_BASE_SCR_BASE
                                                    + HOST_SYS_RST_CTRL_OFFSET);
    volatile uint32_t *reset_ctl_wakeup_reg =
                        (uint32_t *)(DIPHDA_HOST_BASE_SYSTEM_CONTROL_BASE
                                     + HOST_CPU_CORE0_WAKEUP_OFFSET);

    volatile uint32_t *PE0_CONFIG =
                        (uint32_t *)(DIPHDA_HOST_BASE_SYSTEM_CONTROL_BASE
                                     + HOST_CPU_PE0_CONFIG_OFFSET);

    /* Select host CPU architecture as AArch64 */
    *PE0_CONFIG |= AA64nAA32_MASK; /* 0b1 – AArch64 */

    /* wakeup CORE0 before bringing it out of reset */
    *reset_ctl_wakeup_reg = 0x1;

    /* Clear HOST_SYS_RST_CTRL register to bring host out of RESET */
    *reset_ctl_reg = 0;

    (void) start_addr;
}

void tfm_spm_hal_wait_for_ns_cpu_ready(void)
{
    /* Synchronization between Host and SE is done by OpenAMP */
}

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line)
{
    NVIC_SetPriority(irq_line, DEFAULT_IRQ_PRIORITY);

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
#if TFM_LVL >= 2
    p_attr->is_mpu_enabled = true;
#endif
}

void tfm_spm_hal_get_ns_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr)
{
    /* Check static memory layout to get memory attributes */
    tfm_get_ns_mem_region_attr(p, s, p_attr);
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
