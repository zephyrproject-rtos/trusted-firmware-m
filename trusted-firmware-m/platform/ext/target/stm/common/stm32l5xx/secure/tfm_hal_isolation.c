/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "Driver_Common.h"
#include "mpu_armv8m_drv.h"
#include "region.h"
#include "target_cfg.h"
#include "tfm_hal_isolation.h"
#include "tfm_plat_defs.h"

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
#define MPU_REGION_VENEERS           0
#define MPU_REGION_TFM_UNPRIV_CODE   1
#define MPU_REGION_TFM_UNPRIV_DATA   2
#define PARTITION_REGION_RO          4
#define PARTITION_REGION_RW_STACK    5
#define PARTITION_REGION_PERIPH      6
#define PARTITION_REGION_SHARE       7

#if TFM_LVL == 2
#define MPU_REGION_NS_STACK          3
#elif TFM_LVL == 3
#define MPU_REGION_NS_DATA           3
#endif

REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_UNPRIV_DATA, $$RW$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_DATA, $$ZI$$Limit);
#ifndef TFM_PSA_API
REGION_DECLARE(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Limit);
#endif
#if TFM_LVL == 2
REGION_DECLARE(Image$$, TFM_APP_CODE_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_CODE_END, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_END, $$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Limit);
#endif

/* Get address of memory regions to configure MPU */
extern const struct memory_region_limits memory_regions;

#if !defined(TFM_PSA_API)
/**
 * Set share region to which the partition needs access
 */
enum tfm_plat_err_t tfm_spm_hal_set_share_region(
        enum tfm_buffer_share_region_e share)
{
    struct mpu_armv8m_region_cfg_t region_cfg;
    enum tfm_plat_err_t res = TFM_PLAT_ERR_SYSTEM_ERR;
    uint32_t scratch_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
    uint32_t scratch_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Limit);

    mpu_armv8m_disable(&dev_mpu_s);

    if (share == TFM_BUFFER_SHARE_DISABLE) {
        mpu_armv8m_region_disable(&dev_mpu_s, PARTITION_REGION_SHARE);
    } else {

        region_cfg.region_nr = PARTITION_REGION_SHARE;
        region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
        region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
        region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
        region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
        switch (share) {
        case TFM_BUFFER_SHARE_SCRATCH:
            /* Use scratch area for SP-to-SP data sharing */
            region_cfg.region_base = scratch_base;
            region_cfg.region_limit = scratch_limit;
            res = TFM_PLAT_ERR_SUCCESS;
            break;
        case TFM_BUFFER_SHARE_NS_CODE:
            region_cfg.region_base = memory_regions.non_secure_partition_base;
            region_cfg.region_limit = memory_regions.non_secure_partition_limit;
            /* Only allow read access to NS code region and keep
             * exec.never attribute
             */
            region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
            res = TFM_PLAT_ERR_SUCCESS;
            break;
        default:
            /* Leave res to be set to SPM_ERR_INVALID_CONFIG */
            break;
        }
        if (res == TFM_PLAT_ERR_SUCCESS) {
            mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);
        }
    }
    mpu_armv8m_enable(&dev_mpu_s, PRIVILEGED_DEFAULT_ENABLE,
                      HARDFAULT_NMI_ENABLE);

    return res;
}
#endif /* !defined(TFM_PSA_API) */
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

enum tfm_hal_status_t tfm_hal_set_up_static_boundaries(void)
{
    /* Set up isolation boundaries between SPE and NSPE */
    gtzc_init_cfg();
    sau_and_idau_cfg();
    pinmux_init_cfg();

    /* Set up static isolation boundaries inside SPE */
#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
    struct mpu_armv8m_region_cfg_t region_cfg;
    struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };

    mpu_armv8m_clean(&dev_mpu_s);

    /* Veneer region */
    region_cfg.region_nr = MPU_REGION_VENEERS;
    region_cfg.region_base = memory_regions.veneer_base;
    region_cfg.region_limit = memory_regions.veneer_limit;
    region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_CODE_IDX;
    region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
    if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK) {
        return TFM_HAL_ERROR_GENERIC;
    }

    /* TFM Core unprivileged code region */
    region_cfg.region_nr = MPU_REGION_TFM_UNPRIV_CODE;
    region_cfg.region_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
    region_cfg.region_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);
    region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_CODE_IDX;
    region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
    if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK) {
        return TFM_HAL_ERROR_GENERIC;
    }

    /* TFM Core unprivileged data region */
    region_cfg.region_nr = MPU_REGION_TFM_UNPRIV_DATA;
    region_cfg.region_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_DATA, $$RW$$Base);
    region_cfg.region_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_DATA, $$ZI$$Limit);
    region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
    region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
    if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK) {
        return TFM_HAL_ERROR_GENERIC;
    }

#if TFM_LVL == 3
    /* TFM Core unprivileged non-secure data region */
    region_cfg.region_nr = MPU_REGION_NS_DATA;
    region_cfg.region_base = NS_DATA_START;
    region_cfg.region_limit = NS_DATA_LIMIT;
    region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
    region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
    if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK) {
        return TFM_HAL_ERROR_GENERIC;
    }
#endif

#if TFM_LVL == 2
    /* NSPM PSP */
    region_cfg.region_nr = MPU_REGION_NS_STACK;
    region_cfg.region_base =
        (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base);
    region_cfg.region_limit =
        (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Limit);
    region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
    region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
    if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK) {
        return TFM_HAL_ERROR_GENERIC;
    }

    /* RO region */
    region_cfg.region_nr = PARTITION_REGION_RO;
    region_cfg.region_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_START, $$Base);
    region_cfg.region_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_END, $$Base);
    region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_CODE_IDX;
    region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
    if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK) {
        return TFM_HAL_ERROR_GENERIC;
    }

    /* RW, ZI and stack as one region */
    region_cfg.region_nr = PARTITION_REGION_RW_STACK;
    region_cfg.region_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_START, $$Base);
    region_cfg.region_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_END, $$Base);
    region_cfg.region_attridx = MPU_ARMV8M_MAIR_ATTR_DATA_IDX;
    region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
    if (mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg) != MPU_ARMV8M_OK) {
        return TFM_HAL_ERROR_GENERIC;
    }
#endif

    mpu_armv8m_enable(&dev_mpu_s, PRIVILEGED_DEFAULT_ENABLE,
                      HARDFAULT_NMI_ENABLE);
#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

    return TFM_HAL_SUCCESS;
}
