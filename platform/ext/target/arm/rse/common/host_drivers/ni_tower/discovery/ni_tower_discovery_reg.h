/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_DISCOVERY_REG_H__
#define __NI_TOWER_DISCOVERY_REG_H__

#include "tfm_hal_device_header.h"

#include <stdint.h>

/**
 * \brief Offset address to be tested for every config read to determine
 * if a configuration is mapped to FMU register
 */
#define FMU_CHECK_OFFSET_ADDRESS    (0xFFE0)

/**
 * \brief Interconnect Part Number for NI-Tower
 */
#define NI_TOWER_PID0_PART_NUM      (0x3F)
#define NI_TOWER_PID1_PART_NUM      (0x4)

/**
 * \brief NI-Tower Domain top registers. These are common for all domains
 */
struct ni_tower_domain_cfg_hdr {
    __IM uint32_t node_type;
    __IM uint32_t child_node_info;
    __IM uint32_t x_pointers[];
};

/*  Field definitions for node_type register */
#define NI_TOWER_DOMAIN_NODE_TYPE_POS     (0U)
#define NI_TOWER_DOMAIN_NODE_TYPE_MSK     (0xFFFFUL << NI_TOWER_DOMAIN_NODE_TYPE_POS)
#define NI_TOWER_DOMAIN_NODE_TYPE         NI_TOWER_DOMAIN_NODE_TYPE_MSK
#define NI_TOWER_DOMAIN_NODE_ID_POS       (16U)
#define NI_TOWER_DOMAIN_NODE_ID_MSK       (0xFFFFUL << NI_TOWER_DOMAIN_NODE_ID_POS)
#define NI_TOWER_DOMAIN_NODE_ID           NI_TOWER_DOMAIN_NODE_ID_MSK

/**
 * \brief NI-Tower Component top registers. These are common for all components
 */
struct ni_tower_component_cfg_hdr {
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
 * \brief NI-Tower Global register
 */
struct ni_tower_global_reg_map {
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
#define NI_TOWER_GLOBAL_PERIPHERAL_ID0_POS         (0U)
#define NI_TOWER_GLOBAL_PERIPHERAL_ID0_MSK         (0xFFUL << NI_TOWER_GLOBAL_PERIPHERAL_ID0_POS)
#define NI_TOWER_GLOBAL_PERIPHERAL_ID0             NI_TOWER_GLOBAL_PERIPHERAL_ID0_MSK

/*  Field definitions for peripheral_id1 register */
#define NI_TOWER_GLOBAL_PERIPHERAL_ID1_POS         (0U)
#define NI_TOWER_GLOBAL_PERIPHERAL_ID1_MSK         (0xFUL << NI_TOWER_GLOBAL_PERIPHERAL_ID1_POS)
#define NI_TOWER_GLOBAL_PERIPHERAL_ID1             NI_TOWER_GLOBAL_PERIPHERAL_ID1_MSK

#endif /* __NI_TOWER_DISCOVERY_REG_H__ */
