/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_ns_tests.h"

#include <string.h>

#include "ns_test_helpers.h"
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "test/framework/helpers.h"
#include "test/test_services/tfm_sst_test_service/sst_test_service_api.h"

#define DATA "TEST_DATA_ONE_TWO_THREE_FOUR_FIVE"
#define BUF_SIZE (sizeof(DATA))

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
static void tfm_sst_test_5001_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;

    err = sst_test_service_setup();
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Failed to setup the SST test service");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Test SST referenced access with correct permissions
 */
static void tfm_sst_test_5002_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;
    uint16_t key_uuid = SST_ASSET_ID_AES_KEY_128;
    uint8_t buf[BUF_SIZE] = DATA;

    err = sst_test_service_dummy_encrypt(key_uuid, buf, BUF_SIZE);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Encryption should be successful for SST_APP_ID_3");
        return;
    }

    if (memcmp(buf, DATA, BUF_SIZE) == 0) {
        TEST_FAIL("Contents of buf should have changed");
        return;
    }

    err = sst_test_service_dummy_decrypt(key_uuid, buf, BUF_SIZE);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Decryption should be successful for SST_APP_ID_3");
        return;
    }

    if (memcmp(buf, DATA, BUF_SIZE) != 0) {
        TEST_FAIL("Contents of buf should be the same as the original "
                  "contents");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Test SST referenced access with incorrect permissions
 */
static void tfm_sst_test_5003_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;
    uint16_t key_uuid = SST_ASSET_ID_AES_KEY_128;
    uint8_t buf[BUF_SIZE] = DATA;

    err = sst_test_service_dummy_encrypt(key_uuid, buf, BUF_SIZE);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Encryption should not be successful for SST_APP_ID_2");
        return;
    }

    err = sst_test_service_dummy_decrypt(key_uuid, buf, BUF_SIZE);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Decryption should not be successful for SST_APP_ID_2");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Clean the SST referenced access service at the end of the tests
 */
static void tfm_sst_test_5004_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;

    err = sst_test_service_clean();
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Cleaning of the SST referenced access service failed");
        return;
    }

    ret->val = TEST_PASSED;
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
