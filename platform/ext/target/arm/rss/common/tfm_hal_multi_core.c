/*
 * Copyright (c) 2022-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_hal_multi_core.h"
#include "tfm_hal_isolation.h"
#include "psa/service.h"
#include "current.h"

void tfm_hal_boot_ns_cpu(uintptr_t start_addr)
{
    /* Nothing to do, other CPUs are booted by BL2 */
    (void)start_addr;
    return;
}

void tfm_hal_wait_for_ns_cpu_ready(void)
{
    /* Nothing to do, all necessary synchronization done by BL2 */
    return;
}

void tfm_hal_get_mem_security_attr(const void *p, size_t s,
                                   struct security_attr_info_t *p_attr)
{
    /* Multi-core mem-check expects that the memory will be Non-secure, but on
     * RSS it will be Secure (either MHU messages copied into local memory or a
     * Secure-access ATU window). Pretend it is NS to keep the mem-check happy.
     */
    (void)p;
    (void)s;
    p_attr->is_valid = true;
    p_attr->is_secure = false;
}

void tfm_hal_get_secure_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr)
{
    /* Should be unreachable */
    (void)p;
    (void)s;
    (void)p_attr;
    psa_panic();
}

void tfm_hal_get_ns_access_attr(const void *p, size_t s,
                                struct mem_attr_info_t *p_attr)
{
#if TFM_ISOLATION_LEVEL >= 2
    p_attr->is_mpu_enabled = true;
#endif
    p_attr->is_xn = true;

    /* Assume unpriv access is not required */
    p_attr->is_unpriv_rd_allow = false;
    p_attr->is_unpriv_wr_allow = false;

    if (tfm_hal_memory_check((GET_CURRENT_COMPONENT())->boundary, (uintptr_t)p, s,
                             TFM_HAL_ACCESS_READWRITE) == TFM_HAL_SUCCESS) {
        /* MPU region is RW */
        p_attr->is_valid = true;
        p_attr->is_priv_rd_allow = true;
        p_attr->is_priv_wr_allow = true;
    } else if (tfm_hal_memory_check((GET_CURRENT_COMPONENT())->boundary, (uintptr_t)p, s,
                                    TFM_HAL_ACCESS_READABLE) == TFM_HAL_SUCCESS) {
        /* MPU region is RO */
        p_attr->is_valid = true;
        p_attr->is_priv_rd_allow = true;
        p_attr->is_priv_wr_allow = false;
    } else {
        /* MPU region is not accessible */
        p_attr->is_valid = false;
        p_attr->is_priv_rd_allow = false;
        p_attr->is_priv_wr_allow = false;
    }
}
