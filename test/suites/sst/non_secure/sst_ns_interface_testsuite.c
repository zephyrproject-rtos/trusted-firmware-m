/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_ns_tests.h"

#include <stdio.h>
#include <string.h>

#include "ns_test_helpers.h"
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "test/framework/helpers.h"
#include "tfm_sst_api.h"

/* Test suite defines */
#define INVALID_ASSET_ID           0xFFFF
#define READ_BUF_SIZE                12UL
#define WRITE_BUF_SIZE                5UL

/* Memory bounds to check */
#define ROM_ADDR_LOCATION        0x00000000
#define DEV_ADDR_LOCATION        0x40000000
#define SECURE_ADDR_LOCATION     0x30000000
#define NON_EXIST_ADDR_LOCATION  0xFFFFFFFF

/* Test data sized to fill the SHA224 asset */
#define READ_DATA_SHA224    "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define WRITE_DATA_SHA224_1 "TEST_DATA_ONE_TWO_THREE_FOUR"
#define WRITE_DATA_SHA224_2 "(ABCDEFGHIJKLMNOPQRSTUVWXYZ)"
#define BUF_SIZE_SHA224     (SST_ASSET_MAX_SIZE_SHA224_HASH + 1)

/* Shared asset handles for multithreaded tests */
static uint32_t tfm_sst_test_1004_handle;
static uint32_t tfm_sst_test_1006_handle;
static uint32_t tfm_sst_test_1008_handle;
static uint32_t tfm_sst_test_1017_asset1_handle;
static uint32_t tfm_sst_test_1017_asset2_handle;

/* Define test suite for asset manager tests */
/* List of tests */
static void tfm_sst_test_1001(struct test_result_t *ret);
static void tfm_sst_test_1002(struct test_result_t *ret);
static void tfm_sst_test_1003(struct test_result_t *ret);
static void tfm_sst_test_1004(struct test_result_t *ret);
static void tfm_sst_test_1005(struct test_result_t *ret);
static void tfm_sst_test_1006(struct test_result_t *ret);
static void tfm_sst_test_1007(struct test_result_t *ret);
static void tfm_sst_test_1008(struct test_result_t *ret);
static void tfm_sst_test_1009(struct test_result_t *ret);
static void tfm_sst_test_1010(struct test_result_t *ret);
static void tfm_sst_test_1011(struct test_result_t *ret);
static void tfm_sst_test_1012(struct test_result_t *ret);
static void tfm_sst_test_1013(struct test_result_t *ret);
static void tfm_sst_test_1014(struct test_result_t *ret);
static void tfm_sst_test_1015(struct test_result_t *ret);
static void tfm_sst_test_1016(struct test_result_t *ret);
static void tfm_sst_test_1017(struct test_result_t *ret);
static void tfm_sst_test_1018(struct test_result_t *ret);
static void tfm_sst_test_1019(struct test_result_t *ret);

static struct test_t asset_veeners_tests[] = {
    {&tfm_sst_test_1001, "TFM_SST_TEST_1001",
     "Create interface from allowed appid", {0} },
    {&tfm_sst_test_1002, "TFM_SST_TEST_1002",
     "Create interface from non allowed appid", {0} },
    {&tfm_sst_test_1003, "TFM_SST_TEST_1003",
     "Get handle interface", {0} },
    {&tfm_sst_test_1004, "TFM_SST_TEST_1004",
     "Get attributes interface",  {0} },
    {&tfm_sst_test_1005, "TFM_SST_TEST_1005",
     "Get attributes interface from non authorized task",  {0} },
    {&tfm_sst_test_1006, "TFM_SST_TEST_1006",
     "Write interface", {0} },
    {&tfm_sst_test_1007, "TFM_SST_TEST_1007",
     "Write interface from non authorized task", {0} },
    {&tfm_sst_test_1008, "TFM_SST_TEST_1008",
     "Read interface", {0} },
    {&tfm_sst_test_1009, "TFM_SST_TEST_1009",
     "Read interface from non authorized task", {0} },
    {&tfm_sst_test_1010, "TFM_SST_TEST_1010",
     "Read interface test cleanup", {0} },
    {&tfm_sst_test_1011, "TFM_SST_TEST_1011",
     "Delete interface", {0} },
    {&tfm_sst_test_1012, "TFM_SST_TEST_1012",
     "Delete interface multiple from multiple contexts", {0} },
    {&tfm_sst_test_1013, "TFM_SST_TEST_1013",
     "Write and partial reads", {0} },
    {&tfm_sst_test_1014, "TFM_SST_TEST_1014",
     "Write more data than asset max size", {0} },
    {&tfm_sst_test_1015, "TFM_SST_TEST_1015",
     "Appending data to an asset", {0} },
    {&tfm_sst_test_1016, "TFM_SST_TEST_1016",
     "Appending data to an asset until eof", {0} },
    {&tfm_sst_test_1017, "TFM_SST_TEST_1017",
     "Write data to two assets alternately", {0} },
    {&tfm_sst_test_1018, "TFM_SST_TEST_1018",
     "Write and read data from illegal locations", {0} },
    {&tfm_sst_test_1019, "TFM_SST_TEST_1019",
     "Write data to the middle of an existing asset", {0} },
};

void register_testsuite_ns_sst_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(asset_veeners_tests) / sizeof(asset_veeners_tests[0]));

    set_testsuite("SST non-secure interface tests (TFM_SST_TEST_1XXX)",
                  asset_veeners_tests, list_size, p_test_suite);
}

/**
 * \note List of relations between thread name, app ID and permissions
 *
 * Asset permissions: SST_ASSET_ID_X509_CERT_LARGE
 *
 *   THREAD NAME | APP_ID       | Permissions
 *   ------------|--------------------------------------
 *     Thread_A  | SST_APP_ID_0 | REFERENCE
 *     Thread_B  | SST_APP_ID_1 | REFERENCE, READ
 *     Thread_C  | SST_APP_ID_2 | REFERENCE, READ, WRITE
 *
 * Asset permissions: SST_ASSET_ID_SHA224_HASH
 *
 *   THREAD NAME | APP_ID       | Permissions
 *   ------------|--------------------------------------
 *     Thread_A  | SST_APP_ID_0 | NONE
 *     Thread_B  | SST_APP_ID_1 | REFERENCE, READ, WRITE
 *     Thread_C  | SST_APP_ID_2 | NONE
 *
 * Asset permissions: SST_ASSET_ID_SHA384_HASH
 *
 *   THREAD NAME | APP_ID       | Permissions
 *   ------------|--------------------------------
 *     Thread_A  | SST_APP_ID_0 | NONE
 *     Thread_B  | SST_APP_ID_1 | NONE
 *     Thread_C  | SST_APP_ID_2 | REFERENCE, WRITE
 */

/**
 * \brief Tests create function against:
 * - Valid application ID and asset ID
 * - Invalid asset ID
 * - Invalid application ID
 */
TFM_SST_NS_TEST(1001, "Thread_C")
{
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    enum tfm_sst_err_t err;
    uint32_t hdl;

    /* Checks write permissions in create function */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Calls create with invalid asset ID */
    err = tfm_sst_create(INVALID_ASSET_ID);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should fail for invalid ASSET ID");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

TFM_SST_NS_TEST(1002, "Thread_A")
{
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    enum tfm_sst_err_t err;

    /* Calls create with invalid application ID */
    ret->val = TEST_PASSED;
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should fail for thread without write permissions");
    }
}

/**
 * \brief Tests get handle function against:
 * - Valid asset ID and not created asset
 * - Valid asset ID and created asset
 * - Invalid asset ID
 */
TFM_SST_NS_TEST(1003, "Thread_C")
{
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    enum tfm_sst_err_t err;
    uint32_t hdl;

    /* Calls get handle before create the asset */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should fail as the asset is not created");
        return;
    }

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    /* Calls get handle with a valid asset ID and the asset created */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Calls get handle with invalid asset ID */
    err = tfm_sst_get_handle(INVALID_ASSET_ID, &hdl);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should fail as asset hanlde is invalid");
        return;
    }

    /* Calls get handle with invalid handle pointer */
    err = tfm_sst_get_handle(asset_uuid, NULL);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should fail as handle pointer is invalid");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get attributes function against:
 * - Valid application ID, asset handle and attributes struct pointer
 * - Invalid application ID
 * - Invalid asset handle
 * - Invalid attributes struct pointer
 */
