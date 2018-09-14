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
#include "test/framework/test_framework_helpers.h"
#include "psa_sst_api.h"

/* Test suite defines */
#define INVALID_ASSET_ID             0xFFFF
#define INVALID_THREAD_NAME "Thread_INVALID"

#define READ_BUF_SIZE                  14UL
#define WRITE_BUF_SIZE                  5UL

/* Define default asset's token */
#define ASSET_TOKEN      NULL
#define ASSET_TOKEN_SIZE 0

/* Memory bounds to check */
#define ROM_ADDR_LOCATION        0x00000000
#define DEV_ADDR_LOCATION        0x40000000
#define SECURE_ADDR_LOCATION     0x30000000
#define NON_EXIST_ADDR_LOCATION  0xFFFFFFFF

/* Test data sized to fill the SHA224 and AES KEY 192 assets */
#define READ_DATA_SHA224  "XXXXXXXXXXXXXXXXXXXXXXXXXXXX"
#define WRITE_DATA_SHA224 "TEST_DATA_ONE_TWO_THREE_FOUR"
#define BUF_SIZE_SHA224   (SST_ASSET_MAX_SIZE_SHA224_HASH + 1)

#define BUF_SIZE_AES_KEY_192     (SST_ASSET_MAX_SIZE_AES_KEY_192 + 1)
#define READ_DATA_AES_KEY_192    "XXXXXXXXXXXXXXXXXXXXXXXX"
#define WRITE_DATA_AES_KEY_192_1 "TEST_DATA_ONE_TWO_THREE_"
#define WRITE_DATA_AES_KEY_192_2 "ABCDEFGHIJKLMNOPQRSTUVWY"

/* Define used for bounds checking type tests */
#define BUFFER_SIZE_PLUS_ONE (BUFFER_SIZE + 1)

/* Define test suite for asset manager tests */
/* List of tests */
static void tfm_sst_test_1001(struct test_result_t *ret);
static void tfm_sst_test_1002(struct test_result_t *ret);

#ifdef TFM_NS_CLIENT_IDENTIFICATION
static void tfm_sst_test_1003(struct test_result_t *ret);
static void tfm_sst_test_1004(struct test_result_t *ret);
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

static void tfm_sst_test_1005(struct test_result_t *ret);
static void tfm_sst_test_1006(struct test_result_t *ret);
static void tfm_sst_test_1007(struct test_result_t *ret);
static void tfm_sst_test_1008(struct test_result_t *ret);
static void tfm_sst_test_1009(struct test_result_t *ret);
static void tfm_sst_test_1010(struct test_result_t *ret);

#ifdef TFM_NS_CLIENT_IDENTIFICATION
static void tfm_sst_test_1011(struct test_result_t *ret);
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

static void tfm_sst_test_1012(struct test_result_t *ret);
static void tfm_sst_test_1013(struct test_result_t *ret);
static void tfm_sst_test_1014(struct test_result_t *ret);

#ifdef TFM_NS_CLIENT_IDENTIFICATION
static void tfm_sst_test_1015(struct test_result_t *ret);
static void tfm_sst_test_1016(struct test_result_t *ret);
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

static void tfm_sst_test_1017(struct test_result_t *ret);
static void tfm_sst_test_1018(struct test_result_t *ret);

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
static void tfm_sst_test_1019(struct test_result_t *ret);
static void tfm_sst_test_1020(struct test_result_t *ret);

#ifdef TFM_NS_CLIENT_IDENTIFICATION
static void tfm_sst_test_1021(struct test_result_t *ret);
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

static void tfm_sst_test_1022(struct test_result_t *ret);
static void tfm_sst_test_1023(struct test_result_t *ret);
static void tfm_sst_test_1024(struct test_result_t *ret);
static void tfm_sst_test_1025(struct test_result_t *ret);

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
static void tfm_sst_test_1026(struct test_result_t *ret);
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

static void tfm_sst_test_1027(struct test_result_t *ret);

