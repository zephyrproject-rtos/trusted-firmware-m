/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
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

enum spm_part_state_t {
    SPM_PART_STATE_UNINIT = 0,
    SPM_PART_STATE_IDLE,
    SPM_PART_STATE_RUNNING,
    SPM_PART_STATE_SUSPENDED,
    SPM_PART_STATE_BLOCKED,
    SPM_PART_STATE_CLOSED,
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
 * \brief Get the current state of a service
 *
 * \param[in] service_id     Service id
 *
 * \return The state of the specified service
 *
 * \note This function doesn't check if service_id is valid.
 * \note The returned value has the value set of \ref spm_part_state_t.
 */
uint32_t tfm_spm_service_get_state(uint32_t service_id);

/**
 * \brief Get the Id of the caller of the service given
 *
 * \param[in] service_id     Service id to get the caller of
 *
 * \return The Id of the caller service
 *
 * \note This function doesn't check if service_id is valid.
 */
uint32_t tfm_spm_service_get_caller_service_id(uint32_t service_id);

/**
 * \brief Get the original PSP of the service
 *
 * \param[in] service_id     Service id
 *
 * \return The original PSP of the service
 *
 * \note This function doesn't check if service_id is valid.
 */
uint32_t tfm_spm_service_get_orig_psp(uint32_t service_id);

/**
 * \brief Get the original PSP limit of the service
 *
 * \param[in] service_id     Service id
 *
 * \return The original PSP limit of the service
 *
 * \note This function doesn't check if service_id is valid.
 */
uint32_t tfm_spm_service_get_orig_psplim(uint32_t service_id);

/**
 * \brief Get the original link register value of the service
 *
 * \param[in] service_id     Service id
 *
 * \return The original link register value of the service
 *
 * \note This function doesn't check if service_id is valid.
 */
uint32_t tfm_spm_service_get_orig_lr(uint32_t service_id);

/**
 * \brief Get the buffer share region of the service
 *
 * \param[in] service_id     Service id
 *
 * \return The buffer share region of the service
 *
 * \note This function doesn't check if service_id is valid.
 * \note The returned value has the value set of \ref tfm_buffer_share_region_e
 */
uint32_t tfm_spm_service_get_share(uint32_t service_id);

/**
 * \brief Returns the id of the service that has running state
 *
 * \return The Id of the service with the running state, if there is any set.
 *         0 otherwise.
 */
uint32_t tfm_spm_service_get_running_service_id(void);

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
 * \brief Set the current state of a service
 *
 * \param[in] service_id     Service id
 * \param[in] state          The state to be set
 *
 * \note This function doesn't check if service_id is valid.
 * \note The \ref state has to have the value set of \ref spm_part_state_t.
 */
void tfm_spm_service_set_state(uint32_t service_id, uint32_t state);

/**
 * \brief Set the caller service Id for a given service
 *
 * \param[in] service_id         Service id
 * \param[in] caller_service_id  The Id of the caller service
 *
 * \note This function doesn't check if any of the service_ids is valid.
 */
void tfm_spm_service_set_caller_service_id(uint32_t service_id,
                                           uint32_t caller_service_id);

/**
 * \brief Set the original PSP value of a service
 *
 * \param[in] service_id     Service id
 * \param[in] orig_psp       The PSP value to set
 *
 * \note This function doesn't check if service_id is valid.
 */
void tfm_spm_service_set_orig_psp(uint32_t service_id, uint32_t orig_psp);

/**
 * \brief Set the original PSP limit value of a service
 *
 * \param[in] service_id     Service id
 * \param[in] orig_psplim    The PSP limit value to set
 *
 * \note This function doesn't check if service_id is valid.
 */
void tfm_spm_service_set_orig_psplim(uint32_t service_id, uint32_t orig_psplim);

/**
 * \brief Set the original link register value of a service
 *
 * \param[in] service_id     Service id
 * \param[in] orig_lr        The link register value to set
 *
 * \note This function doesn't check if service_id is valid.
 */
void tfm_spm_service_set_orig_lr(uint32_t service_id, uint32_t orig_lr);

/**
 * \brief Set the buffer share region of the service
 *
 * \param[in] service_id     Service id
 * \param[in] share          The buffer share region to be set
 *
 * \return Error code \ref spm_err_t
 *
 * \note This function doesn't check if service_id is valid.
 * \note share has to have the value set of \ref tfm_buffer_share_region_e
 */
enum spm_err_t tfm_spm_service_set_share(uint32_t service_id, uint32_t share);

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
 * \brief Clears the context info from the database for a service.
 *
 * \param[in] service_id     Service id
 *
 * \note This function doesn't check if service_id is valid.
 */
void tfm_spm_service_cleanup_context(uint32_t service_id);

#endif /*__SPM_API_H__ */
