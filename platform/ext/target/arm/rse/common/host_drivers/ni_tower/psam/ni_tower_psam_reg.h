/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_PSAM_REG_H__
#define __NI_TOWER_PSAM_REG_H__

#include "tfm_hal_device_header.h"

#include <stdint.h>

#define NI_TOWER_MAX_NH_REGIONS     128

/**
 * \brief NI-Tower PSAM register map
 */
__PACKED_STRUCT ni_tower_psam_reg_map {
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
    } nh_region[NI_TOWER_MAX_NH_REGIONS];
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
#define NI_TOWER_SAM_STATUS_SETUP_COMPLETE_POS  (0U)
#define NI_TOWER_SAM_STATUS_SETUP_COMPLETE_MSK  (0x1UL << NI_TOWER_SAM_STATUS_SETUP_COMPLETE_POS)
#define NI_TOWER_SAM_STATUS_SETUP_COMPLETE      NI_TOWER_SAM_STATUS_SETUP_COMPLETE_MSK

/*  Field definitions for nh_region_cfg0 register */
#define NI_TOWER_NH_REGION_REGION_VALID_POS     (0U)
#define NI_TOWER_NH_REGION_REGION_VALID_MSK     (0x1UL << NI_TOWER_NH_REGION_REGION_VALID_POS)
#define NI_TOWER_NH_REGION_REGION_VALID         NI_TOWER_NH_REGION_REGION_VALID_MSK

/*  Field definitions for nh_region_cfg2 register */
#define NI_TOWER_NH_REGION_TGT_ID_POS           (0U)
#define NI_TOWER_NH_REGION_TGT_ID_MSK           (0x7FUL << NI_TOWER_NH_REGION_TGT_ID_POS)
#define NI_TOWER_NH_REGION_TGT_ID               NI_TOWER_NH_REGION_TGT_ID_MSK

#endif /* __NI_TOWER_PSAM_REG_H__ */