static struct test_t asset_veeners_tests[] = {
    {&tfm_sst_test_1001, "TFM_SST_TEST_1001",
     "Create interface", {0} },
    {&tfm_sst_test_1002, "TFM_SST_TEST_1002",
     "Get attributes interface",  {0} },
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    {&tfm_sst_test_1003, "TFM_SST_TEST_1003",
     "Create with invalid thread name", {0} },
    {&tfm_sst_test_1004, "TFM_SST_TEST_1004",
     "Get attributes with invalid thread name", {0} },
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
    {&tfm_sst_test_1005, "TFM_SST_TEST_1005",
     "Get attributes with null attributes struct pointer", {0} },
    {&tfm_sst_test_1006, "TFM_SST_TEST_1006",
     "Write interface", {0} },
    {&tfm_sst_test_1007, "TFM_SST_TEST_1007",
     "Write with invalid thread name", {0} },
    {&tfm_sst_test_1008, "TFM_SST_TEST_1008",
     "Write with null buffer pointers", {0} },
    {&tfm_sst_test_1009, "TFM_SST_TEST_1009",
     "Write beyond end of asset", {0} },
    {&tfm_sst_test_1010, "TFM_SST_TEST_1010",
     "Read interface", {0} },
#ifdef TFM_NS_CLIENT_IDENTIFICATION
    {&tfm_sst_test_1011, "TFM_SST_TEST_1011",
     "Read with invalid thread name", {0} },
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
    {&tfm_sst_test_1012, "TFM_SST_TEST_1012",
     "Read with null buffer pointers", {0} },
    {&tfm_sst_test_1013, "TFM_SST_TEST_1013",
     "Read beyond current size of asset", {0} },
    {&tfm_sst_test_1014, "TFM_SST_TEST_1014",
     "Delete interface", {0} },

#ifdef TFM_NS_CLIENT_IDENTIFICATION
    {&tfm_sst_test_1015, "TFM_SST_TEST_1015",
     "Delete with invalid thread name", {0} },
    {&tfm_sst_test_1016, "TFM_SST_TEST_1016",
     "Delete with block compaction", {0} },
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

    {&tfm_sst_test_1017, "TFM_SST_TEST_1017",
     "Write and partial reads", {0} },
    {&tfm_sst_test_1018, "TFM_SST_TEST_1018",
     "Write more data than asset max size", {0} },

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
    {&tfm_sst_test_1019, "TFM_SST_TEST_1019",
     "Append data to an asset", {0} },
    {&tfm_sst_test_1020, "TFM_SST_TEST_1020",
     "Append data to an asset until EOF", {0} },

#ifdef TFM_NS_CLIENT_IDENTIFICATION
    {&tfm_sst_test_1021, "TFM_SST_TEST_1021",
     "Write data to two assets alternately", {0} },
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

    {&tfm_sst_test_1022, "TFM_SST_TEST_1022",
     "Access an illegal location: ROM", {0} },
    {&tfm_sst_test_1023, "TFM_SST_TEST_1023",
     "Access an illegal location: device memory", {0} },
    {&tfm_sst_test_1024, "TFM_SST_TEST_1024",
     "Access an illegal location: non-existant memory", {0} },
    {&tfm_sst_test_1025, "TFM_SST_TEST_1025",
     "Access an illegal location: secure memory", {0} },

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
    {&tfm_sst_test_1026, "TFM_SST_TEST_1026",
     "Write data to the middle of an existing asset", {0} },
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

    {&tfm_sst_test_1027, "TFM_SST_TEST_1027",
     "Basic test to verify set and get attributes functionality", {0} },
};

void register_testsuite_ns_sst_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(asset_veeners_tests) / sizeof(asset_veeners_tests[0]));

    set_testsuite("SST non-secure interface tests (TFM_SST_TEST_1XXX)",
                  asset_veeners_tests, list_size, p_test_suite);

#ifdef SST_SHOW_FLASH_WARNING
    TEST_LOG("\r\n**WARNING** The SST regression tests reduce the life of the "
             "flash memory as they write/erase multiple times the memory. \r\n"
             "Please, set the SST_RAM_FS flag to use RAM instead of flash."
             "\r\n\r\n");
#endif
}

