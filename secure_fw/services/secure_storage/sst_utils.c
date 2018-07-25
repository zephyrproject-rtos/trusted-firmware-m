/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "tfm_secure_api.h"
#include "tfm_sst_defs.h"
#include "assets/sst_asset_defs.h"
#include "sst_utils.h"

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

enum psa_sst_err_t sst_utils_memory_bound_check(void *addr,
                                                uint32_t size,
                                                uint32_t client_id,
                                                uint32_t access)
{
    enum psa_sst_err_t err;

    /* FIXME:
     * The only check that may be required is whether the caller client
     * has permission to read/write to the memory area specified
     * by addr and size.
     */
    (void) client_id;
    err = tfm_core_memory_permission_check(addr, size, access);

    return err;
}

enum psa_sst_err_t sst_utils_bound_check_and_copy(uint8_t *src,
                                                  uint8_t *dest,
                                                  uint32_t size,
                                                  uint32_t client_id)
{
    enum psa_sst_err_t bound_check;

    /* src is passed on from untrusted domain, verify boundary */
    bound_check = sst_utils_memory_bound_check(src, size, client_id,
                                               TFM_MEMORY_ACCESS_RO);
    if (bound_check == PSA_SST_ERR_SUCCESS) {
        sst_utils_memcpy(dest, src, size);
    }

    return bound_check;
}

enum psa_sst_err_t sst_utils_check_contained_in(uint32_t superset_start,
                                                uint32_t superset_size,
                                                uint32_t subset_start,
                                                uint32_t subset_size)
{
    /* Check if the subset is really within superset
     * if the subset's size is large enough, it can cause integer rollover
     * causing appearance of subset being within superset.
     * to avoid this, all of the parameters are promoted to 64 bit
     * so that 64 bit mathematics is performed (on actually 32 bit values)
     * removing possibility of rollovers.
     */
    uint64_t tmp_superset_start = superset_start;
    uint64_t tmp_superset_size = superset_size;
    uint64_t tmp_subset_start = subset_start;
    uint64_t tmp_subset_size = subset_size;
    enum psa_sst_err_t err = PSA_SST_ERR_SUCCESS;

    if ((tmp_subset_start < tmp_superset_start) ||
        ((tmp_subset_start + tmp_subset_size) >
         (tmp_superset_start + tmp_superset_size))) {
        err = PSA_SST_ERR_PARAM_ERROR;
    }
    return err;
}

uint32_t sst_utils_validate_secure_caller(void)
{
    return tfm_core_validate_secure_caller();
}

enum psa_sst_err_t sst_utils_validate_fid(uint32_t fid)
{
    if (fid == SST_INVALID_FID) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    return PSA_SST_ERR_SUCCESS;
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
