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
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "secure_fw/services/secure_storage/sst_object_system.h"
#include "tfm_sst_veneers.h"
#include "s_test_helpers.h"

/* Test suite defines */
#define INVALID_APP_ID         0xFFFFFFFF
#define INVALID_ASSET_ID           0xFFFF
#define READ_BUF_SIZE                14UL
#define WRITE_BUF_SIZE                5UL

/* Memory bounds to check */
#define ROM_ADDR_LOCATION        0x10000000
#define DEV_ADDR_LOCATION        0x50000000
#define NON_EXIST_ADDR_LOCATION  0xFFFFFFFF

/* Test data sized to fill the SHA224 asset */
#define READ_DATA_SHA224    "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define WRITE_DATA_SHA224_1 "TEST_DATA_ONE_TWO_THREE_FOUR"
#define WRITE_DATA_SHA224_2 "(ABCDEFGHIJKLMNOPQRSTUVWXYZ)"
#define BUF_SIZE_SHA224     (SST_ASSET_MAX_SIZE_SHA224_HASH + 1)

/* Define used for bounds checking type tests */
#define BUFFER_SIZE_PLUS_ONE (BUFFER_SIZE + 1)

/* Define default asset's token */
static struct tfm_sst_token_t test_token = { .token = NULL, .token_size = 0};

/* Define test suite for asset manager tests */
/* List of tests */
static void tfm_sst_test_2001(struct test_result_t *ret);
static void tfm_sst_test_2002(struct test_result_t *ret);
static void tfm_sst_test_2003(struct test_result_t *ret);
static void tfm_sst_test_2004(struct test_result_t *ret);
static void tfm_sst_test_2005(struct test_result_t *ret);
static void tfm_sst_test_2006(struct test_result_t *ret);
static void tfm_sst_test_2007(struct test_result_t *ret);
static void tfm_sst_test_2008(struct test_result_t *ret);
static void tfm_sst_test_2009(struct test_result_t *ret);
static void tfm_sst_test_2010(struct test_result_t *ret);
static void tfm_sst_test_2011(struct test_result_t *ret);
static void tfm_sst_test_2012(struct test_result_t *ret);
static void tfm_sst_test_2013(struct test_result_t *ret);
static void tfm_sst_test_2014(struct test_result_t *ret);
static void tfm_sst_test_2015(struct test_result_t *ret);
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
static void tfm_sst_test_2016(struct test_result_t *ret);
static void tfm_sst_test_2017(struct test_result_t *ret);
static void tfm_sst_test_2018(struct test_result_t *ret);
#endif
static void tfm_sst_test_2019(struct test_result_t *ret);
static void tfm_sst_test_2020(struct test_result_t *ret);
static void tfm_sst_test_2021(struct test_result_t *ret);
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
static void tfm_sst_test_2022(struct test_result_t *ret);
#endif

static struct test_t write_tests[] = {
    {&tfm_sst_test_2001, "TFM_SST_TEST_2001",
     "Create interface", {0} },
    {&tfm_sst_test_2002, "TFM_SST_TEST_2002",
     "Get handle interface (DEPRECATED)", {0} },
    {&tfm_sst_test_2003, "TFM_SST_TEST_2003",
     "Get handle with null handle pointer (DEPRECATED)", {0} },
    {&tfm_sst_test_2004, "TFM_SST_TEST_2004",
     "Get information interface", {0} },
    {&tfm_sst_test_2005, "TFM_SST_TEST_2005",
     "Get information with null attributes struct pointer", {0} },
    {&tfm_sst_test_2006, "TFM_SST_TEST_2006",
     "Write interface", {0} },
    {&tfm_sst_test_2007, "TFM_SST_TEST_2007",
     "Write with null buffer pointers", {0} },
    {&tfm_sst_test_2008, "TFM_SST_TEST_2008",
     "Write beyond end of asset", {0} },
    {&tfm_sst_test_2009, "TFM_SST_TEST_2009",
     "Read interface", {0} },
    {&tfm_sst_test_2010, "TFM_SST_TEST_2010",
     "Read with null buffer pointers", {0} },
    {&tfm_sst_test_2011, "TFM_SST_TEST_2011",
     "Read beyond current size of asset", {0} },
    {&tfm_sst_test_2012, "TFM_SST_TEST_2012",
     "Delete interface", {0} },
    {&tfm_sst_test_2013, "TFM_SST_TEST_2013",
     "Write and partial reads", {0} },
    {&tfm_sst_test_2014, "TFM_SST_TEST_2014",
     "Write partial data in an asset and reload secure storage area", {0} },
    {&tfm_sst_test_2015, "TFM_SST_TEST_2015",
     "Write more data than asset max size", {0} },
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
    {&tfm_sst_test_2016, "TFM_SST_TEST_2016",
     "Append data to an asset", {0} },
    {&tfm_sst_test_2017, "TFM_SST_TEST_2017",
     "Append data to an asset until EOF", {0} },
    {&tfm_sst_test_2018, "TFM_SST_TEST_2018",
     "Write data to two assets alternately", {0} },
#endif
    {&tfm_sst_test_2019, "TFM_SST_TEST_2019",
     "Access an illegal location: ROM", {0} },
    {&tfm_sst_test_2020, "TFM_SST_TEST_2020",
     "Access an illegal location: device memory", {0} },
    {&tfm_sst_test_2021, "TFM_SST_TEST_2021",
     "Access an illegal location: non-existant memory", {0} },
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
    {&tfm_sst_test_2022, "TFM_SST_TEST_2022",
     "Write data to the middle of an existing asset", {0} },
#endif
};

