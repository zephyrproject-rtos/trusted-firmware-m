/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "ns_test_helpers.h"

#include "os_wrapper.h"

#include "tfm_nspm_api.h"

#define SST_TEST_TASK_STACK_SIZE 2048

struct test_task_t {
    test_func_t *func;
    struct test_result_t *ret;
};

static uint32_t test_semaphore;

/**
 * \brief Executes the supplied test task and then releases the test semaphore.
 *
 * \param[in,out] arg  Pointer to the test task. Must be a pointer to a
 *                     struct test_task_t
 */
static void test_task_runner(void *arg)
{
    struct test_task_t *test = arg;

#ifdef TFM_NS_CLIENT_IDENTIFICATION
    tfm_nspm_register_client_id();
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

    /* Call the test function */
    test->func(test->ret);

    /* Release the semaphore to unblock the parent thread */
    os_wrapper_semaphore_release(test_semaphore);
}

void tfm_sst_run_test(const char *thread_name, struct test_result_t *ret,
                      test_func_t *test_func)
{
    uint32_t current_thread_id;
    uint32_t current_thread_priority;
    uint32_t err;
    uint32_t thread;
    struct test_task_t test_task = { .func = test_func, .ret = ret };

    test_semaphore = os_wrapper_semaphore_create(1, 0, "sst_tests_mutex");
    if (test_semaphore == OS_WRAPPER_ERROR) {
        TEST_FAIL("Semaphore creation failed");
        return;
    }

    current_thread_id = os_wrapper_get_thread_id();
    if (current_thread_id == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(test_semaphore);
        TEST_FAIL("Failed to get current thread ID");
        return;
    }

    current_thread_priority = os_wrapper_get_thread_priority(current_thread_id);
    if (current_thread_priority == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(test_semaphore);
        TEST_FAIL("Failed to get current thread priority");
        return;
    }

    thread = os_wrapper_new_thread(thread_name, SST_TEST_TASK_STACK_SIZE,
                                   test_task_runner, &test_task,
                                   current_thread_priority);
    if (thread == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(test_semaphore);
        TEST_FAIL("Failed to create test thread");
        return;
    }

    /* Wait indefinitely for the test to finish and release the semaphore */
    err = os_wrapper_semaphore_acquire(test_semaphore, 0xFFFFFFFF);
    err |= os_wrapper_join_thread(thread);
    if (err == OS_WRAPPER_ERROR) {
        TEST_FAIL("Failed while waiting for test thread to end");
    }

    os_wrapper_semaphore_delete(test_semaphore);
}
