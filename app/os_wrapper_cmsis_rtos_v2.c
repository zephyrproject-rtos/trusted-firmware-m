/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "os_wrapper/thread.h"
#include "os_wrapper/mutex.h"
#include "os_wrapper/semaphore.h"

#include <string.h>
#include "cmsis_os2.h"

/* This is an example OS abstraction layer for CMSIS-RTOSv2 */

uint32_t os_wrapper_thread_new(const char *name, int32_t stack_size,
                               os_wrapper_thread_func func, void *arg,
                               uint32_t priority)
{
    osThreadAttr_t task_attribs = {.tz_module = 1};
    osThreadId_t thread_id;

    /* By default, the thread starts as osThreadDetached */
    if (stack_size != OS_WRAPPER_DEFAULT_STACK_SIZE) {
        task_attribs.stack_size = stack_size;
    }
    task_attribs.name = name;
    task_attribs.priority = (osPriority_t) priority;

    thread_id = osThreadNew(func, arg, &task_attribs);
    if (thread_id == NULL) {
        return OS_WRAPPER_ERROR;
    }

    return (uint32_t)thread_id;
}


uint32_t os_wrapper_semaphore_create(uint32_t max_count, uint32_t initial_count,
                                     const char *name)
{
    osSemaphoreAttr_t sema_attrib = {0};
    osSemaphoreId_t semaphore;

    sema_attrib.name = name;

    semaphore = osSemaphoreNew(max_count, initial_count, &sema_attrib);
    if (semaphore == NULL) {
        return OS_WRAPPER_ERROR;
    }

    return (uint32_t)semaphore;
}

uint32_t os_wrapper_semaphore_acquire(uint32_t semaphore_id, uint32_t timeout)
{
    osStatus_t status;

    status = osSemaphoreAcquire((osSemaphoreId_t)semaphore_id,
                                (timeout == OS_WRAPPER_WAIT_FOREVER) ?
                                osWaitForever : timeout);
    if (status != osOK) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_semaphore_release(uint32_t sema)
{
    osStatus_t status;

    status = osSemaphoreRelease((osSemaphoreId_t)sema);
    if (status != osOK) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_semaphore_delete(uint32_t sema)
{
    osStatus_t status;

    status = osSemaphoreDelete((osSemaphoreId_t)sema);
    if (status != osOK) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_mutex_create(void)
{
    osMutexId_t id;
    const osMutexAttr_t attr = {
        .name = NULL,
        .attr_bits = osMutexPrioInherit, /* Priority inheritance is recommended
                                          * to enable if it is supported.
                                          * For recursive mutex and the ability
                                          * of auto release when owner being
                                          * terminated is not required.
                                          */
        .cb_mem = NULL,
        .cb_size = 0U
    };

    id = osMutexNew(&attr);
    if (!id) {
        return OS_WRAPPER_ERROR;
    }

    return (uint32_t)id;
}

uint32_t os_wrapper_mutex_acquire(uint32_t mutex_id, uint32_t timeout)
{
    osStatus_t status = osOK;

    if (!mutex_id) {
        return OS_WRAPPER_ERROR;
    }

    status = osMutexAcquire((osMutexId_t)mutex_id,
                            (timeout == OS_WRAPPER_WAIT_FOREVER) ?
                             osWaitForever : timeout);
    if (status != osOK) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_mutex_release(uint32_t mutex_id)
{
    osStatus_t status = osOK;

    if (!mutex_id) {
        return OS_WRAPPER_ERROR;
    }

    status = osMutexRelease((osMutexId_t)mutex_id);
    if (status != osOK) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_mutex_delete(uint32_t mutex_id)
{
    osStatus_t status = osOK;

    if (!mutex_id) {
        return OS_WRAPPER_ERROR;
    }

    status = osMutexDelete((osMutexId_t)mutex_id);
    if (status != osOK) {
        return OS_WRAPPER_ERROR;
    }

    return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_thread_get_id(void)
{
    osThreadId_t thread_id;

    thread_id = osThreadGetId();
    if (thread_id == NULL) {
        return OS_WRAPPER_ERROR;
    }

    return (uint32_t)thread_id;
}

uint32_t os_wrapper_thread_get_priority(uint32_t id)
{
    osPriority_t prio;

    prio = osThreadGetPriority((osThreadId_t)id);
    if (prio == osPriorityError) {
        return OS_WRAPPER_ERROR;
    }

    return prio;
}

void os_wrapper_thread_exit(void)
{
    osThreadExit();
}