void register_testsuite_s_sst_sec_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(write_tests) / sizeof(write_tests[0]));

    set_testsuite("SST secure interface tests (TFM_SST_TEST_2XXX)",
                  write_tests, list_size, p_test_suite);
}

/**
 * \brief Tests create function against:
 * - Valid application ID and asset ID
 * - Invalid asset ID
 * - Invalid application ID
 */
static void tfm_sst_test_2001(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Checks write permissions in create function */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Attempts to create the asset a second time */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Should not fail to create an already-created asset");
        return;
    }

    /* Calls create with invalid asset ID */
    err = tfm_sst_veneer_create(app_id, INVALID_ASSET_ID, &test_token);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should fail for invalid ASSET ID");
        return;
    }

    /* Calls create with invalid application ID */
    err = tfm_sst_veneer_create(INVALID_APP_ID, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should fail for invalid application ID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get handle function against:
 * - Valid asset ID and not created file
 * - Valid asset ID and created file
 * - Invalid app ID
 * - Invalid asset ID
 *
 * \note This test is deprecated and will be removed in next iterations.
 */
static void tfm_sst_test_2002(struct test_result_t *ret)
{
    TEST_LOG("This test is DEPRECATED and the test execution was SKIPPED\r\n");
    ret->val = TEST_PASSED;
}

/**
 * \brief Tests the get handle function with an invalid handle pointer.
 *
 * \note This test is deprecated and will be removed in next iterations.
 */
static void tfm_sst_test_2003(struct test_result_t *ret)
{
    TEST_LOG("This test is DEPRECATED and the test execution was SKIPPED\r\n");
    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get attributes function against:
 * - Valid application ID and attributes struct pointer
 * - Invalid application ID
 * - Invalid asset ID
 */
static void tfm_sst_test_2004(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct tfm_sst_asset_info_t asset_info;
    enum tfm_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Calls get_attributes with valid application ID and
     * attributes struct pointer
     */
    err = tfm_sst_veneer_get_info(app_id, asset_uuid, &test_token, &asset_info);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should be able to read the "
                  "information of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_info.size_current != 0) {
        TEST_FAIL("Asset current size should be 0 as it is only created");
        return;
    }

    if (asset_info.size_max != SST_ASSET_MAX_SIZE_AES_KEY_192) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Calls get_attributes with invalid application ID */
    err = tfm_sst_veneer_get_info(INVALID_APP_ID, asset_uuid, &test_token,
                                  &asset_info);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get information function should fail for an invalid "
                  "application ID");
        return;
    }

    /* Calls get information with invalid asset ID */
    err = tfm_sst_veneer_get_info(app_id, INVALID_ASSET_ID, &test_token,
                                  &asset_info);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get attributes function should fail for an invalid "
                  "asset ID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get attributes function with an invalid attributes struct
 *        pointer.
 */