/**
 * \note List of relations between thread name, client ID and permissions
 *
 * Asset permissions: SST_ASSET_ID_AES_KEY_192
 *
 *   THREAD NAME | CLIENT_ID       | Permissions
 *   ------------|-----------------|--------------------
 *     Thread_A  | SST_CLIENT_ID_0 | REFERENCE, READ, WRITE
 *     Thread_B  | SST_CLIENT_ID_1 | REFERENCE, READ
 *     Thread_C  | SST_CLIENT_ID_2 | REFERENCE
 *
 * Asset permissions: SST_ASSET_ID_SHA224_HASH
 *
 *   THREAD NAME | CLIENT_ID       | Permissions
 *   ------------|-----------------|--------------------
 *     Thread_A  | SST_CLIENT_ID_0 | NONE
 *     Thread_B  | SST_CLIENT_ID_1 | REFERENCE, READ, WRITE
 *     Thread_C  | SST_CLIENT_ID_2 | NONE
 *
 * Asset permissions: SST_ASSET_ID_SHA384_HASH
 *
 *   THREAD NAME | CLIENT_ID       | Permissions
 *   ------------|-----------------|--------------
 *     Thread_A  | SST_CLIENT_ID_0 | REFERENCE, WRITE
 *     Thread_B  | SST_CLIENT_ID_1 | NONE
 *     Thread_C  | SST_CLIENT_ID_2 | NONE
 */

/**
 * \brief Tests create function against:
 * - Valid client ID and asset ID
 * - Invalid asset ID
 */
TFM_SST_NS_TEST(1001, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    /* Checks write permissions in create function */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Attempts to create the asset a second time */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Should not fail to create an already-created asset");
        return;
    }

    /* Calls create with invalid asset ID */
    err = psa_sst_create(INVALID_ASSET_ID, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should fail for invalid ASSET ID");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get attributes function against:
 * - Valid client ID and attributes struct pointer
 * - Invalid client ID
 */
TFM_SST_NS_TEST(1002, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct psa_sst_asset_info_t asset_info;
    enum psa_sst_err_t err;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Calls get information with valid client ID and
     * attributes struct pointer
     */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_A should read the information of this asset");
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

    /* Calls get_attributes with invalid asset ID */
    err = psa_sst_get_info(INVALID_ASSET_ID, ASSET_TOKEN,
                           ASSET_TOKEN_SIZE, &asset_info);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Get information function should fail for an invalid "
                  "asset ID");
        return;
    }

    /* Deletes asset to clean up the SST area for the next test */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef TFM_NS_CLIENT_IDENTIFICATION
/**
 * \brief Tests create function with an invalid thread name.
 */
TFM_SST_NS_TEST(1003, INVALID_THREAD_NAME)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    /* Calls create function with an invalid thread name */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should not succeed with an invalid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Creates asset with an authorised client ID.
 */
