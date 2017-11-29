/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include "tfm_ss_core_test_2.h"
#include "tfm_api.h"
#include "tfm_secure_api.h"
#include "test/test_services/tfm_core_test/core_test_defs.h"

int32_t core_test_2_init(void)
{
    return TFM_ERROR_GENERIC;
}

int32_t spm_core_test_2_slave_service(void)
{
    return TFM_SUCCESS;
}

/* Invert function */
#define SFN_INVERT_MAX_LEN 128

int32_t spm_core_test_2_sfn_invert(int32_t *res_ptr, uint32_t *in_ptr,
                                   uint32_t *out_ptr, int32_t len)
{
    int32_t i;
    static uint32_t invert_buffer[SFN_INVERT_MAX_LEN];

    if (tfm_core_memory_permission_check(res_ptr, sizeof(int32_t),
        TFM_MEMORY_ACCESS_RW) != TFM_SUCCESS) {
        return TFM_SERVICE_BUSY;
    }

    *res_ptr = -1;

    if (len > SFN_INVERT_MAX_LEN) {
        return TFM_SERVICE_BUSY;
    }

    /* Check requires byte-based size */
    if ((tfm_core_memory_permission_check(in_ptr, len << 2,
        TFM_MEMORY_ACCESS_RW) != TFM_SUCCESS) ||
        (tfm_core_memory_permission_check(out_ptr, len << 2,
        TFM_MEMORY_ACCESS_RW) != TFM_SUCCESS)) {
        return TFM_SERVICE_BUSY;
    }

    for (i = 0; i < len; i++) {
        invert_buffer[i] = in_ptr[i];
    }
    for (i = 0; i < len; i++) {
        invert_buffer[i] = ~invert_buffer[i];
    }
    for (i = 0; i < len; i++) {
        out_ptr[i] = invert_buffer[i];
    }

    *res_ptr = 0;
    return TFM_SUCCESS;
}