TFM_SST_NS_TEST(1004, "Thread_C")
{
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    struct tfm_sst_attribs_t asset_attrs;
    enum tfm_sst_err_t err;
    uint32_t hdl;

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Make handle available to the next test */
    tfm_sst_test_1004_handle = hdl;

    /* Calls get_attributes with valid application ID, asset handle and
     * attributes struct pointer
     */
    err = tfm_sst_get_attributes(hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_C should read the attributes of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current != 0) {
        TEST_FAIL("Asset current size should be 0 as it is only created");
        return;
    }

    if (asset_attrs.size_max != SST_ASSET_MAX_SIZE_X509_CERT_LARGE) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Calls get_attributes with invalid asset handle */
    err = tfm_sst_get_attributes(0, &asset_attrs);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get attributes function should fail for an invalid "
                  "asset handle");
        return;
    }

    /* Calls get_attributes with invalid struct attributes pointer */
    err = tfm_sst_get_attributes(hdl, NULL);
    if (err != TFM_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Get attributes function should fail for an invalid "
                  "struct attributes pointer");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

TFM_SST_NS_TEST(1005, "Thread_INVALID")
{
    struct tfm_sst_attribs_t asset_attrs;
    enum tfm_sst_err_t err;

    ret->val = TEST_PASSED;
    /* Calls get_attributes with invalid application ID */
    err = tfm_sst_get_attributes(tfm_sst_test_1004_handle, &asset_attrs);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get attributes function should fail for an invalid "
                  "application ID");
    }
}

/**
 * \brief Tests write function against:
 * - Valid application ID, asset handle and data pointer
 * - Invalid application ID
 * - Invalid asset handle
 * - NULL pointer as write buffer
 * - Offet + write data size larger than max asset size
 */
/* handle to be shared between tasks for testing write api
 * from non authorised task with valid handle
 */
TFM_SST_NS_TEST(1006, "Thread_C")
{
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    struct tfm_sst_attribs_t asset_attrs;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint32_t hdl;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Make handle available to the next test */
    tfm_sst_test_1006_handle = hdl;

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should works correctly");
        return;
    }

    /* Calls get_attributes with valid application ID, asset handle and
     * attributes struct pointer
     */
    err = tfm_sst_get_attributes(hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_C should read the attributes of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Asset current size should be size of the write data");
        return;
    }

    /* Calls write function with invalid asset handle */
    err = tfm_sst_write(0, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Invalid asset handle should not write in the asset");
        return;
    }

    /* Calls write function with invalid asset handle */
    err = tfm_sst_write(hdl, NULL);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("NULL data pointer should make the write fail");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = 1;
    io_data.offset = SST_ASSET_MAX_SIZE_X509_CERT_LARGE;

    /* Calls write function with offset + write data size larger than
     * max asset size
     */
    err = tfm_sst_write(hdl, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Offset + write data size larger than max asset size "
                  "should make the write fail");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

TFM_SST_NS_TEST(1007, "Thread_A")
{
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;

    ret->val = TEST_PASSED;
    /* Calls write function with invalid application ID */
    err = tfm_sst_write(tfm_sst_test_1006_handle, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Thread_A should not write in the asset as it doesn't have"
                  " write permissions");
    }
}

/**
 * \brief Tests read function against:
 * - Valid application ID, asset handle and data pointer
 * - Invalid application ID
 * - Invalid asset handle
 * - NULL pointer as write buffer
 * - Offet + read data size larger than current asset size
 */
