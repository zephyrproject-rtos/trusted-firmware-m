/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NOC_S3_DISCOVERY_REG_H__
#define __NOC_S3_DISCOVERY_REG_H__

#include "tfm_hal_device_header.h"

#include <stdint.h>

/**
 * \brief Offset address to be tested for every config read to determine
 * if a configuration is mapped to FMU register
 */
#define FMU_CHECK_OFFSET_ADDRESS    (0xFFE0)

/**
 * \brief Interconnect Part Number for NoC S3
 */
#define NOC_S3_PID0_PART_NUM      (0x3F)
#define NOC_S3_PID1_PART_NUM      (0x4)

/**
 * \brief NoC S3 Domain top registers. These are common for all domains
 */
struct noc_s3_domain_cfg_hdr {
    __IM uint32_t node_type;
    __IM uint32_t child_node_info;
    __IM uint32_t x_pointers[];
};

/*  Field definitions for node_type register */
#define NOC_S3_DOMAIN_NODE_TYPE_POS     (0U)
#define NOC_S3_DOMAIN_NODE_TYPE_MSK     (0xFFFFUL << NOC_S3_DOMAIN_NODE_TYPE_POS)
#define NOC_S3_DOMAIN_NODE_TYPE         NOC_S3_DOMAIN_NODE_TYPE_MSK
#define NOC_S3_DOMAIN_NODE_ID_POS       (16U)
#define NOC_S3_DOMAIN_NODE_ID_MSK       (0xFFFFUL << NOC_S3_DOMAIN_NODE_ID_POS)
#define NOC_S3_DOMAIN_NODE_ID           NOC_S3_DOMAIN_NODE_ID_MSK

/**
 * \brief NoC S3 Component top registers. These are common for all components
 */
struct noc_s3_component_cfg_hdr {
    __IM  uint32_t node_type;
    const uint32_t reserved_0[4];
    __IM  uint32_t interface_id_0_3;
    const uint32_t reserved_1[58];
    __IM  uint32_t num_subfeatures;
    const uint32_t reserved_2;
    struct {
        __IM uint32_t type;
        __IM uint32_t pointer;
    } subfeature[];
};

/**
 * \brief NoC S3 Global register
 */
struct noc_s3_global_reg_map {
    __IM  uint32_t node_type;
    __IM  uint32_t child_node_info;
    __IM  uint32_t vd_pointers;
    const uint32_t reserved[1009];
    __IM  uint32_t peripheral_id4;
    __IM  uint32_t peripheral_id5;
    __IM  uint32_t peripheral_id6;
    __IM  uint32_t peripheral_id7;
    __IM  uint32_t peripheral_id0;
    __IM  uint32_t peripheral_id1;
    __IM  uint32_t peripheral_id2;
    __IM  uint32_t peripheral_id3;
    __IM  uint32_t component_id0;
    __IM  uint32_t component_id1;
    __IM  uint32_t component_id2;
    __IM  uint32_t component_id3;
};

/*  Field definitions for peripheral_id0 register */
#define NOC_S3_GLOBAL_PERIPHERAL_ID0_POS         (0U)
#define NOC_S3_GLOBAL_PERIPHERAL_ID0_MSK         (0xFFUL << NOC_S3_GLOBAL_PERIPHERAL_ID0_POS)
#define NOC_S3_GLOBAL_PERIPHERAL_ID0             NOC_S3_GLOBAL_PERIPHERAL_ID0_MSK

/*  Field definitions for peripheral_id1 register */
#define NOC_S3_GLOBAL_PERIPHERAL_ID1_POS         (0U)
#define NOC_S3_GLOBAL_PERIPHERAL_ID1_MSK         (0xFUL << NOC_S3_GLOBAL_PERIPHERAL_ID1_POS)
#define NOC_S3_GLOBAL_PERIPHERAL_ID1             NOC_S3_GLOBAL_PERIPHERAL_ID1_MSK

#endif /* __NOC_S3_DISCOVERY_REG_H__ */
