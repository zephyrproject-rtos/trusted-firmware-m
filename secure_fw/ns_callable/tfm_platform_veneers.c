/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_platform_veneers.h"
#include "secure_fw/services/platform/platform_sp.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "spm_partition_defs.h"

__tfm_secure_gateway_attributes__
enum tfm_platform_err_t tfm_platform_veneer_system_reset(void)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_PLATFORM_ID,
                         platform_sp_system_reset,
                         0, 0, 0, 0);
}
