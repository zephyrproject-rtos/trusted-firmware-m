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
#include "test/framework/test_framework_helpers.h"
#include "psa_sst_api.h"

/* Define default asset's token */
#define ASSET_TOKEN      NULL
#define ASSET_TOKEN_SIZE 0

/* The tests in this test suite cover access to an asset when the clients
 * has:
 *  - REFERENCE/READ/WRITE permissions,
 *  - REFERENCE/READ permissions,
 *  - REFERENCE/WRITE permissions,
 *  - REFERENCE permission,
 *  - no permissions (NONE).
 *
 * In each case, every function in the SST API is tested for correct access
 * policy.
 */

/**
 * \note List of relations between thread name, client ID and permissions.
 *
 * Asset permissions: SST_ASSET_ID_AES_KEY_192
 *
 *   THREAD NAME | CLIENT_ID       | Permissions
 *   ------------|-----------------|-----------------
 *     Thread_A  | SST_CLIENT_ID_0 | REFERENCE, READ, WRITE
 *     Thread_B  | SST_CLIENT_ID_1 | REFERENCE, READ
 *     Thread_C  | SST_CLIENT_ID_2 | REFERENCE
 *
 * Asset permissions: SST_ASSET_ID_SHA224_HASH
 *
 *   THREAD NAME | CLIENT_ID       | Permissions
 *   ------------|-----------------|-----------------
 *     Thread_A  | SST_CLIENT_ID_0 | NONE
 *     Thread_B  | SST_CLIENT_ID_1 | REFERENCE, READ, WRITE
 *     Thread_C  | SST_CLIENT_ID_2 | NONE
 *
 * Asset permissions: SST_ASSET_ID_SHA384_HASH
 *
 *   THREAD NAME | CLIENT_ID       | Permissions
 *   ------------|-----------------|-----------
 *     Thread_A  | SST_CLIENT_ID_0 | REFERENCE, WRITE
 *     Thread_B  | SST_CLIENT_ID_1 | NONE
 *     Thread_C  | SST_CLIENT_ID_2 | NONE
 */

/* Test suite defines */
/* Each thread has different write data so that the tests can verify the correct
 * thread's data is stored in the asset.
 */
#define WRITE_DATA_A "AAAA"
#define WRITE_DATA_B "BBBB"
#define WRITE_DATA_C "CCCC"

/* Original contents of the read buffer */
#define READ_DATA "XXXXXXXXXXXXX"

/* Contents of the read buffer after reading back write_data */
#define RESULT_DATA(write_data) ("XXXX" write_data "\0XXXX")

/* Sizes of the read and write buffers */
#define WRITE_BUF_SIZE (sizeof(WRITE_DATA_A))
#define READ_BUF_SIZE  (sizeof(READ_DATA))

/* Define test suite for SST policy tests */
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
static void tfm_sst_test_4010(struct test_result_t *ret);

static struct test_t policy_tests[] = {
    { &tfm_sst_test_4001, "TFM_SST_TEST_4001",
      "Check policy for AES_KEY_192 from Thread_A", {0} },
    { &tfm_sst_test_4002, "TFM_SST_TEST_4002",
      "Check policy for AES_KEY_192 from Thread_C", {0} },
    { &tfm_sst_test_4003, "TFM_SST_TEST_4003",
      "Check policy for AES_KEY_192 from Thread_B", {0} },
    { &tfm_sst_test_4004, "TFM_SST_TEST_4004",
      "Delete AES_KEY_192 from Thread_A", {0} },
    { &tfm_sst_test_4005, "TFM_SST_TEST_4005",
      "Check policy for SHA224_HASH from Thread_B", {0} },
    { &tfm_sst_test_4006, "TFM_SST_TEST_4006",
      "Check policy for SHA224_HASH from Thread_C",  {0} },
    { &tfm_sst_test_4007, "TFM_SST_TEST_4007",
      "Delete SHA224_HASH from Thread_B", {0} },
    { &tfm_sst_test_4008, "TFM_SST_TEST_4008",
      "Check policy for SHA384_HASH from Thread_A", {0} },
    { &tfm_sst_test_4009, "TFM_SST_TEST_4009",
      "Check policy for SHA384_HASH from Thread_C", {0} },
    { &tfm_sst_test_4010, "TFM_SST_TEST_4010",
      "Delete SHA384_HASH from Thread_A", {0} },
};

void register_testsuite_ns_sst_policy(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(policy_tests) / sizeof(policy_tests[0]));

    set_testsuite("SST policy tests (TFM_SST_TEST_4XXX)",
                  policy_tests, list_size, p_test_suite);
}

/**
 * \brief Tests policy for SST_ASSET_ID_AES_KEY_192 with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|-----------------------
 *          Thread_A    | SST_CLIENT_ID_0 | REFERENCE, READ, WRITE
 */
TFM_SST_NS_TEST(4001, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct psa_sst_asset_info_t asset_info;
    struct sst_test_buf_t buf;
    enum psa_sst_err_t err;
    uint8_t write_data[WRITE_BUF_SIZE] = WRITE_DATA_C;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* The create function requires WRITE permission */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = write_data;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* The write function requires WRITE permission */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for Thread_A");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = read_data + HALF_PADDING_SIZE;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* The read function requires READ permission */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for Thread_A");
        return;
    }

    /* Checks correct data was read-back */
    if (memcmp(read_data, RESULT_DATA(WRITE_DATA_C), READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer contains incorrect data");
        return;
    }

    /* The get information function requires any permission other than NONE */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Get information should not fail for Thread_A");
        return;
    }

    /* Checks attributes are correct */
    if (asset_info.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Current size of the asset is incorrect");
        return;
    }

    if (asset_info.size_max != SST_ASSET_MAX_SIZE_AES_KEY_192) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests policy for SST_ASSET_ID_AES_KEY_192 with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|------------
 *          Thread_C    | SST_CLIENT_ID_2 | REFERENCE
 */
TFM_SST_NS_TEST(4002, "Thread_C")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct psa_sst_asset_info_t asset_info;
    struct sst_test_buf_t buf;
    enum psa_sst_err_t err;
    uint8_t write_data[WRITE_BUF_SIZE] = WRITE_DATA_A;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Create should fail as Thread_C does not have WRITE permission */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should not succeed for Thread_C");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = write_data;
    buf.size = WRITE_BUF_SIZE;
    /* Increases offset so that current size will change if write succeeds */
    buf.offset = 1;

    /* Write should fail as Thread_C does not have WRITE permission */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should not succeed for Thread_C");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = read_data + 3;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* Read should fail as Thread_C does not have READ permission */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should not succeed for Thread_C");
        return;
    }

    /* Checks read_data has not been changed by the call to read */
    if (memcmp(read_data, READ_DATA, READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer should not have changed");
        return;
    }

    /* Get information should succeed as Thread_C has at least one permission */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Get information should not fail for Thread_C");
        return;
    }

    /* Checks attributes are correct */
    if (asset_info.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Current size of the asset is incorrect");
        return;
    }

    if (asset_info.size_max != SST_ASSET_MAX_SIZE_AES_KEY_192) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Delete should fail as Thread_C does not have WRITE permission */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Delete should not succeed for Thread_C");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests policy for SST_ASSET_ID_AES_KEY_192 with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|----------------
 *          Thread_B    | SST_CLIENT_ID_1 | REFERENCE, READ
 */
TFM_SST_NS_TEST(4003, "Thread_B")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    struct psa_sst_asset_info_t asset_info;
    struct sst_test_buf_t buf;
    enum psa_sst_err_t err;
    uint8_t write_data[WRITE_BUF_SIZE] = WRITE_DATA_B;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Create should fail as Thread_B does not have WRITE permission */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should not succeed for Thread_B");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = write_data;
    buf.size = WRITE_BUF_SIZE;
    /* Increases offset so that current size will change if write succeeds */
    buf.offset = 2;

    /* Write should fail as Thread_B does not have WRITE permission */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should not succeed for Thread_B");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = read_data + HALF_PADDING_SIZE;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* Read should succeed as Thread_B has READ permission */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for Thread_B");
        return;
    }

    /* Checks correct data was read-back. It should still be Thread_A's data as
     * Thread_B does not have write permission.
     */
    if (memcmp(read_data, RESULT_DATA(WRITE_DATA_C), READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer contains incorrect data");
        return;
    }

    /* Get attributes should succeed as Thread_B has at least one permission */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Get information should not fail for Thread_B");
        return;
    }

    /* Checks attributes are correct */
    if (asset_info.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Current size of the asset is incorrect");
        return;
    }

    if (asset_info.size_max != SST_ASSET_MAX_SIZE_AES_KEY_192) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Delete should fail as Thread_B does not have WRITE permission */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Delete should not succeed for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests delete policy for SST_ASSET_ID_AES_KEY_192 with the
 *        following permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|-----------------------
 *          Thread_A    | SST_CLIENT_ID_0 | REFERENCE, READ, WRITE
 *
 * This test is performed last so that the asset still exists during the
 * preceeding test cases.
 */
TFM_SST_NS_TEST(4004, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_AES_KEY_192;
    enum psa_sst_err_t err;

    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests policy for SST_ASSET_ID_SHA224_HASH with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|-----------------------
 *          Thread_B    | SST_CLIENT_ID_1 | REFERENCE, READ, WRITE
 */
TFM_SST_NS_TEST(4005, "Thread_B")
{
    const uint32_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    struct psa_sst_asset_info_t asset_info;
    struct sst_test_buf_t buf;
    enum psa_sst_err_t err;
    uint8_t write_data[WRITE_BUF_SIZE] = WRITE_DATA_B;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Create should succeed as Thread_B has WRITE permission */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_B");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = write_data;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* Write should succeed as Thread_B has WRITE permission */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for Thread_B");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = read_data + HALF_PADDING_SIZE;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* Read should succeed as Thread_B has READ permission */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for Thread_B");
        return;
    }

    /* Checks correct data was read-back */
    if (memcmp(read_data, RESULT_DATA(WRITE_DATA_B), READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer contains incorrect data");
        return;
    }

    /* Get information should succeed as Thread_B has at least one permission */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Get information should not fail for Thread_B");
        return;
    }

    /* Checks attributes are correct */
    if (asset_info.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Current size of the asset is incorrect");
        return;
    }

    if (asset_info.size_max != SST_ASSET_MAX_SIZE_SHA224_HASH) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests policy for SST_ASSET_ID_SHA224_HASH with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|------------
 *          Thread_C    | SST_CLIENT_ID_2 | NONE
 */
TFM_SST_NS_TEST(4006, "Thread_C")
{
    const uint32_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    struct psa_sst_asset_info_t asset_info = { 0 };
    struct sst_test_buf_t buf;
    enum psa_sst_err_t err;
    uint8_t write_data[WRITE_BUF_SIZE] = WRITE_DATA_A;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Create should fail as Thread_C has no permissions */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should not succeed for Thread_C");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = write_data;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* The write function uses a valid asset ID, obtained by the previous test,
     * to check that Thread_C cannot perform the write without the proper access
     * permissions even if it has a valid asset ID. So the write should fail as
     * Thread_C has no permissions.
     */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should not succeed for Thread_C");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = read_data + 3;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* Read should fail as Thread_C has no permissions */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should not succeed for Thread_C");
        return;
    }

    /* Checks read_data has not been changed by the call to read */
    if (memcmp(read_data, READ_DATA, READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer should not have changed");
        return;
    }

    /* Get information should fail as Thread_C has no permissions */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Get information should not succeed for Thread_C");
        return;
    }

    /* Checks attributes have not been changed by the call to get attributes */
    if (asset_info.size_current != 0) {
        TEST_FAIL("Current size of the asset should not have changed");
        return;
    }

    if (asset_info.size_max != 0) {
        TEST_FAIL("Max size of the asset should not have changed");
        return;
    }

    /* Delete should fail as Thread_C has no permissions */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Delete should not succeed for Thread_C");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests delete policy for SST_ASSET_ID_SHA224_HASH with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|-----------------------
 *          Thread_B    | SST_CLIENT_ID_1 | REFERENCE, READ, WRITE
 *
 * This test is performed last so that the asset still exists during the
 * preceeding test cases.
 */
