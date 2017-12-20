/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_tests.h"

#include <stdio.h>
#include <string.h>

#include "test/framework/helpers.h"
#include "tfm_sst_defs.h"
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "tfm_sst_veneers.h"
#include "s_test_helpers.h"

/* Test suite defines */
#define LOOP_ITERATIONS_001 5U

/* Perform one more create/delete than the number of assets to check that
 * object metadata is being freed correctly.
 */
#define LOOP_ITERATIONS_002 (SST_NUM_ASSETS + 1U)

#define WRITE_BUF_SIZE      33U
#define READ_BUF_SIZE       (WRITE_BUF_SIZE + 6U)

#define WRITE_DATA          "PACKMYBOXWITHFIVEDOZENLIQUORJUGS"
#define READ_DATA           "######################################"
#define RESULT_DATA         ("###" WRITE_DATA "###")

/* Define test suite for SST reliability tests */
/* List of tests */
static void tfm_sst_test_3001(struct test_result_t *ret);
static void tfm_sst_test_3002(struct test_result_t *ret);

static struct test_t reliability_tests[] = {
    {&tfm_sst_test_3001, "TFM_SST_TEST_3001",
     "repetitive writes in an asset", {0} },
    {&tfm_sst_test_3002, "TFM_SST_TEST_3002",
     "repetitive create, write, read and delete", {0} },
};

void register_testsuite_s_sst_reliability(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(reliability_tests) /
                          sizeof(reliability_tests[0]));

    set_testsuite("SST reliability tests (TFM_SST_TEST_3XXX)",
                  reliability_tests, list_size, p_test_suite);
}

/**
 * \brief Tests repetitive writes in an asset with the follow permissions:
 *        APP_ID       | Permissions
 *        -------------------------------------
 *        S_APP_ID | REFERENCE, READ, WRITE
 */
static void tfm_sst_test_3001(struct test_result_t *ret)
{
    uint32_t hdl;
    uint32_t app_id = S_APP_ID;
    uint32_t asset_offset = 0;
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    struct tfm_sst_buf_t data;
    enum tfm_sst_err_t err;
    uint32_t itr;
    uint8_t wrt_data[WRITE_BUF_SIZE] = WRITE_DATA;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Checks write permissions in create function */
    err = tfm_sst_veneer_create(app_id, asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    err = tfm_sst_veneer_get_handle(app_id, asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should not fail");
        return;
    }

    /* Sets write and read sizes */
    data.size = WRITE_BUF_SIZE-1;

    for (itr = 0; itr < LOOP_ITERATIONS_001; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS_001);

        do {
            /* Sets data structure */
            data.data = wrt_data;
            data.offset = asset_offset;

            /* Checks write permissions in the write function */
            err = tfm_sst_veneer_write(app_id, hdl, &data);
            if (err != TFM_SST_ERR_SUCCESS) {
                TEST_FAIL("Write should not fail for application S_APP_ID");
                return;
            }

            /* Updates data structure to point to the read buffer */
            data.data = &read_data[3];

            /* Checks write permissions in the read function */
            err = tfm_sst_veneer_read(app_id, hdl, &data);
            if (err != TFM_SST_ERR_SUCCESS) {
                TEST_FAIL("Application S_APP_ID must get file handle");
                return;
            }

            /* Checks read data buffer content */
            if (memcmp(read_data, RESULT_DATA, READ_BUF_SIZE) != 0) {
                TEST_FAIL("Read buffer contains incorrect data");
                return;
            }

            /* Reset read buffer data */
            memset(data.data, 'X', data.size);

            /* Moves asset offsets to next position */
            asset_offset += data.size;

        } while (asset_offset < SST_ASSET_MAX_SIZE_X509_CERT_LARGE);

        /* Resets asset_offset */
        asset_offset = 0;
    }

    TEST_LOG("\n");

    /* Checks write permissions in delete function */
    err = tfm_sst_veneer_delete(app_id, hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for application S_APP_ID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests repetitive creates, reads, writes and deletes, with the follow
 *        permissions:
 *        APP_ID       | Permissions
 *        -------------------------------------
 *        S_APP_ID | REFERENCE, READ, WRITE
 */
static void tfm_sst_test_3002(struct test_result_t *ret)
{
    uint32_t hdl;
    uint32_t app_id = S_APP_ID;
    uint32_t asset_offset = 0;
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    struct tfm_sst_buf_t data;
    enum tfm_sst_err_t err;
    uint32_t itr;
    uint8_t wrt_data[WRITE_BUF_SIZE] = WRITE_DATA;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Sets write and read sizes */
    data.size = WRITE_BUF_SIZE-1;

    for (itr = 0; itr < LOOP_ITERATIONS_002; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS_002);

        /* Checks write permissions in create function */
        err = tfm_sst_veneer_create(app_id, asset_uuid);
        if (err != TFM_SST_ERR_SUCCESS) {
            TEST_FAIL("Create should not fail for application S_APP_ID");
            return;
        }

        err = tfm_sst_veneer_get_handle(app_id, asset_uuid, &hdl);
        if (err != TFM_SST_ERR_SUCCESS) {
            TEST_FAIL("Get handle should not fail");
            return;
        }

        do {
            /* Sets data structure */
            data.data = wrt_data;
            data.offset = asset_offset;

            /* Checks write permissions in the write function */
            err = tfm_sst_veneer_write(app_id, hdl, &data);
            if (err != TFM_SST_ERR_SUCCESS) {
                TEST_FAIL("Write should not fail for application S_APP_ID");
                return;
            }

            /* Updates data structure to point to the read buffer */
            data.data = &read_data[3];

            /* Checks write permissions in the read function */
            err = tfm_sst_veneer_read(app_id, hdl, &data);
            if (err != TFM_SST_ERR_SUCCESS) {
                TEST_FAIL("Application S_APP_ID must get file handle");
                return;
            }

            /* Checks read data buffer content */
            if (memcmp(read_data, RESULT_DATA, READ_BUF_SIZE) != 0) {
                read_data[READ_BUF_SIZE-1] = 0;
                TEST_FAIL("Read buffer contains incorrect data");
                return;
            }

            /* Reset read buffer data */
            memset(data.data, 'X', data.size);

            /* Moves asset offsets to next position */
            asset_offset += data.size;

        } while (asset_offset < SST_ASSET_MAX_SIZE_X509_CERT_LARGE);

        /* Checks write permissions in delete function */
        err = tfm_sst_veneer_delete(app_id, hdl);
        if (err != TFM_SST_ERR_SUCCESS) {
            TEST_FAIL("Delete should not fail for application S_APP_ID");
            return;
        }

        /* Resets asset_offset */
        asset_offset = 0;
    }

    TEST_LOG("\n");

    ret->val = TEST_PASSED;
}
