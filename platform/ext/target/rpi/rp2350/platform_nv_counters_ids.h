/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#ifndef __PLATFORM_NV_COUNTERS_IDS_H__
#define __PLATFORM_NV_COUNTERS_IDS_H__

#include <stdint.h>

enum tfm_nv_counter_t {
    PLAT_NV_COUNTER_PS_0 = 0,  /* Used by PS service */
    PLAT_NV_COUNTER_PS_1,      /* Used by PS service */
    PLAT_NV_COUNTER_PS_2,      /* Used by PS service */

    /* BL2 NV counters must be contiguous */
    PLAT_NV_COUNTER_BL2_0,     /* Used by bootloader */
    PLAT_NV_COUNTER_BL2_1,     /* Used by bootloader */
    PLAT_NV_COUNTER_BL2_2,     /* Used by bootloader */
    PLAT_NV_COUNTER_BL2_3,     /* Used by bootloader */

    /* NS counters must be contiguous */
    PLAT_NV_COUNTER_NS_0,      /* Used by NS */
    PLAT_NV_COUNTER_NS_1,      /* Used by NS */
    PLAT_NV_COUNTER_NS_2,      /* Used by NS */

    PLAT_NV_COUNTER_MAX,
    PLAT_NV_COUNTER_BOUNDARY = UINT32_MAX  /* Fix  tfm_nv_counter_t size
                                              to 4 bytes */
};

#endif /* __PLATFORM_NV_COUNTERS_IDS_H__ */