static void tfm_sst_test_1004_task_1(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Calls get attributes function with an invalid thread name.
 */
static void tfm_sst_test_1004_task_2(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct psa_sst_asset_info_t asset_info;
    enum psa_sst_err_t err;

    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Get info should not succeed with invalid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Deletes asset to clean up the SST area for the next test.
 */
static void tfm_sst_test_1004_task_3(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests get attributes function with an invalid thread name.
 */
static void tfm_sst_test_1004(struct test_result_t *ret)
{
    /* Creates asset with an authorised client ID */
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1004_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    /* Calls get attributes function with an invalid thread name */
    tfm_sst_run_test(INVALID_THREAD_NAME, ret, tfm_sst_test_1004_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    /* Deletes asset to clean up the SST area for the next test */
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1004_task_3);
}
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

/**
 * \brief Tests get attributes function with a null attributes struct pointer.
 */
TFM_SST_NS_TEST(1005, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Calls get information with a null struct info pointer */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE, NULL);
    if (err != PSA_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Get info function should fail for a null "
                  "struct attributes pointer");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against:
 * - Valid client ID and data pointer
 * - Invalid client ID
 */
TFM_SST_NS_TEST(1006, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct psa_sst_asset_info_t asset_info;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Calls get information with valid client ID and
     * attributes struct pointer
     */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_A should read the information of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_info.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Asset current size should be size of the write data");
        return;
    }

    /* Calls write function with invalid client ID */
    err = psa_sst_write(INVALID_ASSET_ID, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Invalid asset ID should not write in the asset");
        return;
    }

    /* Deletes asset to clean up the SST area for the next test */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Creates asset with an authorised client ID.
 */
static void tfm_sst_test_1007_task_1(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Calls write function with an invalid thread name.
 */
static void tfm_sst_test_1007_task_2(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data = {0};

    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should not succeed with an invalid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Deletes asset to clean up the SST area for the next test.
 */
static void tfm_sst_test_1007_task_3(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function with an invalid thread name.
 */
static void tfm_sst_test_1007(struct test_result_t *ret)
{
    /* Creates asset with an authorised client ID */
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1007_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    /* Calls write function with an invalid thread name */
    tfm_sst_run_test(INVALID_THREAD_NAME, ret, tfm_sst_test_1007_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    /* Deletes asset to clean up the SST area for the next test */
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1007_task_3);
}

/**
 * \brief Tests read function with:
 * - Null write data pointer
 */
TFM_SST_NS_TEST(1008, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = NULL;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write function with data pointer set to NULL */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail with data pointer set to NULL");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function with offset + write data size larger than max
 *        asset size.
 */
TFM_SST_NS_TEST(1009, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t wrt_data[SST_ASSET_MAX_SIZE_AES_KEY_192] = {0};

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Attempts to write beyond end of asset starting from a valid offset */
    io_data.data = wrt_data;
    io_data.size = BUFFER_PLUS_PADDING_SIZE;
    io_data.offset = 0;

    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Writing beyond end of asset should not succeed");
        return;
    }

    /* Attempts to write to an offset beyond the end of the asset */
    io_data.size = 1;
    io_data.offset = SST_ASSET_MAX_SIZE_AES_KEY_192;

    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Write to an offset beyond end of asset should not succeed");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read function against:
 * - Valid client ID and data pointer
 * - Invalid asset ID
 */
TFM_SST_NS_TEST(1010, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Sets data structure for read*/
    io_data.data = read_data + HALF_PADDING_SIZE;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Read data from the asset */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
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

    if (memcmp((read_data+(HALF_PADDING_SIZE+WRITE_BUF_SIZE)), "XXXX",
                HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read buffer contains illegal post-data");
        return;
    }

    /* Calls read with invalid asset ID */
    err = psa_sst_read(INVALID_ASSET_ID, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail when read is called with an invalid "
                  "asset ID");
        return;
    }

    /* Deletes asset to clean up the SST area for the next test */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef TFM_NS_CLIENT_IDENTIFICATION
/**
 * \brief Creates asset with an authorised client ID.
 */
static void tfm_sst_test_1011_task_1(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Calls read function with an invalid thread name.
 */
static void tfm_sst_test_1011_task_2(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = 1;
    io_data.offset = 0;

    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should not succeed with an invalid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Deletes asset to clean up the SST area for the next test.
 */
static void tfm_sst_test_1011_task_3(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read function with an invalid thread name.
 */
static void tfm_sst_test_1011(struct test_result_t *ret)
{
    /* Creates asset with an authorised client ID */
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1011_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    /* Calls read function with an invalid thread name */
    tfm_sst_run_test(INVALID_THREAD_NAME, ret, tfm_sst_test_1011_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    /* Deletes asset to clean up the SST area for the next test */
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1011_task_3);
}
#endif /* TFM_NS_CLIENT_IDENTIFICATION */

/**
 * \brief Tests read function with:
 * - Null read data pointer
 */
TFM_SST_NS_TEST(1012, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    io_data.data = NULL;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls read with null read data pointer */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read with read data pointer set to NULL should fail");
        return;
    }

    /* Calls delete asset to clean up SST area for next test */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read function with offset + read data size larger than current
 *        asset size.
 */
TFM_SST_NS_TEST(1013, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    struct psa_sst_asset_info_t asset_info;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should work correctly");
        return;
    }

    /* Gets current asset information */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_A should read the information of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_info.size_current == 0) {
        TEST_FAIL("Asset current size should be bigger than 0");
        return;
    }

    /* Attempts to read beyond the current size starting from a valid offset */
    io_data.data = read_data;
    io_data.size = WRITE_BUF_SIZE + 1;
    io_data.offset = 0;

    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Read beyond current size should not succeed");
        return;
    }

    /* Attempts to read from an offset beyond the current size of the asset */
    io_data.size = 1;
    io_data.offset = asset_info.size_current;

    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Read from an offset beyond current size should not succeed");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests delete function with:
 * - Valid client ID
 * - Invalid asset ID
 */
TFM_SST_NS_TEST(1014, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    /* Creates assset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Calls delete asset */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should not fail as thread has"
                  " write permissions");
        return;
    }

    /* Calls delete with a deleted asset ID */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("The delete action should fail as asset was deleted");
        return;
    }

    /* Calls delete asset with invalid asset ID */
    err = psa_sst_delete(INVALID_ASSET_ID, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("The delete action should fail as asset ID is not valid");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef TFM_NS_CLIENT_IDENTIFICATION
/**
 * \brief Creates asset with an authorised client ID.
 */
static void tfm_sst_test_1015_task_1(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Calls delete function with an invalid thread name.
 */
static void tfm_sst_test_1015_task_2(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Delete should not succeed with an invalid thread name");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Deletes asset to clean up the SST area for the next test.
 */
static void tfm_sst_test_1015_task_3(struct test_result_t *ret)
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests delete function with an invalid thread name.
 */
static void tfm_sst_test_1015(struct test_result_t *ret)
{
    /* Creates asset with an authorised client ID */
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1015_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    /* Calls delete function with an invalid thread name */
    tfm_sst_run_test(INVALID_THREAD_NAME, ret, tfm_sst_test_1015_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    /* Deletes asset to clean up the SST area for the next test */
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1015_task_3);
}

/**
 * \brief Tests data block compact feature.
 *        Create asset 2 to locate it at the beginning of the block. Then,
 *        create asset 1 to be located after asset 2. Write data on asset
 *        1 and remove asset 2. If delete works correctly, when the code
 *        reads back the asset 1 data, the data must be correct.
 */
static void tfm_sst_test_1016_task_1(struct test_result_t *ret)
{
    const uint32_t asset_uuid_2 = SST_ASSET_ID_SHA384_HASH;
    enum psa_sst_err_t err;

    /* Creates asset 2 first to locate it at the beginning of the data block */
    err = psa_sst_create(asset_uuid_2, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1016_task_2(struct test_result_t *ret)
{
    const uint32_t asset_uuid_1 = SST_ASSET_ID_SHA224_HASH;
    struct sst_test_buf_t io_data;
    enum psa_sst_err_t err;
    uint8_t wrt_data[BUF_SIZE_SHA224] = WRITE_DATA_SHA224;

    /* Creates asset 1 */
    err = psa_sst_create(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH;
    io_data.offset = 0;

    /* Writes data into asset 1 */
    err = psa_sst_write(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1016_task_3(struct test_result_t *ret)
{
    const uint32_t asset_uuid_2 = SST_ASSET_ID_SHA384_HASH;
    enum psa_sst_err_t err;

    /* Deletes asset 2. After the delete call, asset 1 should be at the
     * beginning of the block.
     */
    err = psa_sst_delete(asset_uuid_2, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1016_task_4(struct test_result_t *ret)
{
    const uint32_t asset_uuid_1 = SST_ASSET_ID_SHA224_HASH;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t read_data[BUF_SIZE_SHA224] = READ_DATA_SHA224;
    uint8_t wrt_data[BUF_SIZE_SHA224] = WRITE_DATA_SHA224;

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = SST_ASSET_MAX_SIZE_SHA224_HASH;
    io_data.offset = 0;

    /* If the compact worked as expected, the test should be able to read back
     * the data from asset 1 correctly.
     */
    err = psa_sst_read(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for Thread_B");
        return;
    }

    if (memcmp(read_data, wrt_data, BUF_SIZE_SHA224) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Calls delete asset */
    err = psa_sst_delete(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1016(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1016_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1016_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1016_task_3);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1016_task_4);
}

#endif /* TFM_NS_CLIENT_IDENTIFICATION */

/**
 * \brief Tests write and partial reads.
 */
TFM_SST_NS_TEST(1017, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint32_t i;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should works correctly");
        return;
    }

    /* Sets data structure for read*/
    io_data.data = (read_data + HALF_PADDING_SIZE);
    io_data.size = 1;
    io_data.offset = 0;


    for (i = 0; i < WRITE_BUF_SIZE; i++) {
        /* Read data from the asset */
        err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           io_data.size, io_data.offset, io_data.data);
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
        if (err != PSA_SST_ERR_SUCCESS) {
#else
        if (io_data.offset != 0 && err != PSA_SST_ERR_PARAM_ERROR) {
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
    if (memcmp((read_data + HALF_PADDING_SIZE), wrt_data,
                WRITE_BUF_SIZE) != 0) {
#else
    /* Read should fail if no partial asset rw except when offset 0 */
    if (memcmp((read_data + HALF_PADDING_SIZE), "DXXXX",
                WRITE_BUF_SIZE) != 0) {
#endif
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    if (memcmp((read_data + (HALF_PADDING_SIZE + WRITE_BUF_SIZE)), "XXXX",
                HALF_PADDING_SIZE) != 0) {
        TEST_FAIL("Read buffer contains illegal post-data");
        return;
    }

    /* Calls delete asset */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against a write call where data size is
 *        bigger than the maximum asset size.
 */
TFM_SST_NS_TEST(1018, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t wrt_data[BUF_SIZE_AES_KEY_192] = {0};

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = (SST_ASSET_MAX_SIZE_AES_KEY_192 + 1);
    io_data.offset = 0;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Should have failed asset write of too large");
        return;
    }

    /* Calls delete asset */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
/**
 * \brief Tests write function against multiple writes.
 */
TFM_SST_NS_TEST(1019, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE]  = "XXXXXXXXXXXXX";
    uint8_t wrt_data[WRITE_BUF_SIZE+1]  = "Hello";
    uint8_t wrt_data2[WRITE_BUF_SIZE+1] = "World";

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 1 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Writes data 2 in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 2 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = WRITE_BUF_SIZE * 2;
    io_data.offset = 0;

    /* Reads back the data */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, "HelloWorldXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    /* Calls delete asset */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests write function against multiple writes until the end of asset.
 */
TFM_SST_NS_TEST(1020, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t read_data[BUF_SIZE_AES_KEY_192] = READ_DATA_AES_KEY_192;
    uint8_t wrt_data[BUF_SIZE_AES_KEY_192] = WRITE_DATA_AES_KEY_192_1;
    uint8_t wrt_data2[BUF_SIZE_AES_KEY_192] = WRITE_DATA_AES_KEY_192_2;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 1 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = (SST_ASSET_MAX_SIZE_AES_KEY_192 - WRITE_BUF_SIZE) + 1;
    io_data.offset = WRITE_BUF_SIZE;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Write data 2 should have failed as this write tries to "
                  "write more bytes that the max size");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data + WRITE_BUF_SIZE;
    io_data.size = SST_ASSET_MAX_SIZE_AES_KEY_192 - WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Writes data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data 3 failed");
        return;
    }

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = SST_ASSET_MAX_SIZE_AES_KEY_192;
    io_data.offset = 0;

    /* Read back the data */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, wrt_data, BUF_SIZE_AES_KEY_192) != 0) {
        TEST_FAIL("Read buffer has read incorrect data");
        return;
    }

    /* Calls delete asset */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef TFM_NS_CLIENT_IDENTIFICATION
/**
 * \brief Tests writing data to two assets alternately before read-back.
 */
static void tfm_sst_test_1021_task_1(struct test_result_t *ret)
{
    const uint32_t asset_uuid_1 = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    /* Creates asset 1 */
    err = psa_sst_create(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021_task_2(struct test_result_t *ret)
{
    const uint32_t asset_uuid_2 = SST_ASSET_ID_SHA224_HASH;
    enum psa_sst_err_t err;

    /* Creates asset 2 */
    err = psa_sst_create(asset_uuid_2, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021_task_3(struct test_result_t *ret)
{
    const uint32_t asset_uuid_1 = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t wrt_data[WRITE_BUF_SIZE+1]  = "Hello";

    /* Sets data structure */
    io_data.data = wrt_data;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Writes data in asset 1 */
    err = psa_sst_write(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021_task_4(struct test_result_t *ret)
{
    const uint32_t asset_uuid_2 = SST_ASSET_ID_SHA224_HASH;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t wrt_data2[3] = "Hi";

    /* Writes data 2 in asset 2 */
    /* Sets data structure */
    io_data.data = wrt_data2;
    io_data.size = 2;
    io_data.offset = 0;

    err = psa_sst_write(asset_uuid_2, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021_task_5(struct test_result_t *ret)
{
    const uint32_t asset_uuid_1 = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t wrt_data3[WRITE_BUF_SIZE+1] = "World";

    /* Sets data structure */
    io_data.data = wrt_data3;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = WRITE_BUF_SIZE;

    /* Writes data 3 in asset 1 */
    err = psa_sst_write(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021_task_6(struct test_result_t *ret)
{
    const uint32_t asset_uuid_2 = SST_ASSET_ID_SHA224_HASH;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t wrt_data4[WRITE_BUF_SIZE+1] = "12345";

    /* Sets data structure */
    io_data.data = wrt_data4;
    io_data.size = WRITE_BUF_SIZE;
    io_data.offset = 2;

    /* Writes data 4 in asset 2 */
    err = psa_sst_write(asset_uuid_2, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write data should work for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021_task_7(struct test_result_t *ret)
{
    const uint32_t asset_uuid_1 = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE]  = "XXXXXXXXXXXXX";

    /* Sets data structure */
    io_data.data = read_data;
    /* size of wrt_data + wrt_data3 */
    io_data.size = WRITE_BUF_SIZE + WRITE_BUF_SIZE;
    io_data.offset = 0;

    /* Read back the asset 1 */
    err = psa_sst_read(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for Thread_A");
        return;
    }

    if (memcmp(read_data, "HelloWorldXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021_task_8(struct test_result_t *ret)
{
    const uint32_t asset_uuid_2 = SST_ASSET_ID_SHA224_HASH;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE]  = "XXXXXXXXXXXXX";

    /* Sets data structure */
    io_data.data = read_data;
    io_data.size = 2 + WRITE_BUF_SIZE; /* size of wrt_data2 + wrt_data4 */
    io_data.offset = 0;

    /* Read back the asset 1 */
    err = psa_sst_read(asset_uuid_2, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Incorrect number of bytes read back");
        return;
    }

    if (memcmp(read_data, "Hi12345XXXXXX", READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer has incorrect data");
        return;
    }

    /* Calls delete asset */
    err = psa_sst_delete(asset_uuid_2, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021_task_9(struct test_result_t *ret)
{
    const uint32_t asset_uuid_1 = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    /* Calls delete asset 1 */
    err = psa_sst_delete(asset_uuid_1, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("The delete action should work correctly");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_sst_test_1021(struct test_result_t *ret)
{
    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1021_task_1);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1021_task_2);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1021_task_3);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1021_task_4);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1021_task_5);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1021_task_6);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1021_task_7);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_B", ret, tfm_sst_test_1021_task_8);
    if (ret->val != TEST_PASSED) {
        return;
    }

    tfm_sst_run_test("Thread_A", ret, tfm_sst_test_1021_task_9);
}
#endif /* TFM_NS_CLIENT_IDENTIFICATION */
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

/**
 * \brief Tests read from and write to an illegal location: ROM.
 */
TFM_SST_NS_TEST(1022, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)ROM_ADDR_LOCATION;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write with a ROM address location */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a ROM address location */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    /* Deletes asset to clean up the SST area */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read from and write to an illegal location: device memory.
 */
TFM_SST_NS_TEST(1023, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)DEV_ADDR_LOCATION;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write with a device address location */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a device address location */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    /* Deletes asset to clean up the SST area */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read from and write to an illegal location: non-existant memory.
 */
TFM_SST_NS_TEST(1024, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)NON_EXIST_ADDR_LOCATION;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write with a non-existing address location */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a non-existing address location */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    /* Deletes asset to clean up the SST area */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests read from and write to an illegal location: secure memory.
 */
TFM_SST_NS_TEST(1025, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = (uint8_t *)SECURE_ADDR_LOCATION;
    io_data.size = 1;
    io_data.offset = 0;

    /* Calls write with a secure address location */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should fail for an illegal location");
        return;
    }

    /* Calls read with a secure address location */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should fail for an illegal location");
        return;
    }

    /* Deletes asset to clean up the SST area */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail");
        return;
    }

    ret->val = TEST_PASSED;
}

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
/**
 * \brief Tests write data to the middle of an existing asset
 */
TFM_SST_NS_TEST(1026, "Thread_A")
{
    struct psa_sst_asset_info_t asset_info;
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;
    struct sst_test_buf_t io_data;
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXXXX";
    uint8_t wrt_data_1[WRITE_BUF_SIZE] = "AAAA";
    uint8_t wrt_data_2[2] = "B";

    /* Creates asset */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data_1;
    io_data.size = (WRITE_BUF_SIZE - 1);
    io_data.offset = 0;

    /* Write data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("First write should not fail");
        return;
    }

    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_A should read the information of this asset");
        return;
    }

    /* Checks attributes */
    if (asset_info.size_current != (WRITE_BUF_SIZE - 1)) {
        TEST_FAIL("Current size should be equal to write size");
        return;
    }

    /* Sets data structure */
    io_data.data = wrt_data_2;
    io_data.size = 1;
    io_data.offset = 1;

    /* Write data in the asset */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Second write should not fail");
        return;
    }

    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Thread_A should read the information of this asset");
        return;
    }

    /* Checks that the asset's current size has not changed */
    if (asset_info.size_current != (WRITE_BUF_SIZE - 1)) {
        TEST_FAIL("Current size should not have changed");
        return;
    }

    io_data.data = (read_data + HALF_PADDING_SIZE);
    io_data.size = (WRITE_BUF_SIZE - 1);
    io_data.offset = 0;

    /* Calls read with a non-existing address location */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_SUCCESS) {
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
    io_data.data = wrt_data_2;
    io_data.size = 1;
    io_data.offset = (asset_info.size_current + 1);

    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        io_data.size, io_data.offset, io_data.data);
    if (err != PSA_SST_ERR_PARAM_ERROR) {
        TEST_FAIL("Write must fail if the offset is bigger than the current"
                  " asset's size");
        return;
    }

    ret->val = TEST_PASSED;
}
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

/**
 * \brief Basic test to verify set and get attributes functionality.
 */
TFM_SST_NS_TEST(1027, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct psa_sst_asset_attrs_t wrt_attrs;
    struct psa_sst_asset_attrs_t read_attrs = {
        .attrs = 0,
        .validity.start = 0,
        .validity.end = 0 };
    enum psa_sst_err_t err;

    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail");
        return;
    }

    wrt_attrs.attrs = (PSA_SST_ASSET_ATTR_ENCRYPT | PSA_SST_ASSET_ATTR_DECRYPT |
                       PSA_SST_ASSET_ATTR_SIGN | PSA_SST_ASSET_ATTR_VERIFY);
    wrt_attrs.validity.start = 0;
    wrt_attrs.validity.end   = 0;

    err = psa_sst_set_attributes(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                                 &wrt_attrs);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Set attributes should not fail");
        return;
    }

    err = psa_sst_get_attributes(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                                 &read_attrs);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Get attributes should not fail");
        return;
    }

    /* Compare asset attributes */
    if (memcmp(&wrt_attrs, &read_attrs, PSA_SST_ASSET_ATTR_SIZE) != 0) {
        TEST_FAIL("Attributes are differents");
        return;
    }

    ret->val = TEST_PASSED;
}