static void tfm_sst_test_2005(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Calls get information with invalid struct attributes pointer */
    err = tfm_sst_veneer_get_info(app_id, asset_uuid, &test_token, NULL);
    if (err != TFM_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Get information function should fail for an invalid "
                  "struct info pointer");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against:
 * - Valid application ID and data pointer
 * - Invalid application ID
 * - Invalid asset ID
 */
static void tfm_sst_test_2006(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct tfm_sst_asset_info_t asset_info;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Calls get information with valid application ID and
     * attributes struct pointer
     */
    err = tfm_sst_veneer_get_info(app_id, asset_uuid, &test_token, &asset_info);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should be able to read the "
                  "information of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_info.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Asset current size should be size of the write data");
        return;
    }

    /* Calls write function with invalid application ID */
    err = tfm_sst_veneer_write(INVALID_APP_ID, asset_uuid, &test_token,
                               &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Invalid application ID should not write in the file");
        return;
    }

    /* Calls write function with invalid asset ID */
    err = tfm_sst_veneer_write(app_id, INVALID_ASSET_ID, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Invalid asset ID should not write in the file");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function with:
 * - Null tfm_sst_buf_t pointer
 * - Null write buffer pointer
 */
static void tfm_sst_test_2007(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Calls write function with tfm_sst_buf_t pointer set to NULL */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, NULL);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail with tfm_sst_buf_t pointer set to NULL");
        return;
    }

    /* Sets data structure */
    io_data.data = NULL;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write function with data pointer set to NULL */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail with data pointer set to NULL");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function with offset + write data size larger than max
 *        asset size.
 */
static void tfm_sst_test_2008(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t wrt_data[BUFFER_PLUS_PADDING_SIZE] = {0};

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Attempts to write beyond end of asset starting from a valid offset */
    io_data.data = wrt_data;
    io_data.size = BUFFER_SIZE_PLUS_ONE;
    io_data.offset = 0;

    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Writing beyond end of asset should not succeed");
        return;
    }

    /* Attempts to write to an offset beyond the end of the asset */
    io_data.size = 1;
    io_data.offset = SST_ASSET_MAX_SIZE_AES_KEY_192;

    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write to an offset beyond end of asset should not succeed");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read function against:
 * - Valid application ID and data pointer
 * - Invalid application ID
 * - Invalid asset ID
 */