TFM_SST_NS_TEST(4007, "Thread_B")
{
    const uint32_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    enum psa_sst_err_t err;

    /* Delete should succeed as Thread_B has WRITE permission */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for Thread_B");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests policy for SST_ASSET_ID_SHA384_HASH with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|-----------------
 *          Thread_A    | SST_CLIENT_ID_0 | REFERENCE, WRITE
 */
TFM_SST_NS_TEST(4008, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_SHA384_HASH;
    struct psa_sst_asset_info_t asset_info;
    struct sst_test_buf_t buf;
    enum psa_sst_err_t err;
    uint8_t write_data[WRITE_BUF_SIZE] = WRITE_DATA_C;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Create should succeed as Thread_A has WRITE permission */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Create should not fail for Thread_A");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = write_data;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* Write should succeed as Thread_A has WRITE permission */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for Thread_A");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = read_data + HALF_PADDING_SIZE;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* Read should fail as Thread_A does not have READ permission */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should not succeed for Thread_A");
        return;
    }

    /* Checks read_data has not been changed by the call to read */
    if (memcmp(read_data, READ_DATA, READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer should not have changed");
        return;
    }

    /* Get information should succeed as Thread_A has at least one permission */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Get information should not fail for Thread_A");
        return;
    }

    /* Checks attributes are correct */
    if (asset_info.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Current size of the asset is incorrect");
        return;
    }

    if (asset_info.size_max != SST_ASSET_MAX_SIZE_SHA384_HASH) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    ret->val = TEST_PASSED;
}


/**
 * \brief Tests policy for SST_ASSET_ID_SHA384_HASH with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|------------
 *          Thread_C    | SST_CLIENT_ID_2 | NONE
 */
TFM_SST_NS_TEST(4009, "Thread_C")
{
    const uint32_t asset_uuid = SST_ASSET_ID_SHA384_HASH;
    struct psa_sst_asset_info_t asset_info = { 0 };
    struct sst_test_buf_t buf;
    enum psa_sst_err_t err;
    uint8_t write_data[WRITE_BUF_SIZE] = WRITE_DATA_A;
    uint8_t read_data[READ_BUF_SIZE] = READ_DATA;

    /* Create should fail as Thread_C has no permissions */
    err = psa_sst_create(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Create should not succeed for Thread_C");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = write_data;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* The write function uses a valid asset ID, obtained by the previous test,
     * to check that Thread_C cannot perform the write without the proper access
     * permissions even if it has a valid asset ID. So the write should fail as
     * Thread_C has no permissions.
     */
    err = psa_sst_write(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Write should not succeed for Thread_C");
        return;
    }

    /* Sets the sst_test_buf_t structure */
    buf.data = read_data + HALF_PADDING_SIZE;
    buf.size = WRITE_BUF_SIZE;
    buf.offset = 0;

    /* Read should fail as Thread_C has no permissions */
    err = psa_sst_read(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                       buf.size, buf.offset, buf.data);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Read should not succeed for Thread_C");
        return;
    }

    /* Checks read_data has not been changed by the call to read */
    if (memcmp(read_data, READ_DATA, READ_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer should not have changed");
        return;
    }

    /* Get information should fail as Thread_C has no permissions */
    err = psa_sst_get_info(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                           &asset_info);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Get information should not succeed for Thread_C");
        return;
    }

    /* Checks attributes have not been changed by the call to get attributes */
    if (asset_info.size_current != 0) {
        TEST_FAIL("Current size of the asset should not have changed");
        return;
    }

    if (asset_info.size_max != 0) {
        TEST_FAIL("Max size of the asset should not have changed");
        return;
    }

    /* Delete should fail as Thread_C has no permissions */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Delete should not succeed for Thread_C");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests delete policy for SST_ASSET_ID_SHA384_HASH with the following
 *        permissions:
 *          THREAD NAME | CLIENT_ID       | Permissions
 *          ------------|-----------------|-----------------
 *          Thread_A    | SST_CLIENT_ID_0 | REFERENCE, WRITE
 *
 * This test is performed last so that the asset still exists during the
 * preceeding test cases.
 */
TFM_SST_NS_TEST(4010, "Thread_A")
{
    const uint32_t asset_uuid = SST_ASSET_ID_SHA384_HASH;
    enum psa_sst_err_t err;

    /* Delete should succeed as Thread_A has WRITE permission */
    err = psa_sst_delete(asset_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for Thread_A");
        return;
    }

    ret->val = TEST_PASSED;
}
