/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_tests.h"

#include <stdio.h>
#include <string.h>

#include "nv_counters/test_sst_nv_counters.h"
#include "psa_sst_api.h"
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "secure_fw/services/secure_storage/sst_object_system.h"
#include "s_test_helpers.h"
#include "test/framework/test_framework_helpers.h"

/* Test suite defines */
#define READ_BUF_SIZE  1UL
#define WRITE_BUF_SIZE 1UL

/* Define default asset's token */
#define ASSET_TOKEN      NULL
#define ASSET_TOKEN_SIZE 0

/*
 * Summary of tests covered by the test suite.
 *
 * SST version | NVC1 | NVC2 | NVC3 |  Result  |  Test Num
 * ------------|------|------|------|----------|------------
 *      X      |   X  |   X  |   X  |  Valid   |     1
 *      N      |   X  |   X  |   X  |  Invalid |     2
 *      X      |   X  |   X  |   N  |  Valid   |     3
 *      N      |   X  |   N  |   N  |  Valid   |     4
 *      X      |   X  |   N  |   N  |  Valid   |     5
 *      X      |   X  |   M  |   N  |  Valid   |     6
 *      M      |   X  |   M  |   N  |  Invalid |     7
 *      N      |   X  |   M  |   N  |  Invalid |     8
 *
 * Test 9 checks the SST result when the non-volatile (NV) counter 1 cannot be
 * incremented (e.g it has reached its maximum value).
 */

/* List of tests */
static void tfm_sst_test_4001(struct test_result_t *ret);
static void tfm_sst_test_4002(struct test_result_t *ret);
static void tfm_sst_test_4003(struct test_result_t *ret);
static void tfm_sst_test_4004(struct test_result_t *ret);
static void tfm_sst_test_4005(struct test_result_t *ret);
static void tfm_sst_test_4006(struct test_result_t *ret);
static void tfm_sst_test_4007(struct test_result_t *ret);
static void tfm_sst_test_4008(struct test_result_t *ret);
static void tfm_sst_test_4009(struct test_result_t *ret);

static struct test_t interface_tests[] = {
    {&tfm_sst_test_4001, "TFM_SST_TEST_4001",
     "Check SST area version when NV counters 1/2/3 have the same value",
     {0}},
    {&tfm_sst_test_4002, "TFM_SST_TEST_4002",
     "Check SST area version when it is different from NV counters 1/2/3",
     {0}},
    {&tfm_sst_test_4003, "TFM_SST_TEST_4003",
     "Check SST area version when NV counters 1 and 2 are equals, 3 is "
     "different, and SST area version match NV counters 1 and 2", {0}},
    {&tfm_sst_test_4004, "TFM_SST_TEST_4004",
     "Check SST area version when NV counters 2 and 3 are equals, 1 is "
     "different and SST area version match NV counter 2 and 3", {0}},
    {&tfm_sst_test_4005, "TFM_SST_TEST_4005",
     "Check SST area version when NV counters 2 and 3 are equals, 1 is "
     "different and SST area version match NV counter 1", {0}},
    {&tfm_sst_test_4006, "TFM_SST_TEST_4006",
     "Check SST area version when NV counters 1, 2 and 3 have different values "
     "and SST area version match NV counter 1 value", {0}},
    {&tfm_sst_test_4007, "TFM_SST_TEST_4007",
     "Check SST area version when NV counters 1, 2 and 3 have different values "
     "and SST area version match NV counter 2 value", {0}},
    {&tfm_sst_test_4008, "TFM_SST_TEST_4008",
     "Check SST area version when NV counters 1, 2 and 3 have different values "
     "and SST area version match NV counter 3 value", {0}},
    {&tfm_sst_test_4009, "TFM_SST_TEST_4009",
     "Check SST area version when NV counter 1 cannot be incremented", {0}},
};

void register_testsuite_s_rollback_protection(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(interface_tests) / sizeof(interface_tests[0]));

    set_testsuite("SST rollback protection tests (TFM_SST_TEST_4XXX)",
                  interface_tests, list_size, p_test_suite);
}

/**
 * \brief Check SST area version when NV counters 1/2/3 have the same value.
 *        It also checks that the 3 NV counters are aligned and they have been
 *        increased by 1 unit.
 */
