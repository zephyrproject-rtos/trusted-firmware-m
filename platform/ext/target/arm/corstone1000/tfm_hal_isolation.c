/*
 * Copyright (c) 2020-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_api.h"
#include "tfm_hal_defs.h"
#include "tfm_multi_core.h"

#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT

#include "tfm_hal_isolation.h"
#include "mpu_config.h"

REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_APP_CODE_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_CODE_END, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_START, $$Base);
REGION_DECLARE(Image$$, TFM_APP_RW_STACK_END, $$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Limit);
#ifdef TFM_SP_META_PTR_ENABLE
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$RW$$Limit);
#endif /* TFM_SP_META_PTR_ENABLE */


static void configure_mpu(uint32_t rnr, uint32_t base, uint32_t limit,
                                uint32_t is_xn_exec, uint32_t ap_permissions)
{
    uint32_t size; /* region size */
    uint32_t rasr; /* region attribute and size register */
    uint32_t rbar; /* region base address register */

    size = get_rbar_size_field(limit - base);

    rasr = ARM_MPU_RASR(is_xn_exec, ap_permissions, TEX, NOT_SHAREABLE,
            YES_CACHEABLE, NOT_BUFFERABLE, SUB_REGION_DISABLE, size);
    rbar = base & MPU_RBAR_ADDR_Msk;

    ARM_MPU_SetRegionEx(rnr, rbar, rasr);
}

#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */

enum tfm_hal_status_t tfm_hal_set_up_static_boundaries(void)
{
#ifdef CONFIG_TFM_ENABLE_MEMORY_PROTECT
    uint32_t rnr = TFM_ISOLATION_REGION_START_NUMBER; /* current region number */
    uint32_t base; /* start address */
    uint32_t limit; /* end address */
    uint32_t total_region;

    /* ARMv6-M only supports a unified MPU so reading DREGION is enough */
    total_region = ((MPU->TYPE & MPU_TYPE_DREGION_Msk) >>
                                MPU_TYPE_DREGION_Pos);

    /* Clear all regions before initialization */
    for (uint32_t i = 0; i < total_region; i++) {
        ARM_MPU_ClrRegion(i);
    }

    ARM_MPU_Disable();

    /* TFM Core unprivileged code region */
    base = (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);

    configure_mpu(rnr++, base, limit, XN_EXEC_OK, AP_RO_PRIV_UNPRIV);

    /* NSPM PSP */
    base = (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Limit);

    configure_mpu(rnr++, base, limit, XN_EXEC_NOT_OK, AP_RW_PRIV_UNPRIV);

    /* RO region */
    base = (uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_START, $$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, TFM_APP_CODE_END, $$Base);

    configure_mpu(rnr++, base, limit, XN_EXEC_OK, AP_RO_PRIV_UNPRIV);

    /* RW, ZI and stack as one region */
    base = (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_START, $$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, TFM_APP_RW_STACK_END, $$Base);

    configure_mpu(rnr++, base, limit, XN_EXEC_NOT_OK, AP_RW_PRIV_UNPRIV);

#ifdef TFM_SP_META_PTR_ENABLE
    /* TFM partition metadata pointer region */
    base = (uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$RW$$Base);
    limit = (uint32_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$RW$$Limit);

    configure_mpu(rnr++, base, limit, XN_EXEC_NOT_OK, AP_RW_PRIV_UNPRIV);
#endif

    arm_mpu_enable();

#endif /* CONFIG_TFM_ENABLE_MEMORY_PROTECT */
    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_memory_has_access(uintptr_t base,
                                                size_t size,
                                                uint32_t attr)
{
    enum tfm_status_e status;

    status = tfm_has_access_to_region((const void *)base, size, attr);
    if (status != TFM_SUCCESS) {
         return TFM_HAL_ERROR_MEM_FAULT;
    }

    return TFM_HAL_SUCCESS;
}

/*
 * Implementation of tfm_hal_bind_boundaries() on Corstone1000:
 *
 * The API encodes some attributes into a handle and returns it to SPM.
 * The attributes include isolation boundaries, privilege, and MMIO information.
 * When scheduler switches running partitions, SPM compares the handle between
 * partitions to know if boundary update is necessary. If update is required,
 * SPM passes the handle to platform to do platform settings and update
 * isolation boundaries.
 */
enum tfm_hal_status_t tfm_hal_bind_boundaries(
                                    const struct partition_load_info_t *p_ldinf,
                                    void **pp_boundaries)
{
    (void)p_ldinf;
    (void)pp_boundaries;
    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_update_boundaries(
                             const struct partition_load_info_t *p_ldinf,
                             void *p_boundaries)
{
    (void)p_ldinf;
    (void)p_boundaries;
    return TFM_HAL_SUCCESS;
}
