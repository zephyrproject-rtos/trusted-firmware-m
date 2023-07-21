/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_hal_multi_core.h"

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
    /* Check static memory layout to get memory attributes */
    tfm_get_mem_region_security_attr(p, s, p_attr);
}

void tfm_hal_get_secure_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr)
{
    /* Check static memory layout to get memory attributes */
    tfm_get_secure_mem_region_attr(p, s, p_attr);
#if TFM_LVL >= 2
    p_attr->is_mpu_enabled = true;
#endif
}

void tfm_hal_get_ns_access_attr(const void *p, size_t s,
                                struct mem_attr_info_t *p_attr)
{
    /* Check static memory layout to get memory attributes */
    tfm_get_ns_mem_region_attr(p, s, p_attr);
}
