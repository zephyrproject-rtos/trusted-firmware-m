/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NOC_S3_APU_DRV_H__
#define __NOC_S3_APU_DRV_H__

#include "ni_tower_drv.h"

#include <stdint.h>

/* Number of possible apu entities in NoC S3 */
#define NOC_S3_APU_NUM_ENTITIES 4

/**
 * \brief NoC S3 APU background type enumerations
 */
enum noc_s3_apu_br_type {
    NOC_S3_FOREGROUND = 0x0,
    NOC_S3_BACKGROUND = 0x1
};

/**
 * \brief NoC S3 APU access permission type enumerations
 */
enum noc_s3_apu_access_perm_type {
    /* As per spec */
    NOC_S3_N_SEC_W = 0b00000001,
    NOC_S3_SEC_W   = 0b00000010,
    NOC_S3_N_SEC_R = 0b00000100,
    NOC_S3_SEC_R   = 0b00001000,
    NOC_S3_REALM_W = 0b00010000,
    NOC_S3_ROOT_W  = 0b00100000,
    NOC_S3_REALM_R = 0b01000000,
    NOC_S3_ROOT_R  = 0b10000000,

    NOC_S3_N_SEC_RW = NOC_S3_N_SEC_R | NOC_S3_N_SEC_W,
    NOC_S3_SEC_RW   = NOC_S3_SEC_R | NOC_S3_SEC_W,
    NOC_S3_REALM_RW = NOC_S3_REALM_R | NOC_S3_REALM_W,
    NOC_S3_ROOT_RW  = NOC_S3_ROOT_R | NOC_S3_ROOT_W,

    NOC_S3_ALL_PERM = NOC_S3_N_SEC_RW | NOC_S3_SEC_RW | NOC_S3_REALM_RW | NOC_S3_ROOT_RW
};

/**
 * \brief NoC S3 APU entity selection type enumerations
 */
enum noc_s3_apu_entity_type {
    NOC_S3_ID_0_SELECT = 0b0001,
    NOC_S3_ID_1_SELECT = 0b0010,
    NOC_S3_ID_2_SELECT = 0b0100,
    NOC_S3_ID_3_SELECT = 0b1000,
};

/**
 * \brief NoC S3 APU entity valid type enumerations
 */
enum noc_s3_apu_entity_valid_type {
    NOC_S3_ID_VALID_NONE = 0x0,

    NOC_S3_ID_0_VALID = 0b0001,
    NOC_S3_ID_1_VALID = 0b0010,
    NOC_S3_ID_2_VALID = 0b0100,
    NOC_S3_ID_3_VALID = 0b1000,

    NOC_S3_ID_VALID_ALL = NOC_S3_ID_0_VALID | NOC_S3_ID_1_VALID |
                          NOC_S3_ID_2_VALID | NOC_S3_ID_3_VALID,
};

/**
 * \brief NoC S3 APU lock type enumerations
 */
enum noc_s3_apu_lock_type {
    NOC_S3_UNLOCK = 0x0,
    NOC_S3_LOCK = 0x1,
};

/**
 * \brief NoC S3 APU region enable type enumerations
 */
enum noc_s3_apu_region_enable_type {
    NOC_S3_REGION_DISABLE = 0x0,
    NOC_S3_REGION_ENABLE = 0x1,
};

/**
 * \brief NoC S3 APU region configuration info structure
 */
struct noc_s3_apu_reg_cfg_info {
    /* Base address of the region */
    uint64_t base_addr;
    /* End address of the region */
    uint64_t end_addr;
    /* Whether the region is background or a foreground region*/
    enum noc_s3_apu_br_type background;
    /* Array of access permissions for all entities */
    enum noc_s3_apu_access_perm_type permissions[NOC_S3_APU_NUM_ENTITIES];
    /* Array of entity ids */
    uint8_t entity_ids[NOC_S3_APU_NUM_ENTITIES];
    /* Value for id_valid */
    enum noc_s3_apu_entity_valid_type id_valid;
    /* Whether the region should be enabled or not */
    enum noc_s3_apu_region_enable_type region_enable;
    /* Whether the region should be locked or not */
    enum noc_s3_apu_lock_type lock;
};

/**
 * \brief NoC S3 APU device structure
 */
struct noc_s3_apu_dev {
    uintptr_t base;
    /* Offset to be added to the memory map base and end addresses */
    uint64_t region_mapping_offset;
#ifdef NOC_S3_PRETTY_PRINT_LOG_ENABLED
    /* Device data */
    struct noc_s3_dev_data data;
#endif
};

/**
 * \brief Initialize and return APU device
 *
 * \param[in]  noc_s3_dev            NoC S3 device struct \ref noc_s3_dev.
 * \param[in]  component             NoC S3 component node struct \ref
 *                                   noc_s3_component_node.
 * \param[in]  region_mapping_offset Offset which will added to the memory map
 *                                   base and end addresses.
 * \param[out] dev                   NoC S3 APU device struct \ref
 *                                   noc_s3_apu_dev.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_apu_dev_init(
    const struct noc_s3_dev *noc_s3_dev,
    const struct noc_s3_component_node* component,
    const uint64_t region_mapping_offset,
    struct noc_s3_apu_dev *dev);

/**
 * \brief Configure NoC S3 APU
 *
 * \param[in] dev           NoC S3 APU device struct \ref noc_s3_apu_dev.
 * \param[in] cfg_info      Configuration info of a region to be configured
 *                          struct \ref noc_s3_apu_reg_cfg_info.
 * \param[in] region        Region number to be configured.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_apu_configure_region(
    const struct noc_s3_apu_dev *dev,
    const struct noc_s3_apu_reg_cfg_info *cfg_info,
    const uint32_t region);

/**
 * \brief Get the next available region number and configure NoC S3 APU
 *        region
 *
 * \param[in] dev           NoC S3 APU device struct \ref noc_s3_apu_dev.
 * \param[in] cfg_info      Configuration info of the region to be configured
 *                          struct \ref noc_s3_apu_reg_cfg_info.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_apu_configure_next_available_region(
    const struct noc_s3_apu_dev *dev,
    const struct noc_s3_apu_reg_cfg_info *cfg_info);

/**
 * \brief Enables NoC S3 APU
 *
 * \param[in] dev           NoC S3 APU device struct \ref noc_s3_apu_dev.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_apu_enable(const struct noc_s3_apu_dev *dev);

/**
 * \brief Enables NoC S3 APU SLVERR response.
 *
 * \param[in] dev           NoC S3 APU device struct \ref noc_s3_apu_dev.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_apu_sync_err_enable(const struct noc_s3_apu_dev *dev);

#endif /* __NOC_S3_APU_DRV_H__ */
