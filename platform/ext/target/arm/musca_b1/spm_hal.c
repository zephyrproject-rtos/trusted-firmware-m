/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "tfm_spm_hal.h"
#include "target_cfg.h"
#include "utilities.h"
#include "tfm_hal_platform.h"

/* Get address of memory regions to configure MPU */
extern const struct memory_region_limits memory_regions;

uint32_t tfm_spm_hal_get_ns_VTOR(void)
{
    return memory_regions.non_secure_code_start;
}

uint32_t tfm_spm_hal_get_ns_MSP(void)
{
    return *((uint32_t *)memory_regions.non_secure_code_start);
}

uint32_t tfm_spm_hal_get_ns_entry_point(void)
{
    return *((uint32_t *)(memory_regions.non_secure_code_start+ 4));
}

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line)
{
    NVIC_SetPriority(irq_line, DEFAULT_IRQ_PRIORITY);
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
    uint32_t result;

    if (target_state == TFM_IRQ_TARGET_STATE_SECURE) {
        result = NVIC_ClearTargetState(irq_line);
    } else {
        result = NVIC_SetTargetState(irq_line);
    }

    if (result) {
        return TFM_IRQ_TARGET_STATE_NON_SECURE;
    } else {
        return TFM_IRQ_TARGET_STATE_SECURE;
    }
}

enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
                  bool privileged,
                  const struct platform_data_t *platform_data)
{

    if (!platform_data) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    if (platform_data->periph_ppc_bank != PPC_SP_DO_NOT_CONFIGURE) {
        ppc_configure_to_secure(platform_data->periph_ppc_bank,
                                platform_data->periph_ppc_loc);
        if (privileged) {
            ppc_clr_secure_unpriv(platform_data->periph_ppc_bank,
                                  platform_data->periph_ppc_loc);
        } else {
            ppc_en_secure_unpriv(platform_data->periph_ppc_bank,
                                 platform_data->periph_ppc_loc);
        }
    }
    return TFM_PLAT_ERR_SUCCESS;
}
