/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020, Nordic Semiconductor ASA. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "cmsis.h"
#include "tfm_spm_hal.h"
#include "tfm_platform_core_api.h"
#include "target_cfg.h"
#include "spu.h"
#include "mpu_armv8m_drv.h"
#include "region_defs.h"
#include "region.h"

/* Get address of memory regions to configure MPU */
extern const struct memory_region_limits memory_regions;

#if defined(CONFIG_TFM_ENABLE_MEMORY_PROTECT)
static struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };

#define PARTITION_REGION_PERIPH_START   6
#define PARTITION_REGION_PERIPH_MAX_NUM 2

static uint32_t periph_num_count = 0;
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

enum tfm_plat_err_t tfm_spm_hal_init_isolation_hw(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;

    /* Configures non-secure memory spaces in the target */
    sau_and_idau_cfg();

    err = spu_init_cfg();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = spu_periph_init_cfg();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_spm_hal_configure_default_isolation(
                  uint32_t partition_idx,
                  const struct tfm_spm_partition_platform_data_t *platform_data)
{
    if (!platform_data) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

#if defined(CONFIG_TFM_ENABLE_MEMORY_PROTECT) && (TFM_LVL != 1)

    if (!tfm_is_partition_privileged(partition_idx)) {
        struct mpu_armv8m_region_cfg_t region_cfg;

        region_cfg.region_nr = PARTITION_REGION_PERIPH_START + periph_num_count;
        periph_num_count++;
        if (periph_num_count >= PARTITION_REGION_PERIPH_MAX_NUM) {
            return TFM_PLAT_ERR_MAX_VALUE;
        }
        region_cfg.region_base = platform_data->periph_start;
        region_cfg.region_limit = platform_data->periph_limit;
        region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX;
        region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
        region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
        region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;

        mpu_armv8m_disable(&dev_mpu_s);

        if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg)
            != MPU_ARMV8M_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
        mpu_armv8m_enable(&dev_mpu_s, PRIVILEGED_DEFAULT_ENABLE,
                          HARDFAULT_NMI_ENABLE);
    }
#endif /* defined(CONFIG_TFM_ENABLE_MEMORY_PROTECT) && (TFM_LVL != 1) */

    /* Peripheral for a (secure) partition is configured as Secure.
     * Note that the SPU does not allow to configure PRIV/nPRIV permissions
     * for address ranges, including peripheral address space.
     */
    if (platform_data->periph_start != 0) {
        spu_periph_configure_to_secure(platform_data->periph_start);
    }

    return TFM_PLAT_ERR_SUCCESS;
}

void SPU_Handler(void)
{
    /*
     * TODO
     * Inspect RAMACCERR, FLASHACCERR,PERIPHACCERR to identify
     * the source of access violation.
     */

    /* Clear SPU interrupt flag and pending SPU IRQ */
    spu_clear_events();

    NVIC_ClearPendingIRQ(SPU_IRQn);

    /* Print fault message and block execution */
    ERROR_MSG("Oops... SPU fault!!!");

    /* Inform TF-M core that isolation boundary has been violated */
    tfm_access_violation_handler();
}

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
    return *((const uint32_t *)(memory_regions.non_secure_code_start + 4));
}

enum tfm_plat_err_t tfm_spm_hal_set_secure_irq_priority(IRQn_Type irq_line,
                                                        uint32_t priority)
{
    uint32_t quantized_priority = priority >> (8U - __NVIC_PRIO_BITS);
    NVIC_SetPriority(irq_line, quantized_priority);
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

enum tfm_plat_err_t tfm_spm_hal_enable_fault_handlers(void)
{
    return enable_fault_handlers();
}

enum tfm_plat_err_t tfm_spm_hal_system_reset_cfg(void)
{
    return system_reset_cfg();
}

enum tfm_plat_err_t tfm_spm_hal_init_debug(void)
{
    return init_debug();
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_target_state_cfg(void)
{
    return nvic_interrupt_target_state_cfg();
}

enum tfm_plat_err_t tfm_spm_hal_nvic_interrupt_enable(void)
{
    return nvic_interrupt_enable();
}

bool tfm_spm_hal_has_access_to_region(const void *p, size_t s,
                                              int flags)
{
    cmse_address_info_t tt_base = cmse_TT((void *)p);
    cmse_address_info_t tt_last = cmse_TT((void *)((uint32_t)p + s - 1));

    uint32_t base_spu_id = tt_base.flags.idau_region;
    uint32_t last_spu_id = tt_last.flags.idau_region;

    size_t size;
    uint32_t p_start = (uint32_t)p;
    int i;

    if ((base_spu_id >= spu_regions_flash_get_start_id()) &&
        (last_spu_id <= spu_regions_flash_get_last_id())) {

        size = spu_regions_flash_get_last_address_in_region(base_spu_id) + 1 - p_start;

        if (cmse_check_address_range((void *)p_start, size, flags) == 0) {
            return false;
        }

        for (i = base_spu_id + 1; i < last_spu_id; i++) {
            p_start = spu_regions_flash_get_base_address_in_region(i);
            if (cmse_check_address_range((void *)p_start,
                spu_regions_flash_get_region_size(), flags) == 0) {
                return false;
            }
        }

        p_start = spu_regions_flash_get_base_address_in_region(last_spu_id);
        size = (uint32_t)p + s - p_start;
        if (cmse_check_address_range((void *)p_start, size, flags) == 0) {
            return false;
        }


    } else if ((base_spu_id >= spu_regions_sram_get_start_id()) &&
        (last_spu_id <= spu_regions_sram_get_last_id())) {

        size = spu_regions_sram_get_last_address_in_region(base_spu_id) + 1 - p_start;
        if (cmse_check_address_range((void *)p_start, size, flags) == 0) {
            return false;
        }

        for (i = base_spu_id + 1; i < last_spu_id; i++) {
            p_start = spu_regions_sram_get_base_address_in_region(i);
            if (cmse_check_address_range((void *)p_start,
                spu_regions_sram_get_region_size(), flags) == 0) {
                return false;
            }
        }

        p_start = spu_regions_sram_get_base_address_in_region(last_spu_id);
        size = (uint32_t)p + s - p_start;
        if (cmse_check_address_range((void *)p_start, size, flags) == 0) {
            return false;
        }
    } else {
        return false;
    }

    return true;
}
