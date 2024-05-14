/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_PSAM_DRV_H__
#define __NI_TOWER_PSAM_DRV_H__

#include "ni_tower_drv.h"

#include <stdint.h>

/**
 * \brief NI-Tower PSAM region configuration info structure
 */
struct ni_tower_psam_reg_cfg_info {
    /* Base address of the region */
    uint64_t base_addr;
    /* End address of the region */
    uint64_t end_addr;
    /* ID of the target interface or the strip group for non-striped region.*/
    uint64_t target_id;
};

/**
 * \brief NI-Tower PSAM device structure
 */
struct ni_tower_psam_dev {
    uintptr_t base;
    /* Offset to be added to the memory map base and end addresses */
    uint64_t region_mapping_offset;
};

/**
 * \brief Initialize and return PSAM device
 *
 * \param[in]  ni_tower_dev          NI-Tower device struct \ref ni_tower_dev.
 * \param[in]  component             NI-Tower component node struct \ref
 *                                   ni_tower_component_node.
 * \param[in]  region_mapping_offset Offset which will added to the memory map
 *                                   base and end addresses.
 * \param[out] dev                   NI-Tower PSAM device struct \ref
 *                                   ni_tower_psam_dev
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_psam_dev_init(
    const struct ni_tower_dev *ni_tower_dev,
    const struct ni_tower_component_node* component,
    const uint64_t region_mapping_offset,
    struct ni_tower_psam_dev *dev);

/**
 * \brief Configure non-striped (non-hashed) region.
 *
 * \param[in] dev         NI-Tower PSAM device struct \ref ni_tower_psam_dev.
 * \param[in] cfg_info    Configuration info of a region to be configured.
 *                        struct \ref ni_tower_psam_reg_cfg_info.
 * \param[in] region      PSAM region number to be initialized.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_psam_configure_nhregion(
    const struct ni_tower_psam_dev *dev,
    const struct ni_tower_psam_reg_cfg_info *cfg_info,
    const uint32_t region);

/**
 * \brief Get the next available region number and configure the non-striped
 *        (non-hashed) region.
 *
 * \param[in] dev         NI-Tower PSAM device struct \ref ni_tower_psam_dev.
 * \param[in] cfg_info    Configuration info of a region to be configured.
 *                        struct \ref ni_tower_psam_reg_cfg_info.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_psam_configure_next_available_nhregion(
    const struct ni_tower_psam_dev *dev,
    const struct ni_tower_psam_reg_cfg_info *cfg_info);

/**
 * \brief Enables the PSAM device
 *
 * \param[in] dev         NI-Tower PSAM device struct \ref ni_tower_psam_dev.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_psam_enable(const struct ni_tower_psam_dev *dev);

/**
 * \brief Disables the PSAM device
 *
 * \param[in] dev         NI-Tower PSAM device struct \ref ni_tower_psam_dev.
 *
 * \return Returns error code as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_psam_disable(const struct ni_tower_psam_dev *dev);

#endif /* __NI_TOWER_PSAM_DRV_H__ */
