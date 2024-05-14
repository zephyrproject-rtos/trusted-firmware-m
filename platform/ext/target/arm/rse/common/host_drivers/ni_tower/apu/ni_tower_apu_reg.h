/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_APU_REG_H__
#define __NI_TOWER_APU_REG_H__

#include "tfm_hal_device_header.h"

#include <stdint.h>

#define NI_TOWER_MAX_APU_REGIONS    32

/**
 * \brief NI-Tower APU register map
 */
__PACKED_STRUCT ni_tower_apu_reg_map {
    __PACKED_STRUCT {
        __IOM uint32_t prbar_low;
        __IOM uint32_t prbar_high;
        __IOM uint32_t prlar_low;
        __IOM uint32_t prlar_high;
        __IOM uint32_t prid_low;
        __IOM uint32_t prid_high;
        const uint32_t reserved_2[2];
    } region[NI_TOWER_MAX_APU_REGIONS];
    const uint32_t reserved_3[766];
    __IOM uint32_t apu_ctlr;
    __IM  uint32_t apu_iidr;
};

/*  Field definitions for prbar_low register */
#define NI_TOWER_APU_REGION_ENABLE_POS         (0U)
#define NI_TOWER_APU_REGION_ENABLE_MSK         (0x1UL << NI_TOWER_APU_REGION_ENABLE_POS)
#define NI_TOWER_APU_REGION_ENABLE             NI_TOWER_APU_REGION_ENABLE_MSK
#define NI_TOWER_APU_BR_POS                    (1U)
#define NI_TOWER_APU_BR_MSK                    (0x1UL << NI_TOWER_APU_BR_POS)
#define NI_TOWER_APU_BR                        NI_TOWER_APU_BR_MSK
#define NI_TOWER_APU_LOCK_POS                  (2U)
#define NI_TOWER_APU_LOCK_MSK                  (0x1UL << NI_TOWER_APU_LOCK_POS)
#define NI_TOWER_APU_LOCK                      NI_TOWER_APU_LOCK_MSK

/*  Field definitions for prlar_low register */
#define NI_TOWER_APU_ID_VALID_POS              (0U)
#define NI_TOWER_APU_ID_VALID_MSK              (0xFUL << NI_TOWER_APU_ID_VALID_POS)
#define NI_TOWER_APU_ID_VALID                  NI_TOWER_APU_ID_VALID_MSK

/*  Field definitions for prid_low register */
#define NI_TOWER_APU_ID_0_POS                  (0U)
#define NI_TOWER_APU_ID_0_MSK                  (0xFFUL << NI_TOWER_APU_ID_0_POS)
#define NI_TOWER_APU_ID_0                      NI_TOWER_APU_ID_0_MSK
#define NI_TOWER_APU_PERM_0_POS                (8U)
#define NI_TOWER_APU_PERM_0_MSK                (0xFFUL << NI_TOWER_APU_PERM_0_POS)
#define NI_TOWER_APU_PERM_0                    NI_TOWER_APU_PERM_0_MSK
#define NI_TOWER_APU_ID_1_POS                  (16U)
#define NI_TOWER_APU_ID_1_MSK                  (0xFFUL << NI_TOWER_APU_ID_1_POS)
#define NI_TOWER_APU_ID_1                      NI_TOWER_APU_ID_1_MSK
#define NI_TOWER_APU_PERM_1_POS                (24U)
#define NI_TOWER_APU_PERM_1_MSK                (0xFFUL << NI_TOWER_APU_PERM_1_POS)
#define NI_TOWER_APU_PERM_1                    NI_TOWER_APU_PERM_1_MSK

/*  Field definitions for prid_high register */
#define NI_TOWER_APU_ID_2_POS                  (0U)
#define NI_TOWER_APU_ID_2_MSK                  (0xFFUL << NI_TOWER_APU_ID_2_POS)
#define NI_TOWER_APU_ID_2                      NI_TOWER_APU_ID_2_MSK
#define NI_TOWER_APU_PERM_2_POS                (8U)
#define NI_TOWER_APU_PERM_2_MSK                (0xFFUL << NI_TOWER_APU_PERM_2_POS)
#define NI_TOWER_APU_PERM_2                    NI_TOWER_APU_PERM_2_MSK
#define NI_TOWER_APU_ID_3_POS                  (16U)
#define NI_TOWER_APU_ID_3_MSK                  (0xFFUL << NI_TOWER_APU_ID_3_POS)
#define NI_TOWER_APU_ID_3                      NI_TOWER_APU_ID_3_MSK
#define NI_TOWER_APU_PERM_3_POS                (24U)
#define NI_TOWER_APU_PERM_3_MSK                (0xFFUL << NI_TOWER_APU_PERM_3_POS)
#define NI_TOWER_APU_PERM_3                    NI_TOWER_APU_PERM_3_MSK

/*  Field definitions for apu_ctlr register */
#define NI_TOWER_APU_CTLR_APU_ENABLE_POS       (0U)
#define NI_TOWER_APU_CTLR_APU_ENABLE_MSK       (0x1UL << NI_TOWER_APU_CTLR_APU_ENABLE_POS)
#define NI_TOWER_APU_CTLR_APU_ENABLE           NI_TOWER_APU_CTLR_APU_ENABLE_MSK
#define NI_TOWER_APU_CTLR_FMU_ERROR_EN_POS     (1U)
#define NI_TOWER_APU_CTLR_FMU_ERROR_EN_MSK     (0x1UL << NI_TOWER_APU_CTLR_FMU_ERROR_EN_POS)
#define NI_TOWER_APU_CTLR_FMU_ERROR_EN         NI_TOWER_APU_CTLR_FMU_ERROR_EN_MSK
#define NI_TOWER_APU_CTLR_SYNC_ERROR_EN_POS    (2U)
#define NI_TOWER_APU_CTLR_SYNC_ERROR_EN_MSK    (0x1UL << NI_TOWER_APU_CTLR_SYNC_ERROR_EN_POS)
#define NI_TOWER_APU_CTLR_SYNC_ERROR_EN        NI_TOWER_APU_CTLR_SYNC_ERROR_EN_MSK
#define NI_TOWER_APU_CTLR_APU_REGION_4K_POS    (3U)
#define NI_TOWER_APU_CTLR_APU_REGION_4K_MSK    (0x1UL << NI_TOWER_APU_CTLR_APU_REGION_4K_POS)
#define NI_TOWER_APU_CTLR_APU_REGION_4K        NI_TOWER_APU_CTLR_APU_REGION_4K_MSK

#endif /* __NI_TOWER_APU_REG_H__ */
