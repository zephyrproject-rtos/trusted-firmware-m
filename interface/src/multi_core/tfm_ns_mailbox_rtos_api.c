/*
 * Copyright (c) 2020-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This file is a reference implementation of Non-secure mailbox RTOS API.
 * This reference implementation is based on TF-M ROTS wrapper API.
 * It can be replaced by RTOS specific implementation.
 */

#include "os_wrapper/semaphore.h"
#include "os_wrapper/thread.h"

#include "tfm_ns_mailbox.h"

#define MAX_SEMAPHORE_COUNT            NUM_MAILBOX_QUEUE_SLOT

/*
 * Thread flag to manage wait/wake mechanism in mailbox.、
 * Thread flag can be RTOS specific.
 * The following example definition also covers the rule of CMSIS-RTOS2, which
 * requires the MSB of thread flags must be 0b0.
 */
#define MAILBOX_THREAD_FLAG            0x5FCA0000

static void *ns_lock_handle = NULL;

const void *tfm_ns_mailbox_os_get_task_handle(void)
{
    return os_wrapper_thread_get_handle();
}

void tfm_ns_mailbox_os_wait_reply(void)
{
    os_wrapper_thread_wait_flag(MAILBOX_THREAD_FLAG, OS_WRAPPER_WAIT_FOREVER);
}

void tfm_ns_mailbox_os_wake_task_isr(const void *task_handle)
{
    os_wrapper_thread_set_flag_isr((void *)task_handle, MAILBOX_THREAD_FLAG);
}

int32_t tfm_ns_mailbox_os_lock_init(void)
{
    ns_lock_handle = os_wrapper_semaphore_create(MAX_SEMAPHORE_COUNT,
                                                 MAX_SEMAPHORE_COUNT,
                                                 NULL);
    if (!ns_lock_handle) {
        return MAILBOX_GENERIC_ERROR;
    }

    return MAILBOX_SUCCESS;
}

int32_t tfm_ns_mailbox_os_lock_acquire(void)
{
    return os_wrapper_semaphore_acquire(ns_lock_handle,
                                        OS_WRAPPER_WAIT_FOREVER);
}

int32_t tfm_ns_mailbox_os_lock_release(void)
{
    return os_wrapper_semaphore_release(ns_lock_handle);
}
