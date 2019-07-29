/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __OS_WRAPPER_SEMAPHORE_H__
#define __OS_WRAPPER_SEMAPHORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

/**
 * \brief Creates a new semaphore
 *
 * \param[in] max_count       Highest count of the semaphore
 * \param[in] initial_count   Starting count of the semaphore
 * \param[in] name            Name of the semaphore
 *
 * \return Returns ID of the semaphore created, or \ref OS_WRAPPER_ERROR in case
 *         of error
 */
uint32_t os_wrapper_semaphore_create(uint32_t max_count, uint32_t initial_count,
                                     const char *name);

/**
 * \brief Acquires the semaphore
 *
 * \param[in] semaphore_id Semaphore ID
 * \param[in] timeout      Timeout value
 *
 * \return \ref OS_WRAPPER_SUCCESS in case of successful acquision, or
 *         \ref OS_WRAPPER_ERROR in case of error
 */
uint32_t os_wrapper_semaphore_acquire(uint32_t semaphore_id, uint32_t timeout);

/**
 * \brief Releases the semaphore
 *
 * \param[in] semaphore_id Semaphore ID
 *
 * \return \ref OS_WRAPPER_SUCCESS in case of successful release, or
 *         \ref OS_WRAPPER_ERROR in case of error
 */
uint32_t os_wrapper_semaphore_release(uint32_t semaphore_id);

/**
 * \brief Deletes the semaphore
 *
 * \param[in] semaphore_id Semaphore ID
 *
 * \return \ref OS_WRAPPER_SUCCESS in case of successful release, or
 *         \ref OS_WRAPPER_ERROR in case of error
 */
uint32_t os_wrapper_semaphore_delete(uint32_t semaphore_id);

#ifdef __cplusplus
}
#endif

#endif /* __OS_WRAPPER_SEMAPHORE_H__ */