static void tfm_sst_test_2009(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Sets data structure for read*/
    io_data.data = read_data + HALF_PADDING_SIZE;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Read data from the asset */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should work correctly");
        return;
    }

    if (memcmp(read_data, "XXXX", HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read buffer contains illegal pre-data");
        return;
    }

    if (memcmp((read_data+HALF_PADDING_SIZE), wrt_data, WRITE_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    if (memcmp((read_data+HALF_PADDING_SIZE+WRITE_BUF_SIZE), "XXXX",
                HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read buffer contains illegal post-data");
        return;
    }

    /* Calls read with invalid application ID */
    err = tfm_sst_veneer_read(INVALID_APP_ID, asset_uuid, &test_token,
                              &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail when read is called with an invalid "
                  "application ID");
        return;
    }

    /* Calls read with invalid asset ID */
    err = tfm_sst_veneer_read(app_id, INVALID_ASSET_ID, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail when read is called with an invalid "
                  "asset ID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read function with:
 * - Null tfm_sst_buf_t pointer
 * - Null read buffer pointer
 */
static void tfm_sst_test_2010(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Calls read with invalid tfm_sst_buf_t pointer */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, NULL);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read with tfm_sst_buf_t pointer set to NULL should fail");
        return;
    }

    io_data.data = NULL;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls read with invalid data pointer */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read with read data pointer set to NULL should fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read function with offset + read data size larger than current
 *        asset size.
 */
static void tfm_sst_test_2011(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    struct tfm_sst_asset_info_t asset_info;
    uint8_t data[BUFFER_SIZE_PLUS_ONE] = {0};

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = data;
    io_data.size = SST_ASSET_MAX_SIZE_AES_KEY_192;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Gets current asset information */
    err = tfm_sst_veneer_get_info(app_id, asset_uuid, &test_token, &asset_info);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should be able to read the "
                  "information of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_info.size_current == 0) {
        TEST_FAIL("Asset current size should be bigger than 0");
        return;
    }

    /* Attempts to read beyond the current size starting from a valid offset */
    io_data.data = data;
    io_data.size = BUFFER_SIZE_PLUS_ONE;
    io_data.offset = 0;

    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read beyond current size should not succeed");
        return;
    }

    /* Attempts to read from an offset beyond the current size of the asset */
    io_data.size = 1;
    io_data.offset = asset_info.size_current;

    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read from an offset beyond current size should not succeed");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests delete function against:
 * - Valid application ID
 * - Invalid application ID
 * - Invalid asset ID
 * - Remove first asset in the data block and check if
 *   next asset's data is compacted correctly.
 */
static void tfm_sst_test_2012(struct test_result_t *ret)
{
    const uint32_t app_id_1 = S_APP_ID;
    const uint32_t app_id_2 = S_APP_ID;
    const uint32_t asset_uuid_1 =  SST_ASSET_ID_SHA224_HASH;
    const uint32_t asset_uuid_2 = SST_ASSET_ID_SHA384_HASH;
    struct tfm_sst_buf_t io_data;
    enum tfm_sst_err_t err;
    uint8_t read_data[BUF_SIZE_SHA224] = READ_DATA_SHA224;
    uint8_t wrt_data[BUF_SIZE_SHA224] = WRITE_DATA_SHA224_1;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates assset */
    err = tfm_sst_veneer_create(app_id_1, asset_uuid_1, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Calls delete asset with invalid application ID */
    err = tfm_sst_veneer_delete(INVALID_APP_ID, asset_uuid_1, &test_token);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("The delete action should fail if an invalid application "
                  "ID is provided");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_veneer_delete(app_id_1, asset_uuid_1, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    /* Calls delete with a deleted asset ID */
    err = tfm_sst_veneer_delete(app_id_1, asset_uuid_1, &test_token);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("The delete action should fail as ID is not valid");
        return;
    }

    /* Calls delete asset with invalid asset ID */
    err = tfm_sst_veneer_delete(app_id_1, INVALID_ASSET_ID, &test_token);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("The delete action should fail if an invalid asset ID "
                  "is provided");
        return;
    }

    /***** Test data block compact feature *****/
    /* Create asset 2 to locate it at the beginning of the block. Then,
     * create asset 1 to be located after asset 2. Write data on asset
     * 1 and remove asset 2. If delete works correctly, when the code
     * reads back the asset 1 data, the data must be correct.
     */

    /* Creates assset 2 first to locate it at the beginning of the
     * data block
     */
    err = tfm_sst_veneer_create(app_id_2, asset_uuid_2, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Creates asset 1 to locate it after the asset 2 in the data block */
    err = tfm_sst_veneer_create(app_id_1, SST_ASSET_ID_SHA224_HASH,
                                &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH;
    io_data.offset = 0;

    /* Writes data in asset 1 */
    err = tfm_sst_veneer_write(app_id_1, asset_uuid_1, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for application S_APP_ID");
        return;
    }

    /* Deletes asset 2. It means that after the delete call, asset 1 should be
     * at the beginning of the block.
     */
    err = tfm_sst_veneer_delete(app_id_2, asset_uuid_2, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    /* If compact works as expected, the code should be able to read back
     * correctly the data from asset 1
     */

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH;
    io_data.offset = 0;

    /* Read back the asset 1 */
    err = tfm_sst_veneer_read(app_id_1, asset_uuid_1, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, wrt_data, BUF_SIZE_SHA224) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write and partial reads.
 */
static void tfm_sst_test_2013(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint32_t i;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Sets data structure for read */
    io_data.data = read_data + HALF_PADDING_SIZE;
    io_data.size = 1;
    io_data.offset = 0;

    for (i = 0; i < WRITE_BUF_SIZE ; i++) {
        /* Read data from the asset */
        err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
        if (err != TFM_SST_ERR_SUCCESS) {
#else
        if (io_data.offset != 0 && err != TFM_SST_ERR_PARAM_ERROR) {
#endif
            TEST_FAIL("Read did not behave correctly");
            return;
        }

        /* Increases data pointer and offset */
        io_data.data++;
        io_data.offset++;
    }

    if (memcmp(read_data, "XXXX", HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read buffer contains illegal pre-data");
        return;
    }

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
    if (memcmp((read_data+HALF_PADDING_SIZE), wrt_data, WRITE_BUF_SIZE) != 0) {
#else
    /* Only the first read ("D") is from a valid offset (0) */
    if (memcmp((read_data+HALF_PADDING_SIZE), "DXXXX", WRITE_BUF_SIZE) != 0) {
#endif
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    if (memcmp((read_data+HALF_PADDING_SIZE+WRITE_BUF_SIZE), "XXXX",
                HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read buffer contains illegal post-data");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests correct behaviour when data is written in the secure storage
 *        area and the secure_fs_perpare is called after it.
 *        The expected behaviour is to read back the data wrote
 *        before the seconds perpare call.
 */
static void tfm_sst_test_2014(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Calls prepare again to simulate reinitialization */
    err = sst_am_prepare();
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Saved system should have been preparable");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data + HALF_PADDING_SIZE;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Reads back the data after the prepare */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, "XXXX", HALF_PADDING_SIZE)) {
        TEST_FAIL("Read buffer contains illegal pre-data");
        return;
    }

    if (memcmp((read_data+HALF_PADDING_SIZE), wrt_data, WRITE_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    if (memcmp((read_data+HALF_PADDING_SIZE+WRITE_BUF_SIZE), "XXXX",
                HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read buffer contains illegal post-data");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against a write call where data size is
 *        bigger than the maximum assert size.
 */
static void tfm_sst_test_2015(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t wrt_data[BUF_SIZE_SHA224] = {0};

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH + 1;
    io_data.offset = 0;

    /* Writes data in the asset when data size is bigger than asset size */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Should have failed asset write of too large");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
/**
 * \brief Tests write function against multiple writes.
 */
static void tfm_sst_test_2016(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE]  = "XXXXXXXXXXXXX";
    uint8_t wrt_data[WRITE_BUF_SIZE+1]  = "Hello";
    uint8_t wrt_data2[WRITE_BUF_SIZE+1] = "World";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 1 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Writes data 2 in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 2 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = WRITE_BUF_SIZE * 2;
    io_data.offset = 0;

    /* Read back the data */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    /* The X is used to check that the number of bytes read was exactly the
     * number requested
     */
    if (memcmp(read_data, "HelloWorldXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against multiple writes until the end of asset.
 */
static void tfm_sst_test_2017(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t read_data[BUF_SIZE_SHA224] = READ_DATA_SHA224;
    uint8_t wrt_data[BUF_SIZE_SHA224] = WRITE_DATA_SHA224_1;
    uint8_t wrt_data2[BUF_SIZE_SHA224] = WRITE_DATA_SHA224_2;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 1 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = (SST_ASSET_MAX_SIZE_SHA224_HASH - WRITE_BUF_SIZE) + 1;
    io_data.offset = WRITE_BUF_SIZE;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 2 should have failed as this write tries to "
                  "write more bytes than the max size");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data + WRITE_BUF_SIZE;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH - WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Writes data in the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 3 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH;
    io_data.offset = 0;

    /* Read back the data */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, wrt_data, BUF_SIZE_SHA224) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests writing data to two assets alternately before read-back.
 */
static void tfm_sst_test_2018(struct test_result_t *ret)
{

    const uint32_t app_id_1 = S_APP_ID;
    const uint32_t app_id_2 = S_APP_ID;
    const uint32_t asset_uuid_1 = SST_ASSET_ID_AES_KEY_192;
    const uint32_t asset_uuid_2 = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";
    uint8_t wrt_data[WRITE_BUF_SIZE+1] = "Hello";
    uint8_t wrt_data2[3] = "Hi";
    uint8_t wrt_data3[WRITE_BUF_SIZE+1] = "World";
    uint8_t wrt_data4[WRITE_BUF_SIZE+1] = "12345";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset 1 */
    err = tfm_sst_veneer_create(app_id_1, asset_uuid_1, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Creates asset 2 */
    err = tfm_sst_veneer_create(app_id_2, asset_uuid_2, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in asset 1 */
    err = tfm_sst_veneer_write(app_id_1, asset_uuid_1, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = 2;
    io_data.offset = 0;

    /* Writes data 2 in asset 2 */
    err = tfm_sst_veneer_write(app_id_2, asset_uuid_2, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data3;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Writes data 3 in asset 1 */
    err = tfm_sst_veneer_write(app_id_1, asset_uuid_1, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data4;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 2;

    /* Writes data 4 in asset 2 */
    err = tfm_sst_veneer_write(app_id_2, asset_uuid_2, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = WRITE_BUF_SIZE * 2; /* size of wrt_data + wrt_data3 */
    io_data.offset = 0;

    /* Read back the asset 1 */
    err = tfm_sst_veneer_read(app_id_1, asset_uuid_1, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, "HelloWorldXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Resets read buffer content to a known data */
    memset(read_data, 'X', READ_BUF_SIZE - 1);

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = 2 + WRITE_BUF_SIZE; /* size of wrt_data2 + wrt_data4 */
    io_data.offset = 0;

    /* Read back the asset 2 */
    err = tfm_sst_veneer_read(app_id_2, asset_uuid_2, &test_token, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, "Hi12345XXXXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    ret->val = TEST_PASSED;
}
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

/**
 * \brief Tests read from and write to an illegal location: ROM.
 */
static void tfm_sst_test_2019(struct test_result_t *ret)
{
    uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)ROM_ADDR_LOCATION;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write with a ROM address location */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a ROM address location */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read from and write to an illegal location: device memory.
 */
static void tfm_sst_test_2020(struct test_result_t *ret)
{
    uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)DEV_ADDR_LOCATION;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write with a device address location */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a device address location */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read from and write to an illegal location: non-existant memory.
 */
static void tfm_sst_test_2021(struct test_result_t *ret)
{
    uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    /* Creates asset */
    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)NON_EXIST_ADDR_LOCATION;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write with a non-existing address location */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a non-existing address location */
    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
/**
 * \brief Writes data to the middle of an existing asset.
 */
static void tfm_sst_test_2022(struct test_result_t *ret)
{
    const uint32_t app_id = S_APP_ID;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct tfm_sst_asset_info_t asset_info;
    struct tfm_sst_buf_t buf;
    enum tfm_sst_err_t err;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";
    uint8_t write_data_1[WRITE_BUF_SIZE] = "AAAA";
    uint8_t write_data_2[2] = "B";

    if (prepare_test_ctx(ret) != 0) {
        TEST_FAIL("Prepare test context should not fail");
        return;
    }

    err = tfm_sst_veneer_create(app_id, asset_uuid, &test_token);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    buf.data = write_data_1;
    buf.size = (WRITE_BUF_SIZE - 1);
    buf.offset = 0;

    /* Writes write_data_1 to the asset */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &buf);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("First write should not fail");
        return;
    }

    err = tfm_sst_veneer_get_info(app_id, asset_uuid, &test_token, &asset_info);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get information should not fail");
        return;
    }

    /* Checks that the asset's current size is equal to the size of the write
     * data.
     */
    if (asset_info.size_current != WRITE_BUF_SIZE - 1) {
        TEST_FAIL("Current size should be equal to write size");
        return;
    }

    buf.data = write_data_2;
    buf.size = 1;
    buf.offset = 1;

    /* Overwrites the second character in the asset with write_data_2 */
    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &buf);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Second write should not fail");
        return;
    }

    err = tfm_sst_veneer_get_info(app_id, asset_uuid, &test_token, &asset_info);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get information should not fail");
        return;
    }

    /* Checks that the asset's current size has not changed */
    if (asset_info.size_current != (WRITE_BUF_SIZE - 1)) {
        TEST_FAIL("Current size should not have changed");
        return;
    }

    buf.data = (read_data + HALF_PADDING_SIZE);
    buf.size = (WRITE_BUF_SIZE - 1);
    buf.offset = 0;

    err = tfm_sst_veneer_read(app_id, asset_uuid, &test_token, &buf);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    /* Checks that the asset contains write_data_1 with the second character
     * overwritten with write_data_2.
     */
    if (memcmp(read_data, "XXXXABAAXXXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer is incorrect");
        return;
    }

    /* Checks that offset can not be bigger than current asset's size */
    buf.data = write_data_2;
    buf.size = 1;
    buf.offset = (asset_info.size_current + 1);

    err = tfm_sst_veneer_write(app_id, asset_uuid, &test_token, &buf);
    if (err != TFM_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Write must fail if the offset is bigger than the current"
                  " asset's size");
        return;
    }

    ret->val = TEST_PASSED;
}
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */
