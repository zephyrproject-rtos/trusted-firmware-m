/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SMMU_V3_DRV_H__
#define __SMMU_V3_DRV_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief SMMU error enumeration types
 */
enum smmu_error_t {
    /* No Error */
    SMMU_ERR_NONE,
    /* Invalid parameter */
    SMMU_ERR_INVALID_PARAM,
    /* Error accessing smmu */
    SMMU_ERR_ACCESS,
    /* General error with driver */
    SMMU_ERR_GENERAL,
    /* Timeout waiting for smmu */
    SMMU_ERR_TIMEOUT,
};

/**
 * \brief SMMU device structure
 */
struct smmu_dev_t {
    /* Base address of the SMMU */
    const uintptr_t smmu_base;
    /* Approximate number of cycles to wait for ack from SMMU  */
    const uint64_t ack_timeout;
};

#define SMMU_DEFAULT_ACK_TIMEOUT 1000UL

/**
 * \brief Enable granule protection checks (GPC)
 *
 * \param[in] dev                SMMU device struct \ref smmu_dev_t
 *
 * \return Returns error code as specified in \ref smmu_error_t
 */
enum smmu_error_t smmu_gpc_enable(struct smmu_dev_t *dev);

/**
 * \brief Disable granule protection checks (GPC)
 *
 * \param[in] dev                SMMU device struct \ref smmu_dev_t
 *
 * \return Returns error code as specified in \ref smmu_error_t
 */
enum smmu_error_t smmu_gpc_disable(struct smmu_dev_t *dev);

/**
 * \brief Enable smmu and client accesses
 *
 * \param[in] dev                SMMU device struct \ref smmu_dev_t
 *
 * \return Returns error code as specified in \ref smmu_error_t
 */
enum smmu_error_t smmu_access_enable(struct smmu_dev_t *dev);

/**
 * \brief Disable smmu and client accesses
 *
 * \param[in] dev                SMMU device struct \ref smmu_dev_t
 *
 * \return Returns error code as specified in \ref smmu_error_t
 */
enum smmu_error_t smmu_access_disable(struct smmu_dev_t *dev);

#ifdef __cplusplus
}
#endif
#endif /* __SMMU_V3_DRV_H__ */
