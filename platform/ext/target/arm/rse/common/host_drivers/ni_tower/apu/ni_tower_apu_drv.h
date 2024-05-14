/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_APU_DRV_H__
#define __NI_TOWER_APU_DRV_H__

#include "ni_tower_drv.h"

#include <stdint.h>

/* Number of possible apu entities in NI-Tower */
#define NI_TOWER_APU_NUM_ENTITIES 4

/**
 * \brief NI-Tower APU background type enumerations
 */
enum ni_tower_apu_br_type {
    NI_T_FOREGROUND = 0x0,
    NI_T_BACKGROUND = 0x1
};

/**
 * \brief NI-Tower APU access permission type enumerations
 */
enum ni_tower_apu_access_perm_type {
    /* As per spec */
    NI_T_N_SEC_W = 0b00000001,
    NI_T_SEC_W   = 0b00000010,
    NI_T_N_SEC_R = 0b00000100,
    NI_T_SEC_R   = 0b00001000,
    NI_T_REALM_W = 0b00010000,
    NI_T_ROOT_W  = 0b00100000,
    NI_T_REALM_R = 0b01000000,
    NI_T_ROOT_R  = 0b10000000,

    NI_T_N_SEC_RW = NI_T_N_SEC_R | NI_T_N_SEC_W,
    NI_T_SEC_RW   = NI_T_SEC_R | NI_T_SEC_W,
    NI_T_REALM_RW = NI_T_REALM_R | NI_T_REALM_W,
    NI_T_ROOT_RW  = NI_T_ROOT_R | NI_T_ROOT_W,

    NI_T_ALL_PERM = NI_T_N_SEC_RW | NI_T_SEC_RW | NI_T_REALM_RW | NI_T_ROOT_RW
};

/**
 * \brief NI-Tower APU entity selection type enumerations
 */
enum ni_tower_apu_entity_type {
    NI_T_ID_0_SELECT = 0b0001,
    NI_T_ID_1_SELECT = 0b0010,
    NI_T_ID_2_SELECT = 0b0100,
    NI_T_ID_3_SELECT = 0b1000,
};

/**
 * \brief NI-Tower APU entity valid type enumerations
 */
enum ni_tower_apu_entity_valid_type {
    NI_T_ID_VALID_NONE = 0x0,

    NI_T_ID_0_VALID = 0b0001,
    NI_T_ID_1_VALID = 0b0010,
    NI_T_ID_2_VALID = 0b0100,
    NI_T_ID_3_VALID = 0b1000,

    NI_T_ID_VALID_ALL = NI_T_ID_0_VALID | NI_T_ID_1_VALID | NI_T_ID_2_VALID |
                        NI_T_ID_3_VALID,
};

/**
 * \brief NI-Tower APU lock type enumerations
 */
enum ni_tower_apu_lock_type {
    NI_T_UNLOCK = 0x0,
    NI_T_LOCK = 0x1,
};

/**
 * \brief NI-Tower APU region enable type enumerations
 */
enum ni_tower_apu_region_enable_type {
    NI_T_REGION_DISABLE = 0x0,
    NI_T_REGION_ENABLE = 0x1,
};

/**
 * \brief NI-Tower APU region configuration info structure
 */
struct ni_tower_apu_reg_cfg_info {
    /* Base address of the region */
    uint64_t base_addr;
    /* End address of the region */
    uint64_t end_addr;
    /* Whether the region is background or a foreground region*/
    enum ni_tower_apu_br_type background;
    /* Array of access permissions for all entities */
    enum ni_tower_apu_access_perm_type permissions[NI_TOWER_APU_NUM_ENTITIES];
    /* Array of entity ids */
    uint8_t entity_ids[NI_TOWER_APU_NUM_ENTITIES];
    /* Value for id_valid */
    enum ni_tower_apu_entity_valid_type id_valid;
    /* Whether the region should be enabled or not */
    enum ni_tower_apu_region_enable_type region_enable;
    /* Whether the region should be locked or not */
    enum ni_tower_apu_lock_type lock;
};

/**
 * \brief NI-Tower APU device structure
 */
struct ni_tower_apu_dev {
    uintptr_t base;
    /* Offset to be added to the memory map base and end addresses */
    uint64_t region_mapping_offset;
};

/**
 * \brief Initialize and return APU device
 *
 * \param[in]  ni_tower_dev          NI-Tower device struct \ref ni_tower_dev.
 * \param[in]  component             NI-Tower component node struct \ref
 *                                   ni_tower_component_node.
 * \param[in]  region_mapping_offset Offset which will added to the memory map
 *                                   base and end addresses.
 * \param[out] dev                   NI-Tower APU device struct \ref
 *                                   ni_tower_apu_dev.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_apu_dev_init(
    const struct ni_tower_dev *ni_tower_dev,
    const struct ni_tower_component_node* component,
    const uint64_t region_mapping_offset,
    struct ni_tower_apu_dev *dev);

/**
 * \brief Configure NI-Tower APU
 *
 * \param[in] dev           NI-Tower APU device struct \ref ni_tower_apu_dev.
 * \param[in] cfg_info      Configuration info of a region to be configured
 *                          struct \ref ni_tower_apu_reg_cfg_info.
 * \param[in] region        Region number to be configured.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_apu_configure_region(
    const struct ni_tower_apu_dev *dev,
    const struct ni_tower_apu_reg_cfg_info *cfg_info,
    const uint32_t region);

/**
 * \brief Get the next available region number and configure NI-Tower APU
 *        region
 *
 * \param[in] dev           NI-Tower APU device struct \ref ni_tower_apu_dev.
 * \param[in] cfg_info      Configuration info of the region to be configured
 *                          struct \ref ni_tower_apu_reg_cfg_info.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_apu_configure_next_available_region(
    const struct ni_tower_apu_dev *dev,
    const struct ni_tower_apu_reg_cfg_info *cfg_info);

/**
 * \brief Enables NI-Tower APU
 *
 * \param[in] dev           NI-Tower APU device struct \ref ni_tower_apu_dev.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_apu_enable(const struct ni_tower_apu_dev *dev);

/**
 * \brief Enables NI-Tower APU SLVERR response.
 *
 * \param[in] dev           NI-Tower APU device struct \ref ni_tower_apu_dev.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_apu_sync_err_enable(
    const struct ni_tower_apu_dev *dev);

#endif /* __NI_TOWER_APU_DRV_H__ */
