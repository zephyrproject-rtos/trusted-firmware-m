/*
 * Copyright (c) 2020-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "mmio_defs.h"
#include "tfm_api.h"
#include "tfm_hal_defs.h"
#include "tfm_multi_core.h"
#include "load/spm_load_api.h"

enum tfm_hal_status_t tfm_hal_set_up_static_boundaries(void)
{
    /* Nothing to do, there is no isolation HW in this platform to be
     * configured by Secure Enclave */
    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_memory_check(uintptr_t boundary,
                                           uintptr_t base,
                                           size_t size,
                                           uint32_t access_type)
{
    enum tfm_status_e status;
    uint32_t flags = 0;

    if ((access_type & TFM_HAL_ACCESS_READWRITE) == TFM_HAL_ACCESS_READWRITE) {
        flags |= MEM_CHECK_MPU_READWRITE;
    } else if (access_type & TFM_HAL_ACCESS_READABLE) {
        flags |= MEM_CHECK_MPU_READ;
    } else {
        return TFM_HAL_ERROR_INVALID_INPUT;
    }

    if (!((uint32_t)boundary & HANDLE_ATTR_PRIV_MASK)) {
        flags |= MEM_CHECK_MPU_UNPRIV;
    }

    if ((uint32_t)boundary & HANDLE_ATTR_NS_MASK) {
        flags |= MEM_CHECK_NONSECURE;
    }

    status = tfm_has_access_to_region((const void *)base, size, flags);
    if (status != TFM_SUCCESS) {
         return TFM_HAL_ERROR_MEM_FAULT;
    }

    return TFM_HAL_SUCCESS;
}

/*
 * Implementation of tfm_hal_bind_boundary() on Secure enclave:
 */
enum tfm_hal_status_t tfm_hal_bind_boundary(
                                    const struct partition_load_info_t *p_ldinf,
                                    uintptr_t *p_boundary)
{
    bool privileged = true;
    bool ns_agent = false;
    uint32_t partition_attrs = 0;

    if (!p_ldinf || !p_boundary) {
        return TFM_HAL_ERROR_GENERIC;
    }

    partition_attrs = ((uint32_t)privileged << HANDLE_ATTR_PRIV_POS) &
                       HANDLE_ATTR_PRIV_MASK;
    partition_attrs |= ((uint32_t)ns_agent << HANDLE_ATTR_NS_POS) &
                        HANDLE_ATTR_NS_MASK;
    *p_boundary = (uintptr_t)partition_attrs;

    return TFM_HAL_SUCCESS;
}

enum tfm_hal_status_t tfm_hal_activate_boundary(
                             const struct partition_load_info_t *p_ldinf,
                             uintptr_t boundary)
{
    (void)p_ldinf;
    (void)boundary;
    return TFM_HAL_SUCCESS;
}
