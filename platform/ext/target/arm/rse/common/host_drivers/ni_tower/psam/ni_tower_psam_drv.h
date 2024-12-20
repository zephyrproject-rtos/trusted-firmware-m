/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NOC_S3_PSAM_DRV_H__
#define __NOC_S3_PSAM_DRV_H__

#include "ni_tower_drv.h"

#include <stdint.h>

/**
 * \brief NoC S3 PSAM region configuration info structure
 */
struct noc_s3_psam_reg_cfg_info {
    /* Base address of the region */
    uint64_t base_addr;
    /* End address of the region */
    uint64_t end_addr;
    /* ID of the target interface or the strip group for non-striped region.*/
    uint64_t target_id;
};

/**
 * \brief NoC S3 PSAM device structure
 */
struct noc_s3_psam_dev {
    uintptr_t base;
    /* Offset to be added to the memory map base and end addresses */
    uint64_t region_mapping_offset;
#ifdef NOC_S3_PRETTY_PRINT_LOG_ENABLED
    /* Device data */
    struct noc_s3_dev_data data;
#endif
};

/**
 * \brief Initialize and return PSAM device
 *
 * \param[in]  noc_s3_dev            NoC S3 device struct \ref noc_s3_dev.
 * \param[in]  component             NoC S3 component node struct \ref
 *                                   noc_s3_component_node.
 * \param[in]  region_mapping_offset Offset which will added to the memory map
 *                                   base and end addresses.
 * \param[out] dev                   NoC S3 PSAM device struct \ref
 *                                   noc_s3_psam_dev
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_psam_dev_init(
    const struct noc_s3_dev *noc_s3_dev,
    const struct noc_s3_component_node* component,
    const uint64_t region_mapping_offset,
    struct noc_s3_psam_dev *dev);

/**
 * \brief Configure non-striped (non-hashed) region.
 *
 * \param[in] dev         NoC S3 PSAM device struct \ref noc_s3_psam_dev.
 * \param[in] cfg_info    Configuration info of a region to be configured.
 *                        struct \ref noc_s3_psam_reg_cfg_info.
 * \param[in] region      PSAM region number to be initialized.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_psam_configure_nhregion(
    const struct noc_s3_psam_dev *dev,
    const struct noc_s3_psam_reg_cfg_info *cfg_info,
    const uint32_t region);

/**
 * \brief Get the next available region number and configure the non-striped
 *        (non-hashed) region.
 *
 * \param[in] dev         NoC S3 PSAM device struct \ref noc_s3_psam_dev.
 * \param[in] cfg_info    Configuration info of a region to be configured.
 *                        struct \ref noc_s3_psam_reg_cfg_info.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_psam_configure_next_available_nhregion(
    const struct noc_s3_psam_dev *dev,
    const struct noc_s3_psam_reg_cfg_info *cfg_info);

/**
 * \brief Enables the PSAM device
 *
 * \param[in] dev         NoC S3 PSAM device struct \ref noc_s3_psam_dev.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_psam_enable(const struct noc_s3_psam_dev *dev);

/**
 * \brief Disables the PSAM device
 *
 * \param[in] dev         NoC S3 PSAM device struct \ref noc_s3_psam_dev.
 *
 * \return Returns error code as specified in \ref noc_s3_err
 */
enum noc_s3_err noc_s3_psam_disable(const struct noc_s3_psam_dev *dev);

#endif /* __NOC_S3_PSAM_DRV_H__ */
