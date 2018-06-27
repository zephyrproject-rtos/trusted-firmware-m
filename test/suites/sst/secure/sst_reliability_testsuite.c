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

/* Define default asset's token */
static struct tfm_sst_token_t test_token = { .token = NULL, .token_size = 0};

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
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
static void tfm_sst_test_3001(struct test_result_t *ret)
{
    uint32_t app_id = S_APP_ID;
    uint32_t asset_offset = 0;
    const uint32_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    struct tfm_sst_buf_t io_data;
    enum psa_sst_err_t err;
    uint32_t itr;
    uint8_t wrt_data[WRITE_BUF_SIZE] = WRITE_DATA;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Checks write permissions in create function */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets write and read sizes */
    io_data.size = WRITE_BUF_SIZE-1;

    for (itr = 0; itr < LOOP_ITERATIONS_001; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS_001);

        do {
            /* Sets data structure */
            io_data.data = wrt_data;
            io_data.offset = asset_offset;

            /* Checks write permissions in the write function */
            err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token,
                                       &io_data);
            if (err != PSA_SST_ERR_SUCCESS) {
                TEST_FAIL("Write should not fail for application S_APP_ID");
                return;
            }

            /* Updates data structure to point to the read buffer */
            io_data.data = &read_data[3];

            /* Checks write permissions in the read function */
            err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token,
                                      &io_data);
            if (err != PSA_SST_ERR_SUCCESS) {
                TEST_FAIL("Application S_APP_ID must get file handle");
                return;
            }

            /* Checks read data buffer content */
            if (memcmp(read_data, RESULT_DATA, READ_BUF_SIZE) != 0) {
                TEST_FAIL("Read buffer contains incorrect data");
                return;
            }

            /* Reset read buffer data */
            memset(io_data.data, 'X', io_data.size);

            /* Moves asset offsets to next position */
            asset_offset += io_data.size;

        } while (asset_offset < SST_ASSET_MAX_SIZE_X509_CERT_LARGE);

        /* Resets asset_offset */
        asset_offset = 0;
    }

    TEST_LOG("\n");

    /* Checks write permissions in delete function */
    err = tfm_sst_veneer_delete(app_id, asset_uuid, &test_token);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for application S_APP_ID");
        return;
    }

    ret->val = TEST_PASSED;
}
#else
static void tfm_sst_test_3001(struct test_result_t *ret)
{
    uint32_t app_id = S_APP_ID;
    struct tfm_sst_buf_t io_data;
    enum psa_sst_err_t err;
    uint32_t itr;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    uint8_t data[BUFFER_PLUS_PADDING_SIZE] = {0};
    uint32_t i;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Checks write permissions in create function */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets write and read sizes */
    io_data.size = SST_ASSET_MAX_SIZE_AES_KEY_192;

    for (itr = 0; itr < LOOP_ITERATIONS_001; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS_001);

        memset(data, 0, BUFFER_PLUS_PADDING_SIZE);
        /* Sets data structure */
        io_data.data = data;
        io_data.offset = 0;

        /* Checks write permissions in the write function */
        err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
        if (err != PSA_SST_ERR_SUCCESS) {
            TEST_FAIL("Write should not fail for application S_APP_ID");
            return;
        }

        memset(data, 'X', BUFFER_PLUS_PADDING_SIZE);
        io_data.data = data + HALF_PADDING_SIZE;

        /* Checks write permissions in the read function */
        err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
        if (err != PSA_SST_ERR_SUCCESS) {
            TEST_FAIL("Application S_APP_ID must get file handle");
            return;
        }

        /* Checks read data buffer content */
        if (memcmp(data, "XXXX", HALF_PADDING_SIZE) != 0) {
            TEST_FAIL("Read buffer contains illegal pre-data");
            return;
        }

        for (i=HALF_PADDING_SIZE; i<(BUFFER_PLUS_HALF_PADDING_SIZE); i++) {
            if (data[i] != 0) {
                TEST_FAIL("Read buffer has read incorrect data");
                return;
            }
        }

        if (memcmp((data+BUFFER_PLUS_HALF_PADDING_SIZE), "XXXX",
                    HALF_PADDING_SIZE) != 0) {
            TEST_FAIL("Read buffer contains illegal post-data");
            return;
        }
    }

    TEST_LOG("\n");

    /* Checks write permissions in delete function */
    err = tfm_sst_veneer_delete(app_id, asset_uuid, &test_token);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for application S_APP_ID");
        return;
    }

    ret->val = TEST_PASSED;
}
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

