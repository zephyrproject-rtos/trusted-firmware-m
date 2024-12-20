/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NOC_S3_PSAM_REG_H__
#define __NOC_S3_PSAM_REG_H__

#include "tfm_hal_device_header.h"

#include <stdint.h>

#define NOC_S3_MAX_NH_REGIONS     128

/**
 * \brief NoC S3 PSAM register map
 */
__PACKED_STRUCT noc_s3_psam_reg_map {
    __IM  uint32_t sam_unit_info;
    __IOM uint32_t sam_status;
    const uint32_t reserved_0[2];
    __IOM uint32_t htg_addr_mask_l;
    __IOM uint32_t htg_addr_mask_u;
    __IOM uint32_t axid_mask;
    const uint32_t reserved_1;
    __IOM uint32_t cmp_addr_mask_l;
    __IOM uint32_t cmp_addr_mask_u;
    const uint32_t reserved_2[2];
    __IOM uint32_t generic_config_reg0;
    __IOM uint32_t generic_config_reg1;
    const uint32_t reserved_3[50];
    __PACKED_STRUCT {
        __IOM uint32_t cfg0;
        __IOM uint32_t cfg1;
        __IOM uint32_t cfg2;
        __IOM uint32_t cfg3;
    } nh_region[NOC_S3_MAX_NH_REGIONS];
    __PACKED_STRUCT {
        __IOM uint32_t cfg0;
        __IOM uint32_t cfg1;
        __IOM uint32_t cfg2;
        __IOM uint32_t cfg3;
    } htg_region[32];
    __IOM uint32_t htg_tgtid_cfg[32];
    __IOM uint32_t np2_top_addr_cfg[32];
};

/*  Field definitions for sam_status register */
#define NOC_S3_SAM_STATUS_SETUP_COMPLETE_POS  (0U)
#define NOC_S3_SAM_STATUS_SETUP_COMPLETE_MSK  (0x1UL << NOC_S3_SAM_STATUS_SETUP_COMPLETE_POS)
#define NOC_S3_SAM_STATUS_SETUP_COMPLETE      NOC_S3_SAM_STATUS_SETUP_COMPLETE_MSK

/*  Field definitions for nh_region_cfg0 register */
#define NOC_S3_NH_REGION_REGION_VALID_POS     (0U)
#define NOC_S3_NH_REGION_REGION_VALID_MSK     (0x1UL << NOC_S3_NH_REGION_REGION_VALID_POS)
#define NOC_S3_NH_REGION_REGION_VALID         NOC_S3_NH_REGION_REGION_VALID_MSK

/*  Field definitions for nh_region_cfg2 register */
#define NOC_S3_NH_REGION_TGT_ID_POS           (0U)
#define NOC_S3_NH_REGION_TGT_ID_MSK           (0x7FUL << NOC_S3_NH_REGION_TGT_ID_POS)
#define NOC_S3_NH_REGION_TGT_ID               NOC_S3_NH_REGION_TGT_ID_MSK

#endif /* __NOC_S3_PSAM_REG_H__ */