TFM_SST_NS_TEST(1008, "Thread_C")
{
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    struct tfm_sst_attribs_t asset_attrs;
    uint32_t hdl;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXX";

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Make handle available to the next test */
    tfm_sst_test_1008_handle = hdl;

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Sets data structure for read*/
    io_data.data = read_data+3;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Read data from the asset */
    err = tfm_sst_read(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should works correctly");
        return;
    }

    if (memcmp(read_data, "XXX", 3) != 0) {
        TEST_FAIL("Read buffer contains illegal pre-data");
        return;
    }

    if (memcmp((read_data+3), wrt_data, WRITE_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    if (memcmp((read_data+8), "XXX", 3) != 0) {
        TEST_FAIL("Read buffer contains illegal post-data");
        return;
    }

    /* Calls read with invalid asset handle */
    err = tfm_sst_read(0, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail when read is called with an invalid "
                  "asset handle");
        return;
    }

    /* Calls read with invalid asset handle */
    err = tfm_sst_read(hdl, NULL);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail when read is called with an invalid "
                  "data pointer");
        return;
    }

    /* Gets current asset attributes */
    err = tfm_sst_get_attributes(hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_C should read the attributes of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current == 0) {
        TEST_FAIL("Asset current size should be bigger than 0");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = 1;
    io_data.offset = asset_attrs.size_current;

    /* Calls write function with offset + read data size larger than current
     * asset size
     */
    err = tfm_sst_read(hdl, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Offset + read data size larger than current asset size");
        return;
    }

    ret->val = TEST_PASSED;
}

TFM_SST_NS_TEST(1009, "Thread_A")
{
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXX";

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = 1;
    io_data.offset = 0;

    ret->val = TEST_PASSED;
    /* Calls read with invalid application ID */
    err = tfm_sst_read(tfm_sst_test_1008_handle, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail when read is called from thread without "
                  "read permissions");
    }
}

TFM_SST_NS_TEST(1010, "Thread_C")
{
    enum tfm_sst_err_t err;

    ret->val = TEST_PASSED;
    /* Calls delete asset to clean up SST area for next test */
    err = tfm_sst_delete(tfm_sst_test_1008_handle);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
    }
}

/**
 * \brief Tests delete function against:
 * - Valid application ID and asset handle
 * - Invalid application ID
 * - Invalid asset handle
 * - Remove first asset in the data block and check if
 *   next asset's data is compacted correctly.
 */
TFM_SST_NS_TEST(1011, "Thread_B")
{
    const uint16_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    uint32_t hdl;

    ret->val = TEST_PASSED;
    /* Creates assset 1 to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Gets asset handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should not fail as thread has"
                  " write permissions");
        return;
    }

    /* Calls delete with a deleted asset handle */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("The delete action should fail as handle is not valid");
        return;
    }

    /* Calls delete asset with invalid asset handle */
    err = tfm_sst_delete(0);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("The delete action should fail handle is not valid");
    }
}

/**
 * \brief Test data block compact feature.
 *        Create asset 2 to locate it at the beginning of the block. Then,
 *        create asset 1 to be located after asset 2. Write data on asset
 *        1 and remove asset 2. If delete works correctly, when the code
 *        reads back the asset 1 data, the data must be correct.
 */
static void tfm_sst_test_1012_part1_task(struct test_result_t *ret)
{
    const uint16_t asset_uuid_2 = SST_ASSET_ID_SHA384_HASH;
    enum tfm_sst_err_t err;
    uint32_t hdl_2;

    ret->val = TEST_PASSED;

    /* Create asset 2 first to locate it at the beginning of the data block */
    err = tfm_sst_create(asset_uuid_2);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    /* Gets asset 2 handle */
    err = tfm_sst_get_handle(asset_uuid_2, &hdl_2);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
    }
}

static void tfm_sst_test_1012_part2_task(struct test_result_t *ret)
{
    const uint16_t asset_uuid_1 = SST_ASSET_ID_SHA224_HASH;
    struct tfm_sst_buf_t io_data;
    enum tfm_sst_err_t err;
    uint32_t hdl_1;
    uint8_t wrt_data[BUF_SIZE_SHA224] = WRITE_DATA_SHA224_1;

    ret->val = TEST_PASSED;

    /* Creates asset 1 handle */
    err = tfm_sst_create(asset_uuid_1);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Gets asset 1 handle */
    err = tfm_sst_get_handle(asset_uuid_1, &hdl_1);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH;
    io_data.offset = 0;

    /* Calls write function  */
    err = tfm_sst_write(hdl_1, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for Thread_B");
    }

}

