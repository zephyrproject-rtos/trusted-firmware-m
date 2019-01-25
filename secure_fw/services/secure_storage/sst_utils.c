/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_utils.h"

#include <stdint.h>

#include "secure_fw/core/tfm_secure_api.h"
#include "tfm_sst_defs.h"

void sst_global_lock(void)
{
    /* FIXME: a system call to be added for acquiring lock */
    return;
}

void sst_global_unlock(void)
{
    /* FIXME: a system call to be added for releasing lock */
    return;
}

psa_ps_status_t sst_utils_check_contained_in(uint32_t superset_size,
                                             uint32_t subset_offset,
                                             uint32_t subset_size)
{
    /* Check that subset_offset is valid */
    if (subset_offset > superset_size) {
        return PSA_PS_ERROR_OFFSET_INVALID;
    }

    /* Check that subset_offset + subset_size fits in superset_size.
     * The previous check passed, so we know that subset_offset <= superset_size
     * and so the right hand side of the inequality cannot underflow.
     */
    if (subset_size > (superset_size - subset_offset)) {
        return PSA_PS_ERROR_INCORRECT_SIZE;
    }

    return PSA_PS_SUCCESS;
}

uint32_t sst_utils_validate_secure_caller(void)
{
    return tfm_core_validate_secure_caller();
}

psa_ps_status_t sst_utils_validate_fid(uint32_t fid)
{
    if (fid == SST_INVALID_FID) {
        return PSA_PS_ERROR_UID_NOT_FOUND;
    }

    return PSA_PS_SUCCESS;
}

/* FIXME: following functions are not optimized and will eventually to be
 *        replaced by system provided APIs.
 */
void sst_utils_memcpy(void *dest, const void *src, uint32_t size)
{
    uint32_t i;
    uint8_t *p_dst = (uint8_t *)dest;
    const uint8_t *p_src = (const uint8_t *)src;

    for (i = size; i > 0; i--) {
        *p_dst = *p_src;
        p_src++;
        p_dst++;
    }
}

void sst_utils_memset(void *dest, const uint8_t pattern, uint32_t size)
{
    uint32_t i;
    uint8_t *p_dst = (uint8_t *)dest;

    for (i = size; i > 0; i--) {
        *p_dst = pattern;
        p_dst++;
    }
}