static void tfm_sst_test_4001(struct test_result_t *ret)
{
    struct sst_test_buf_t io_data;
    enum psa_sst_err_t err;
    uint32_t old_nvc_1, nvc_1, nvc_2, nvc_3;
    uint8_t read_data = 'X';
    uint8_t wrt_data = 'D';

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Reads NV counter 1 to get the save the value to compare it later */
    err = test_sst_read_nv_counter(TFM_SST_NV_COUNTER_1, &old_nvc_1);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    /* Sets data structure for write request */
    io_data.data = &wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data into the asset */
    err = psa_sst_write(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for application S_APP_ID");
        return;
    }

    /* Validates the 3 NV counters have the same value and it has been increased
     * by 1 unit.
     */

    /* Reads NV counter 1 to get the current value */
    err = test_sst_read_nv_counter(TFM_SST_NV_COUNTER_1, &nvc_1);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    /* Checks if NV counter 1 value has been increased by 1 unit as result of
     * process the write request.
     */
    if (nvc_1 != (old_nvc_1 + 1)) {
        TEST_FAIL("NV counter 1 has been increased more than 1 unit");
        return;
    }

    /* Reads NV counter 2 to get the current value */
    err = test_sst_read_nv_counter(TFM_SST_NV_COUNTER_2, &nvc_2);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    if (nvc_1 != nvc_2) {
        TEST_FAIL("NV counter 1 and 2 should have the same value");
        return;
    }

    /* Reads NV counter 3 to get the current value */
    err = test_sst_read_nv_counter(TFM_SST_NV_COUNTER_3, &nvc_3);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    if (nvc_2 != nvc_3) {
        TEST_FAIL("NV counter 2 and 3 should have the same value");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the NV counters has the same values and
     * the SST area authentication is aligned with those values.
     */
    err = sst_system_prepare();
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("AM prepare should not fail");
        return;
    }

    /* Sets data structure for read request */
    io_data.data = &read_data;
    io_data.size = READ_BUF_SIZE;
    io_data.offset = 0;

    /* Reads data from the asset */
    err = psa_sst_read(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for application S_APP_ID");
        return;
    }

    if (read_data != wrt_data) {
        TEST_FAIL("Read and write data values are different");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when it is different from NV counters
 *        1/2/3.
 */
static void tfm_sst_test_4002(struct test_result_t *ret)
{
    enum psa_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Increments all counters to make that SST area version old/invalid */
    err = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    err = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_2);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    err = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_3);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should fail as the SST area version does not match the
     * NV counters values.
     */
    err = sst_system_prepare();
    if (err != PSA_SST_ERR_SYSTEM_ERROR) {
        TEST_FAIL("AM prepare should fail as version is old");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 1 and 2 are equals, 3 is
 *        different and SST area version match NV counter 1 and 2 values.
 *        It simulates a power cut during write action while the counter 3 is
 *        being increased.
 */
static void tfm_sst_test_4003(struct test_result_t *ret)
{
    enum psa_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    /* Decrements NV counters 3 to make it different from the other two counters
     * and make the current SST area version match NV counter 1 and 2 values.
     */
    err = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the SST area version match NV counters 1 and
     * 2 values.
     */
    err = sst_system_prepare();
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("AM prepare should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 2 and 3 are equals, 1 is
 *        different and SST area version match NV counter 2 and 3 values.
 *        It simulates a power cut during write action before increment counter
 *        2 and 3, and the new SST area version is corrupted and only the old
 *        version match the NV counters.
 */
static void tfm_sst_test_4004(struct test_result_t *ret)
{
    enum psa_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    /* Increments NV counters 1 to make it different from the other two counters
     * and make the current SST area version match NV counter 2 and 3 values.
     */
    err = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the SST area version match the NV counter 2
     * and 3 values.
     */
    err = sst_system_prepare();
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("AM prepare should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 2 and 3 are equals, 1 is
 *        different and SST area version match NV counter 1 value.
 *        It simulates a power cut during write action before increment counter
 *        2 and 3, and the new SST area version is corrupted and only the old
 *        version match the NV counters.
 */
static void tfm_sst_test_4005(struct test_result_t *ret)
{
    enum psa_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    /* Decrements NV counter 2 and 3 to make the SST area version match NV
     * counter 1 only.
     */
    err = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_2);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    err = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the SST area version match the NV counter 1.
     */
    err = sst_system_prepare();
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("AM prepare should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 1, 2 and 3 have different
 *        values and SST area version match NV counter 1 value.
 */
static void tfm_sst_test_4006(struct test_result_t *ret)
{
    enum psa_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    /* Decrements NV counter 2 (1 time) and 3 (2 times) to make the SST area
     * version match NV counter 1 only.
     */
    err = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_2);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    err = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    err = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should not fail as the SST area version match the NV counter 1.
     */
    err = sst_system_prepare();
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("AM prepare should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 1, 2 and 3 have different
 *        values and SST area version match NV counter 2 value.
 */
static void tfm_sst_test_4007(struct test_result_t *ret)
{
    enum psa_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    /* Increments NV counter 1 and decrements 3 to make the SST area
     * version match NV counter 2 only.
     */
    err = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    err = test_sst_decrement_nv_counter(TFM_SST_NV_COUNTER_3);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Decrement should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should fail as the SST area version match the NV counter 2 and
     * the other counters are different.
     */
    err = sst_system_prepare();
    if (err != PSA_SST_ERR_SYSTEM_ERROR) {
        TEST_FAIL("AM prepare should fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counters 1, 2 and 3 have different
 *        values and SST area version match NV counter 3 value.
 */
static void tfm_sst_test_4008(struct test_result_t *ret)
{
    enum psa_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    /* Increments NV counter 1 (2 times) and 2 (1 time) to make the SST area
     * version match NV counter 3 only.
     */
    err = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    err = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    err = test_sst_increment_nv_counter(TFM_SST_NV_COUNTER_2);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Increment should not fail");
        return;
    }

    /* Simulates a reboot in the system by calling sst_system_prepare(). This
     * function is called when the SST service is initialized.
     *
     * Prepare should fail as the SST area version match the NV counter 2 and
     * the other counters are different.
     */
    err = sst_system_prepare();
    if (err != PSA_SST_ERR_SYSTEM_ERROR) {
        TEST_FAIL("AM prepare should fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Check SST area version when NV counter 1 cannot be incremented
 *        (e.g it has reached its maximum value)
 */
static void tfm_sst_test_4009(struct test_result_t *ret)
{
     enum psa_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Disables increment function to simulate that NV counter 1 has
     * reached its maximum value.
     */
    test_sst_disable_increment_nv_counter();

    /* Creates an asset in the SST area to generate a new SST area version */
    err = psa_sst_create(SST_ASSET_ID_AES_KEY_192, ASSET_TOKEN,
                         ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SYSTEM_ERROR) {
        TEST_FAIL("Create should fail as NV counter cannot be incremented");
        return;
    }

    /* Enables counter again to not affect the next tests, if any */
    test_sst_enable_increment_nv_counter();

    ret->val = TEST_PASSED;
}
