/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SPM_API_H__
#define __SPM_API_H__

/* This file contains the apis exported by the SPM to tfm core */
#include "service_defs.h"
#include "secure_fw/core/tfm_secure_api.h"

enum spm_err_t {
    SPM_ERR_OK = 0,
    SPM_ERR_SERV_DB_NOT_INIT,
    SPM_ERR_SERV_ALREADY_ACTIVE,
    SPM_ERR_SERV_NOT_AVAILABLE,
    SPM_ERR_INVALID_CONFIG,
};

/**
 * \brief Configure isolated sandbox for a service
 *
 * \param[in] service_id     Service id
 *
 * \return Error code \ref spm_err_t
 *
 * \note This function doesn't check if service_id is valid.
 */
enum spm_err_t tfm_spm_service_sandbox_config(uint32_t service_id);

/**
 * \brief Deconfigure sandbox for a service
 *
 * \param[in] service_id     Service id
 *
 * \return Error code \ref spm_err_t
 *
 * \note This function doesn't check if service_id is valid.
 */
enum spm_err_t tfm_spm_service_sandbox_deconfig(uint32_t service_id);

/**
 * \brief Get saved stack pointer for a partition
 *
 * \param[in] service_id     Service id
 *
 * \return Stack pointer value
 *
 * \note This function doesn't check if service_id is valid.
 */
uint32_t tfm_spm_service_get_stack(uint32_t service_id);

/**
 * \brief Get bottom of stack region for a service
 *
 * \param[in] service_id     Service id
 *
 * \return Stack region bottom value
 *
 * \note This function doesn't check if service_id is valid.
 */
uint32_t tfm_spm_service_get_stack_bottom(uint32_t service_id);

/**
 * \brief Get top of stack region for a service
 *
 * \param[in] service_id     Service id
 *
 * \return Stack region top value
 *
 * \note This function doesn't check if service_id is valid.
 */
uint32_t tfm_spm_service_get_stack_top(uint32_t service_id);

/**
 * \brief Save stack pointer for service in database
 *
 * \param[in] service_id     Service id
 * \param[in] stack_ptr      Stack pointer to be stored
 *
 * \note This function doesn't check if service_id is valid.
 */
void tfm_spm_service_set_stack(uint32_t service_id, uint32_t stack_ptr);

/**
 * \brief Initialize service database
 *
 * \return Error code \ref spm_err_t
 */
enum spm_err_t tfm_spm_db_init(void);

/**
 * \brief Apply default MPU configuration for execution
 *
 * \return Error code \ref spm_err_t
 */
enum spm_err_t tfm_spm_mpu_init(void);

/**
 * \brief Execute service init function
 *
 * \return Error code \ref spm_err_t
 */
enum spm_err_t tfm_spm_service_init(void);

/**
 * \brief Set share region to which the service needs access
 *
 * \param[in] share     Share region id \ref tfm_buffer_share_region_e
 *
 * \return Error code \ref spm_err_t
 *
 * \note This function doesn't check if service_id is valid.
 */
enum spm_err_t tfm_spm_set_share_region(enum tfm_buffer_share_region_e share);

#endif /*__SPM_API_H__ */
