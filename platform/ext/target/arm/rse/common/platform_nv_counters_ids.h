/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_NV_COUNTERS_IDS_H__
#define __PLATFORM_NV_COUNTERS_IDS_H__

#include <stdint.h>
#include "rse_nv_counter_config.h"

#ifdef __cplusplus
extern "C" {
#endif

enum tfm_nv_counter_t {
    PLAT_NV_COUNTER_PS_0 = 0,  /* Used by PS service */
    PLAT_NV_COUNTER_PS_1,      /* Used by PS service */
    PLAT_NV_COUNTER_PS_2,      /* Used by PS service */

    PLAT_NV_COUNTER_BL1_0,
    PLAT_NV_COUNTER_BL1_MAX = PLAT_NV_COUNTER_BL1_0 + RSE_NV_COUNTER_BL1_AMOUNT,

    PLAT_NV_COUNTER_BL2_0,
    PLAT_NV_COUNTER_BL2_MAX = PLAT_NV_COUNTER_BL2_0 + RSE_NV_COUNTER_BL2_AMOUNT,

    PLAT_NV_COUNTER_HOST_0,
    PLAT_NV_COUNTER_HOST_MAX = PLAT_NV_COUNTER_HOST_0 + RSE_NV_COUNTER_HOST_AMOUNT,

    PLAT_NV_COUNTER_SUBPLATFORM_0,
    PLAT_NV_COUNTER_SUBPLATFORM_MAX = PLAT_NV_COUNTER_SUBPLATFORM_0 + RSE_NV_COUNTER_SUBPLATFORM_AMOUNT,

    PLAT_NV_COUNTER_LFT,
    PLAT_NV_COUNTER_KRTL_USAGE,

    PLAT_NV_COUNTER_CM_ROTPK_REPROVISIONING,
    PLAT_NV_COUNTER_DM_ROTPK_REPROVISIONING,

    PLAT_NV_COUNTER_MAX,
    PLAT_NV_COUNTER_BOUNDARY = UINT32_MAX  /* Fix tfm_nv_counter_t size
                                              to 4 bytes */
};

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_NV_COUNTERS_IDS_H__ */
