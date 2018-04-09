/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform/include/tfm_spm_hal.h"
#include "spm_api.h"
#include "spm_db.h"

void tfm_spm_hal_init_platform_data(uint32_t partition_id,
        struct tfm_spm_partition_platform_data_t *platform_data)
{
    platform_data->periph_start = 0;
    platform_data->periph_limit = 0;
    platform_data->periph_ppc_bank = 0;
    platform_data->periph_ppc_loc = 0;
}