/**
 * \brief Tests repetitive creates, reads, writes and deletes, with the follow
 *        permissions:
 *        APP_ID       | Permissions
 *        -------------------------------------
 *        S_APP_ID | REFERENCE, READ, WRITE
 */
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
static void tfm_sst_test_3002(struct test_result_t *ret)
{
    uint32_t app_id = S_APP_ID;
    uint32_t asset_offset = 0;
    const uint32_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    struct tfm_sst_buf_t io_data;
    enum psa_sst_err_t err;
    uint32_t itr;
    uint8_t wrt_data[WRITE_BUF_SIZE] = WRITE_DATA;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Sets write and read sizes */
    io_data.size = WRITE_BUF_SIZE-1;

    for (itr = 0; itr < LOOP_ITERATIONS_002; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS_002);

        /* Checks write permissions in create function */
        err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
        if (err != PSA_SST_ERR_SUCCESS) {
            TEST_FAIL("Create should not fail for application S_APP_ID");
            return;
        }

        do {
            /* Sets data structure */
            io_data.data = wrt_data;
            io_data.offset = asset_offset;

            /* Checks write permissions in the write function */
            err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token,
                                       &io_data);
            if (err != PSA_SST_ERR_SUCCESS) {
                TEST_FAIL("Write should not fail for application S_APP_ID");
                return;
            }

            /* Updates data structure to point to the read buffer */
            io_data.data = &read_data[3];

            /* Checks write permissions in the read function */
            err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token,
                                      &io_data);
            if (err != PSA_SST_ERR_SUCCESS) {
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
            memset(io_data.data, 'X', io_data.size);

            /* Moves asset offsets to next position */
            asset_offset += io_data.size;

        } while (asset_offset < SST_ASSET_MAX_SIZE_X509_CERT_LARGE);

        /* Resets asset_offset */
        asset_offset = 0;

        /* Checks write permissions in delete function */
        err = tfm_sst_veneer_delete(app_id, asset_uuid, &test_token);
        if (err != PSA_SST_ERR_SUCCESS) {
            TEST_FAIL("Delete should not fail for application S_APP_ID");
            return;
        }
    }

    TEST_LOG("\n");

    ret->val = TEST_PASSED;
}

#else
static void tfm_sst_test_3002(struct test_result_t *ret)
{
    uint32_t app_id = S_APP_ID;
    struct tfm_sst_buf_t io_data;
    enum psa_sst_err_t err;
    uint32_t itr;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    uint8_t data[BUFFER_PLUS_PADDING_SIZE] = {0};
    uint32_t i;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Sets write and read sizes */
    io_data.size = SST_ASSET_MAX_SIZE_AES_KEY_192;

    for (itr = 0; itr < LOOP_ITERATIONS_002; itr++) {
        TEST_LOG("  > Iteration %d of %d\r", itr + 1, LOOP_ITERATIONS_002);

        /* Checks write permissions in create function */
        err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
        if (err != PSA_SST_ERR_SUCCESS) {
            TEST_FAIL("Create should not fail for application S_APP_ID");
            return;
        }

        memset(data, 0, BUFFER_PLUS_PADDING_SIZE);
        /* Sets data structure */
        io_data.data = data;
        io_data.offset = 0;

        /* Checks write permissions in the write function */
        err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
        if (err != PSA_SST_ERR_SUCCESS) {
            TEST_FAIL("Write should not fail for application S_APP_ID");
            return;
        }

        memset(data, 'X', BUFFER_PLUS_PADDING_SIZE);
        io_data.data = data + HALF_PADDING_SIZE;

        /* Checks write permissions in the read function */
        err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
        if (err != PSA_SST_ERR_SUCCESS) {
            TEST_FAIL("Application S_APP_ID must get file handle");
            return;
        }

        /* Checks read data buffer content */
        if (memcmp(data, "XXXX", HALF_PADDING_SIZE) != 0) {
            TEST_FAIL("Read buffer contains illegal pre-data");
            return;
        }

        for (i=HALF_PADDING_SIZE; i<(BUFFER_PLUS_HALF_PADDING_SIZE); i++) {
            if (data[i] != 0) {
                TEST_FAIL("Read buffer has read incorrect data");
                return;
            }
        }

        if (memcmp((data+BUFFER_PLUS_HALF_PADDING_SIZE), "XXXX",
                    HALF_PADDING_SIZE) != 0) {
            TEST_FAIL("Read buffer contains illegal post-data");
            return;
        }

        /* Checks write permissions in delete function */
        err = tfm_sst_veneer_delete(app_id, asset_uuid, &test_token);
        if (err != PSA_SST_ERR_SUCCESS) {
            TEST_FAIL("Delete should not fail for application S_APP_ID");
            return;
        }
    }

    TEST_LOG("\n");

    ret->val = TEST_PASSED;
}
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */
