/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_ns_tests.h"

#include <string.h>

#include "os_wrapper.h"
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "test/framework/helpers.h"
#include "test/test_services/tfm_sst_test_service/sst_test_service_api.h"

#define DATA "TEST_DATA_ONE_TWO_THREE_FOUR_FIVE"
#define BUF_SIZE (sizeof(DATA))

/* Test tasks's stack size */
#define SST_TEST_TASK_STACK_SIZE 2048

static uint32_t tfm_sst_test_sema;
static struct test_result_t transient_result;

/* List of tests */
static void tfm_sst_test_5001(struct test_result_t *ret);
static void tfm_sst_test_5002(struct test_result_t *ret);
static void tfm_sst_test_5003(struct test_result_t *ret);
static void tfm_sst_test_5004(struct test_result_t *ret);

static struct test_t sst_ref_access_tests[] = {
    { &tfm_sst_test_5001, "TFM_SST_TEST_5001",
     "Setup the SST test service at the start of the tests", { 0 } },
    { &tfm_sst_test_5002, "TFM_SST_TEST_5002",
     "Access by reference with correct permissions", { 0 } },
    { &tfm_sst_test_5003, "TFM_SST_TEST_5003",
     "Access by reference with incorrect permissions", { 0 } },
    { &tfm_sst_test_5004, "TFM_SST_TEST_5004",
     "Clean up the SST test service at the end of the tests", { 0 } },
};

void register_testsuite_ns_sst_ref_access(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = sizeof(sst_ref_access_tests) / sizeof(sst_ref_access_tests[0]);

    set_testsuite("SST referenced access tests (TFM_SST_TEST_5XXX)",
                  sst_ref_access_tests, list_size, p_test_suite);
}

/* FIXME: Duplicated function should be refactored into a new interface */
/**
 * \brief Executes given test from specified thread context
 */
static void tfm_sst_run_test(const char *name, struct test_result_t *ret,
                             os_wrapper_thread_func func)
{
    uint32_t err;
    uint32_t current_thread_id;
    uint32_t current_thread_priority;
    uint32_t thread;

    tfm_sst_test_sema = os_wrapper_semaphore_create(1, 0, "sst_tests_mutext");
    if (tfm_sst_test_sema == OS_WRAPPER_ERROR) {
        TEST_FAIL("Semaphore creation has failed\n");
        return;
    }

    current_thread_id = os_wrapper_get_thread_id();
    if (current_thread_id == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(tfm_sst_test_sema);
        TEST_FAIL("Get current thread ID has failed");
        return;
    }

    current_thread_priority = os_wrapper_get_thread_priority(current_thread_id);
    if (current_thread_priority == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(tfm_sst_test_sema);
        TEST_FAIL("Get current thread priority has failed");
        return;
    }

    thread = os_wrapper_new_thread(name, SST_TEST_TASK_STACK_SIZE,
                                   func, current_thread_priority);
    if (thread == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(tfm_sst_test_sema);
        TEST_FAIL("Thread creation has failed");
        return;
    }

    /* Wait for the test to finish and release the semaphore */
    err = os_wrapper_semaphore_acquire(tfm_sst_test_sema, 0xFFFFFFFF);
    if (err == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(tfm_sst_test_sema);
        TEST_FAIL("Semaphore wait has failed");
        return;
    }

    err = os_wrapper_join_thread(thread);
    if (err == OS_WRAPPER_ERROR) {
        os_wrapper_semaphore_delete(tfm_sst_test_sema);
        TEST_FAIL("Join thread has failed");
        return;
    }

    /* Populate the results */
    memcpy(ret, &transient_result, sizeof(struct test_result_t));
    os_wrapper_semaphore_delete(tfm_sst_test_sema);
}

/**
 * \note List of relations between thread name, app ID and permissions
 *
 * Asset permissions: SST_ASSET_ID_AES_KEY_128
 *
 *   THREAD NAME | APP_ID       | Permissions
 *   ------------|--------------------------------------
 *     Thread_C  | SST_APP_ID_2 | NONE
 *     Thread_D  | SST_APP_ID_3 | REFERENCE
 *
 */

