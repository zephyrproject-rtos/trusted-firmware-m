/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __OS_WRAPPER_H__
#define __OS_WRAPPER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define OS_WRAPPER_SUCCESS            (0x0)
#define OS_WRAPPER_ERROR              (0xFFFFFFFFU)
#define OS_WRAPPER_WAIT_FOREVER       (0xFFFFFFFFU)
#define OS_WRAPPER_DEFAULT_STACK_SIZE (-1)

/* prototype for the thread entry function */
typedef void (*os_wrapper_thread_func) (void *argument);

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

/**
 * \brief Creates a mutex for mutual exclusion of resources
 *
 * \return The ID of the created mutex on success or \ref OS_WRAPPER_ERROR on
 *         error
 */
uint32_t os_wrapper_mutex_create(void);

/**
 * \brief Acquires a mutex that is created by \ref os_wrapper_mutex_create()
 *
 * \param[in] mutex_id The ID of the mutex to acquire. Should be one of the IDs
 *                     returned by \ref os_wrapper_mutex_create()
 * \param[in] timeout  The maximum amount of time(in tick periods) for the
 *                     thread to wait for the mutex to be available.
 *                     If timeout is zero, the function will return immediately.
 *                     Setting timeout to \ref OS_WRAPPER_WAIT_FOREVER will
 *                     cause the thread to wait indefinitely
 *
 * \return \ref OS_WRAPPER_SUCCESS on success or \ref OS_WRAPPER_ERROR on error
 */
uint32_t os_wrapper_mutex_acquire(uint32_t mutex_id, uint32_t timeout);

/**
 * \brief Releases the mutex acquired previously
 *
 * \param[in] mutex_id The ID of the mutex that has been acquired
 *
 * \return \ref OS_WRAPPER_SUCCESS on success or \ref OS_WRAPPER_ERROR on error
 */
uint32_t os_wrapper_mutex_release(uint32_t mutex_id);

/**
 * \brief Deletes a mutex that is created by \ref os_wrapper_mutex_create()
 *
 * \param[in] mutex_id The ID of the mutex to be deleted
 *
 * \return \ref OS_WRAPPER_SUCCESS on success or \ref OS_WRAPPER_ERROR on error
 */
uint32_t os_wrapper_mutex_delete(uint32_t mutex_id);

/**
 * \brief Creates a new thread
 *
 * \param[in] name        Name of the thread
 * \param[in] stack_size  Size of stack to be allocated for this thread. It can
 *                        be \ref OS_WRAPPER_DEFAULT_STACK_SIZE to use the
 *                        default value provided by the underlying RTOS
 * \param[in] func        Pointer to the function invoked by thread
 * \param[in] arg         Argument to pass to the function invoked by thread
 * \param[in] priority    Initial thread priority
 *
 * \return Returns thread ID, or \ref OS_WRAPPER_ERROR in case of error
 */
uint32_t os_wrapper_thread_new(const char *name, int32_t stack_size,
                               os_wrapper_thread_func func, void *arg,
                               uint32_t priority);
/**
 * \brief Gets current thread ID
 *
 * \return Returns thread ID, or \ref OS_WRAPPER_ERROR in case of error
 */
uint32_t os_wrapper_thread_get_id(void);

/**
 * \brief Gets thread priority
 *
 * \param[in] id Thread ID
 *
 * \return Returns thread priority value, or \ref OS_WRAPPER_ERROR in case of
 *         error
 */
uint32_t os_wrapper_thread_get_priority(uint32_t id);

/**
 * \brief Exits the calling thread
 */
void os_wrapper_thread_exit(void);

#ifdef __cplusplus
}
#endif

#endif /* __OS_WRAPPER_H__ */