static void tfm_sst_test_1012_part3_task(struct test_result_t *ret)
{
    const uint16_t asset_uuid_2 = SST_ASSET_ID_SHA384_HASH;
    enum tfm_sst_err_t err;
    uint32_t hdl_2;

    ret->val = TEST_PASSED;

    err = tfm_sst_get_handle(asset_uuid_2, &hdl_2);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_delete(hdl_2);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
    }
}

static void tfm_sst_test_1012_part4_task(struct test_result_t *ret)
{
    const uint16_t asset_uuid_1 = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    uint32_t hdl_1;
    struct tfm_sst_buf_t io_data;
    uint8_t read_data[BUF_SIZE_SHA224] = READ_DATA_SHA224;
    uint8_t wrt_data[BUF_SIZE_SHA224] = WRITE_DATA_SHA224_1;

    ret->val = TEST_PASSED;

    err = tfm_sst_get_handle(asset_uuid_1, &hdl_1);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH;
    io_data.offset = 0;

    err = tfm_sst_read(hdl_1, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for Thread_B");
        return;
    }

    if (memcmp(read_data, wrt_data, BUF_SIZE_SHA224) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_delete(hdl_1);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
    }
}

/**
 * \brief Tests write and partial reads.
 */
TFM_SST_NS_TEST(1013, "Thread_C")
{
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint32_t hdl;
    uint32_t i;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXX";
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should works correctly");
        return;
    }

    /* Sets data structure for read*/
    io_data.data = (read_data + 3);
    io_data.size = 1;
    io_data.offset = 0;

    for (i = 0; i < WRITE_BUF_SIZE; i++) {
        /* Read data from the asset */
        err = tfm_sst_read(hdl, &io_data);
        if (err != TFM_SST_ERR_SUCCESS) {
            TEST_FAIL("Read should works correctly");
            return;
        }

        /* Increases data pointer and offset */
        io_data.data++;
        io_data.offset++;
    }

    if (memcmp(read_data, "XXX", 3) != 0) {
        TEST_FAIL("Read buffer contains illegal pre-data");
        return;
    }

    if (memcmp((read_data + 3), wrt_data, WRITE_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    if (memcmp((read_data + 8), "XXX", 3) != 0) {
        TEST_FAIL("Read buffer contains illegal post-data");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against a write call where data size is
 *        bigger than the maximum assert size.
 */
TFM_SST_NS_TEST(1014, "Thread_B")
{
    const uint16_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint32_t hdl;
    uint8_t wrt_data[BUF_SIZE_SHA224] = {0};

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH + 1;
    io_data.offset = 0;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Should have failed asset write of too large");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against multiple writes.
 */
TFM_SST_NS_TEST(1015, "Thread_B")
{
    const uint16_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint32_t hdl;
    uint8_t read_data[READ_BUF_SIZE]  = "XXXXXXXXXXX";
    uint8_t wrt_data[WRITE_BUF_SIZE+1]  = "Hello";
    uint8_t wrt_data2[WRITE_BUF_SIZE+1] = "World";

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 1 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Write data 2 in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 2 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = WRITE_BUF_SIZE * 2;
    io_data.offset = 0;

    /* Read back the data */
    err = tfm_sst_read(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, "HelloWorldX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against multiple writes until the end of asset.
 */
TFM_SST_NS_TEST(1016, "Thread_B")
{
    const uint16_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint32_t hdl;
    uint8_t read_data[BUF_SIZE_SHA224] = READ_DATA_SHA224;
    uint8_t wrt_data[BUF_SIZE_SHA224] = WRITE_DATA_SHA224_1;
    uint8_t wrt_data2[BUF_SIZE_SHA224] = WRITE_DATA_SHA224_2;

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 1 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = (SST_ASSET_MAX_SIZE_SHA224_HASH - WRITE_BUF_SIZE) + 1;
    io_data.offset = WRITE_BUF_SIZE;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 2 should have failed as this write tries to "
                  "write more bytes that the max size");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data + WRITE_BUF_SIZE;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH - WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 3 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH;
    io_data.offset = 0;

    /* Read back the data */
    err = tfm_sst_read(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, wrt_data, BUF_SIZE_SHA224) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_delete(hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write and read to/from 2 assets.
 */
static void tfm_sst_test_1017_part1_task(struct test_result_t *ret)
{
    const uint16_t asset_uuid_1 = SST_ASSET_ID_X509_CERT_LARGE;
    enum tfm_sst_err_t err;
    uint32_t hdl_1;

    /* Creates asset 1 to get a valid handle */
    err = tfm_sst_create(asset_uuid_1);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    /* Gets asset's handle 1 */
    err = tfm_sst_get_handle(asset_uuid_1, &hdl_1);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    tfm_sst_test_1017_asset1_handle = hdl_1;

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1017_part2_task(struct test_result_t *ret)
{
    const uint16_t asset_uuid_2 = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    uint32_t hdl_2;

    /* Creates asset 2 to get a valid handle */
    err = tfm_sst_create(asset_uuid_2);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Gets asset's handle 2*/
    err = tfm_sst_get_handle(asset_uuid_2, &hdl_2);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    tfm_sst_test_1017_asset2_handle = hdl_2;

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1017_part3_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t wrt_data[WRITE_BUF_SIZE+1]  = "Hello";

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Write data in asset 1 */
    err = tfm_sst_write(tfm_sst_test_1017_asset1_handle, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for Thread_C");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1017_part4_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t wrt_data2[3] = "Hi";

    /* Write data 2 in asset 2 */
    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = 2;
    io_data.offset = 0;

    err = tfm_sst_write(tfm_sst_test_1017_asset2_handle, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1017_part5_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t wrt_data3[WRITE_BUF_SIZE+1] = "World";

    /* Sets data structure */
    io_data.data = wrt_data3;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Write data 3 in asset 1 */
    err = tfm_sst_write(tfm_sst_test_1017_asset1_handle, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for Thread_C");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1017_part6_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t wrt_data4[WRITE_BUF_SIZE+1] = "12345";

    /* Sets data structure */
    io_data.data = wrt_data4;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 2;

    /* Write data 4 in asset 2 */
    err = tfm_sst_write(tfm_sst_test_1017_asset2_handle, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1017_part7_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE]  = "XXXXXXXXXXX";

    /* Sets data structure */
    io_data.data = read_data;
    /* size of wrt_data + wrt_data3 */
    io_data.size = WRITE_BUF_SIZE + WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Read back the asset 1 */
    err = tfm_sst_read(tfm_sst_test_1017_asset1_handle, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for Thread_C");
        return;
    }

    if (memcmp(read_data, "HelloWorldX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1017_part8_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE]  = "XXXXXXXXXXX";

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = 2 + WRITE_BUF_SIZE; /* size of wrt_data2 + wrt_data4 */
    io_data.offset = 0;

    /* Read back the asset 1 */
    err = tfm_sst_read(tfm_sst_test_1017_asset2_handle, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, "Hi12345XXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Calls delete asset */
    err = tfm_sst_delete(tfm_sst_test_1017_asset2_handle);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1017_part9_task(struct test_result_t *ret)
{
    enum tfm_sst_err_t err;

    /* Calls delete asset 1 */
    err = tfm_sst_delete(tfm_sst_test_1017_asset1_handle);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write and read to/from the follow illegal locations:
 * - ROM memory
 * - Device memory
 * - Secure memory
 * - Non existing memory location
 */
TFM_SST_NS_TEST(1018, "Thread_B")
{
    const uint16_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint32_t hdl;

    /* Creates asset to get a valid handle */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /*** Check functions against ROM address location ***/

    /* Gets asset's handle with a ROM address location to store asset's
     * handle
     */
    err = tfm_sst_get_handle(asset_uuid, (uint32_t *)ROM_ADDR_LOCATION);
    if (err != TFM_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Get handle should fail for an illegal location");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)ROM_ADDR_LOCATION;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write with a ROM address location */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a ROM address location */
    err = tfm_sst_read(hdl, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    /*** Check functions against devices address location ***/

    /* Gets asset's handle with a devices address location to store asset's
     * handle
     */
    err = tfm_sst_get_handle(asset_uuid, (uint32_t *)DEV_ADDR_LOCATION);
    if (err != TFM_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Get handle should fail for an illegal location");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)DEV_ADDR_LOCATION;

    /* Calls write with a device address location */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a device address location */
    err = tfm_sst_read(hdl, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    /*** Check functions against secure address location ***/

    /* Gets asset's handle with a secure address location to store asset's
     * handle
     */
    err = tfm_sst_get_handle(asset_uuid,
                             (uint32_t *)SECURE_ADDR_LOCATION);
    if (err != TFM_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Get handle should fail for an illegal location");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)SECURE_ADDR_LOCATION;

    /* Calls write with a secure address location */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a secure address location */
    err = tfm_sst_read(hdl, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    /*** Check functions against non existing address location ***/

    /* Gets asset's handle with a non existing address location to store asset's
     * handle
     */
    err = tfm_sst_get_handle(asset_uuid,
                             (uint32_t *)NON_EXIST_ADDR_LOCATION);
    if (err != TFM_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Get handle should fail for an illegal location");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)NON_EXIST_ADDR_LOCATION;

    /* Calls write with a non-existing address location */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a non-existing address location */
    err = tfm_sst_read(hdl, &io_data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write data to the middle of an existing asset
 */
TFM_SST_NS_TEST(1019, "Thread_C")
{
    struct tfm_sst_attribs_t asset_attrs;
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    enum tfm_sst_err_t err;
    struct tfm_sst_buf_t io_data;
    uint32_t hdl;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXX";
    uint8_t wrt_data_1[WRITE_BUF_SIZE] = "AAAA";
    uint8_t wrt_data_2[2] = "B";

    /* Creates asset */
    err = tfm_sst_create(asset_uuid);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_C");
        return;
    }

    /* Gets asset's handle */
    err = tfm_sst_get_handle(asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should return a valid asset handle");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data_1;
    io_data.size = (WRITE_BUF_SIZE - 1);
    io_data.offset = 0;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("First write should not fail");
        return;
    }

    err = tfm_sst_get_attributes(hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_C should read the attributes of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current != (WRITE_BUF_SIZE - 1)) {
        TEST_FAIL("Current size should be equal to write size");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data_2;
    io_data.size = 1;
    io_data.offset = 1;

    /* Write data in the asset */
    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Second write should not fail");
        return;
    }

    err = tfm_sst_get_attributes(hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_C should read the attributes of this asset");
        return;
    }

    /* Checks that the asset's current size has not changed */
    if (asset_attrs.size_current != (WRITE_BUF_SIZE - 1)) {
        TEST_FAIL("Current size should not have changed");
        return;
    }

    io_data.data = (read_data + 3);
    io_data.size = (WRITE_BUF_SIZE - 1);
    io_data.offset = 0;

    /* Calls read with a non-existing address location */
    err = tfm_sst_read(hdl, &io_data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail");
        return;
    }

    /* Checks that the asset contains write_data_1 with the second character
     * overwritten with write_data_2.
     */
    if (memcmp(read_data, "XXXABAAXXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer is incorrect");
        return;
    }

    /* Checks that offset can not be bigger than current asset's size */
    io_data.data = wrt_data_2;
    io_data.size = 1;
    io_data.offset = (asset_attrs.size_current + 1);

    err = tfm_sst_write(hdl, &io_data);
    if (err != TFM_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Write must fail if the offset is bigger than the current"
                  " asset's size");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1012(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_C", ret, tfm_sst_test_1012_part1_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1012_part2_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_C", ret, tfm_sst_test_1012_part3_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1012_part4_task);
}

static void tfm_sst_test_1017(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_C", ret, tfm_sst_test_1017_part1_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1017_part2_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_C", ret, tfm_sst_test_1017_part3_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1017_part4_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_C", ret, tfm_sst_test_1017_part5_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1017_part6_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_C", ret, tfm_sst_test_1017_part7_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1017_part8_task);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_C", ret, tfm_sst_test_1017_part9_task);
}