/**
 * \brief Tests the set-up of the SST test service
 */
static void tfm_sst_test_5001_task(void *arg)
{
    enum tfm_sst_err_t err;
    struct test_result_t *ret = &transient_result;

    err = sst_test_service_setup();
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Failed to setup the SST test service");
        os_wrapper_semaphore_release(tfm_sst_test_sema);
        return;
    }

    ret->val = TEST_PASSED;
    os_wrapper_semaphore_release(tfm_sst_test_sema);
}

/**
 * \brief Test SST referenced access with correct permissions
 */
static void tfm_sst_test_5002_task(void *arg)
{
    enum tfm_sst_err_t err;
    struct test_result_t *ret = &transient_result;
    uint16_t key_uuid = SST_ASSET_ID_AES_KEY_128;
    uint8_t buf[BUF_SIZE] = DATA;

    err = sst_test_service_dummy_encrypt(key_uuid, buf, BUF_SIZE);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Encryption should be successful for SST_APP_ID_3");
        os_wrapper_semaphore_release(tfm_sst_test_sema);
        return;
    }

    if (memcmp(buf, DATA, BUF_SIZE) == 0) {
        TEST_FAIL("Contents of buf should have changed");
        os_wrapper_semaphore_release(tfm_sst_test_sema);
        return;
    }

    err = sst_test_service_dummy_decrypt(key_uuid, buf, BUF_SIZE);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Decryption should be successful for SST_APP_ID_3");
        os_wrapper_semaphore_release(tfm_sst_test_sema);
        return;
    }

    if (memcmp(buf, DATA, BUF_SIZE) != 0) {
        TEST_FAIL("Contents of buf should be the same as the original "
                  "contents");
        os_wrapper_semaphore_release(tfm_sst_test_sema);
        return;
    }

    ret->val = TEST_PASSED;
    os_wrapper_semaphore_release(tfm_sst_test_sema);
}

/**
 * \brief Test SST referenced access with incorrect permissions
 */
static void tfm_sst_test_5003_task(void *arg)
{
    enum tfm_sst_err_t err;
    struct test_result_t *ret = &transient_result;
    uint16_t key_uuid = SST_ASSET_ID_AES_KEY_128;
    uint8_t buf[BUF_SIZE] = DATA;

    err = sst_test_service_dummy_encrypt(key_uuid, buf, BUF_SIZE);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Encryption should not be successful for SST_APP_ID_2");
        os_wrapper_semaphore_release(tfm_sst_test_sema);
        return;
    }

    err = sst_test_service_dummy_decrypt(key_uuid, buf, BUF_SIZE);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Decryption should not be successful for SST_APP_ID_2");
        os_wrapper_semaphore_release(tfm_sst_test_sema);
        return;
    }

    ret->val = TEST_PASSED;
    os_wrapper_semaphore_release(tfm_sst_test_sema);
}

/**
 * \brief Clean the SST referenced access service at the end of the tests
 */
static void tfm_sst_test_5004_task(void *arg)
{
    enum tfm_sst_err_t err;
    struct test_result_t *ret = &transient_result;

    err = sst_test_service_clean();
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Cleaning of the SST referenced access service failed");
        os_wrapper_semaphore_release(tfm_sst_test_sema);
        return;
    }

    ret->val = TEST_PASSED;
    os_wrapper_semaphore_release(tfm_sst_test_sema);
}

static void tfm_sst_test_5001(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_D", ret, tfm_sst_test_5001_task);
}

static void tfm_sst_test_5002(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_D", ret, tfm_sst_test_5002_task);
}

static void tfm_sst_test_5003(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_C", ret, tfm_sst_test_5003_task);
}

static void tfm_sst_test_5004(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_D", ret, tfm_sst_test_5004_task);
}
