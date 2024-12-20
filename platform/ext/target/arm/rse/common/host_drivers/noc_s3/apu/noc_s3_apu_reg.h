/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NOC_S3_APU_REG_H__
#define __NOC_S3_APU_REG_H__

#include "tfm_hal_device_header.h"

#include <stdint.h>

#define NOC_S3_MAX_APU_REGIONS    32

/**
 * \brief NoC S3 APU register map
 */
__PACKED_STRUCT noc_s3_apu_reg_map {
    __PACKED_STRUCT {
        __IOM uint32_t prbar_low;
        __IOM uint32_t prbar_high;
        __IOM uint32_t prlar_low;
        __IOM uint32_t prlar_high;
        __IOM uint32_t prid_low;
        __IOM uint32_t prid_high;
        const uint32_t reserved_2[2];
    } region[NOC_S3_MAX_APU_REGIONS];
    const uint32_t reserved_3[766];
    __IOM uint32_t apu_ctlr;
    __IM  uint32_t apu_iidr;
};

/*  Field definitions for prbar_low register */
#define NOC_S3_APU_REGION_ENABLE_POS         (0U)
#define NOC_S3_APU_REGION_ENABLE_MSK         (0x1UL << NOC_S3_APU_REGION_ENABLE_POS)
#define NOC_S3_APU_REGION_ENABLE             NOC_S3_APU_REGION_ENABLE_MSK
#define NOC_S3_APU_BR_POS                    (1U)
#define NOC_S3_APU_BR_MSK                    (0x1UL << NOC_S3_APU_BR_POS)
#define NOC_S3_APU_BR                        NOC_S3_APU_BR_MSK
#define NOC_S3_APU_LOCK_POS                  (2U)
#define NOC_S3_APU_LOCK_MSK                  (0x1UL << NOC_S3_APU_LOCK_POS)
#define NOC_S3_APU_LOCK                      NOC_S3_APU_LOCK_MSK

/*  Field definitions for prlar_low register */
#define NOC_S3_APU_ID_VALID_POS              (0U)
#define NOC_S3_APU_ID_VALID_MSK              (0xFUL << NOC_S3_APU_ID_VALID_POS)
#define NOC_S3_APU_ID_VALID                  NOC_S3_APU_ID_VALID_MSK

/*  Field definitions for prid_low register */
#define NOC_S3_APU_ID_0_POS                  (0U)
#define NOC_S3_APU_ID_0_MSK                  (0xFFUL << NOC_S3_APU_ID_0_POS)
#define NOC_S3_APU_ID_0                      NOC_S3_APU_ID_0_MSK
#define NOC_S3_APU_PERM_0_POS                (8U)
#define NOC_S3_APU_PERM_0_MSK                (0xFFUL << NOC_S3_APU_PERM_0_POS)
#define NOC_S3_APU_PERM_0                    NOC_S3_APU_PERM_0_MSK
#define NOC_S3_APU_ID_1_POS                  (16U)
#define NOC_S3_APU_ID_1_MSK                  (0xFFUL << NOC_S3_APU_ID_1_POS)
#define NOC_S3_APU_ID_1                      NOC_S3_APU_ID_1_MSK
#define NOC_S3_APU_PERM_1_POS                (24U)
#define NOC_S3_APU_PERM_1_MSK                (0xFFUL << NOC_S3_APU_PERM_1_POS)
#define NOC_S3_APU_PERM_1                    NOC_S3_APU_PERM_1_MSK

/*  Field definitions for prid_high register */
#define NOC_S3_APU_ID_2_POS                  (0U)
#define NOC_S3_APU_ID_2_MSK                  (0xFFUL << NOC_S3_APU_ID_2_POS)
#define NOC_S3_APU_ID_2                      NOC_S3_APU_ID_2_MSK
#define NOC_S3_APU_PERM_2_POS                (8U)
#define NOC_S3_APU_PERM_2_MSK                (0xFFUL << NOC_S3_APU_PERM_2_POS)
#define NOC_S3_APU_PERM_2                    NOC_S3_APU_PERM_2_MSK
#define NOC_S3_APU_ID_3_POS                  (16U)
#define NOC_S3_APU_ID_3_MSK                  (0xFFUL << NOC_S3_APU_ID_3_POS)
#define NOC_S3_APU_ID_3                      NOC_S3_APU_ID_3_MSK
#define NOC_S3_APU_PERM_3_POS                (24U)
#define NOC_S3_APU_PERM_3_MSK                (0xFFUL << NOC_S3_APU_PERM_3_POS)
#define NOC_S3_APU_PERM_3                    NOC_S3_APU_PERM_3_MSK

/*  Field definitions for apu_ctlr register */
#define NOC_S3_APU_CTLR_APU_ENABLE_POS       (0U)
#define NOC_S3_APU_CTLR_APU_ENABLE_MSK       (0x1UL << NOC_S3_APU_CTLR_APU_ENABLE_POS)
#define NOC_S3_APU_CTLR_APU_ENABLE           NOC_S3_APU_CTLR_APU_ENABLE_MSK
#define NOC_S3_APU_CTLR_FMU_ERROR_EN_POS     (1U)
#define NOC_S3_APU_CTLR_FMU_ERROR_EN_MSK     (0x1UL << NOC_S3_APU_CTLR_FMU_ERROR_EN_POS)
#define NOC_S3_APU_CTLR_FMU_ERROR_EN         NOC_S3_APU_CTLR_FMU_ERROR_EN_MSK
#define NOC_S3_APU_CTLR_SYNC_ERROR_EN_POS    (2U)
#define NOC_S3_APU_CTLR_SYNC_ERROR_EN_MSK    (0x1UL << NOC_S3_APU_CTLR_SYNC_ERROR_EN_POS)
#define NOC_S3_APU_CTLR_SYNC_ERROR_EN        NOC_S3_APU_CTLR_SYNC_ERROR_EN_MSK
#define NOC_S3_APU_CTLR_APU_REGION_4K_POS    (3U)
#define NOC_S3_APU_CTLR_APU_REGION_4K_MSK    (0x1UL << NOC_S3_APU_CTLR_APU_REGION_4K_POS)
#define NOC_S3_APU_CTLR_APU_REGION_4K        NOC_S3_APU_CTLR_APU_REGION_4K_MSK

#endif /* __NOC_S3_APU_REG_H__ */
