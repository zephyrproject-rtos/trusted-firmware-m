/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_tests.h"

#include <stdio.h>
#include <string.h>

#include "test/framework/helpers.h"
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "tfm_sst_defs.h"
#include "secure_fw/services/secure_storage/sst_core_interface.h"
#include "tfm_sst_veneers.h"
#include "s_test_helpers.h"

/* Test suite defines */
#define INVALID_ASSET_ID           0
#define READ_BUF_SIZE             12UL
#define WRITE_BUF_SIZE             5UL

/*
 * Summary of tests covered by the test suite:
 * - read/write to an asset with READ/WRITE permission
 * - read/write to an asset when the application doesn't have any
 *   permission (NONE)
 * - read/write to an asset when the application has REFERENCE permission
 * - write to an asset when the application has REFERENCE/READ permission
 * - read to an asset when the application has REFERENCE/WRITE permission
 * - read attributes to an asset when the application has REFERENCE permission
 */
/* Define test suite for secure side veneer i/f tests */
/* List of tests */
static void tfm_sst_test_4001(struct test_result_t *ret);
static void tfm_sst_test_4002(struct test_result_t *ret);
static void tfm_sst_test_4003(struct test_result_t *ret);

static struct test_t interface_tests[] = {
    {&tfm_sst_test_4001, "TFM_SST_TEST_4001",
     "Check policy against large asset 1", {0} },
    {&tfm_sst_test_4002, "TFM_SST_TEST_4002",
     "Check policy against small asset 1", {0} },
    {&tfm_sst_test_4003, "TFM_SST_TEST_4003",
     "Check policy against small asset 2", {0} },
};

void register_testsuite_s_sst_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size = (sizeof(interface_tests) / sizeof(interface_tests[0]));

    set_testsuite("SST policy tests (TFM_SST_TEST_4XXX)",
                  interface_tests, list_size, p_test_suite);
}

/**
 * \brief Tests policy against SST_ASSET_ID_X509_CERT_LARGE with the following
 *        permissions
 *        APP_ID       | Permissions
 *        -------------------------------------
 *        S_APP_ID | REFERENCE
 *        S_APP_ID | REFERENCE, READ
 *        S_APP_ID | REFERENCE, READ, WRITE
 */
static void tfm_sst_test_4001(struct test_result_t *ret)
{
    uint32_t hdl;
    uint32_t app_id;
    const uint16_t asset_uuid = SST_ASSET_ID_X509_CERT_LARGE;
    struct tfm_sst_attribs_t asset_attrs = {0};
    struct tfm_sst_buf_t data;
    enum tfm_sst_err_t err;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXX";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions
     * (REFERENCE, READ, WRITE)
     */
    app_id = S_APP_ID;

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

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the read function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for application S_APP_ID");
        return;
    }

    if (memcmp(read_data, "DATA", WRITE_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer contains incorrect data");
        return;
    }

    /* Gets asset's attributes. It should succeed as the app ID has REFERENCE
     * permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should read the attributes of this "
                  "file");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Asset current size should be write buffer size");
        return;
    }

    if (asset_attrs.size_max != SST_ASSET_MAX_SIZE_X509_CERT_LARGE) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions (REFERENCE) */
    app_id = S_APP_ID;

    err = tfm_sst_veneer_get_handle(app_id, asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should not fail");
        return;
    }

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to write into this "
                  "file");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the read function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to read this file");
        return;
    }

    /* Resets asset attributes structure values*/
    asset_attrs.size_current = 0;
    asset_attrs.size_max = 0;

    /* Gets asset's attributes. It should succeed as the app ID has REFERENCE
     * permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should read the attributes of this "
                  "file");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Asset current size should be write buffer size");
        return;
    }

    if (asset_attrs.size_max != SST_ASSET_MAX_SIZE_X509_CERT_LARGE) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions
     * (REFERENCE, READ)
     */
    app_id = S_APP_ID;

    err = tfm_sst_veneer_get_handle(app_id, asset_uuid, &hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Get handle should not fail");
        return;
    }

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to write into this "
                  "file");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Resets read data contain */
    memset(read_data, 'X', READ_BUF_SIZE);

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID is allowed to read this file");
        return;
    }

    if (memcmp(read_data, "DATA", WRITE_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer contains incorrect data");
        return;
    }

    /* Resets asset attributes structure values*/
    asset_attrs.size_current = 0;
    asset_attrs.size_max = 0;

    /* Gets asset's attributes. It should succeed as the app ID has REFERENCE
     * permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should read the attributes of this "
                  "file");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Asset current size should be write buffer size");
        return;
    }

    if (asset_attrs.size_max != SST_ASSET_MAX_SIZE_X509_CERT_LARGE) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions to delete */
    app_id = S_APP_ID;

    /* Checks write permissions in delete function*/
    err = tfm_sst_veneer_delete(app_id, hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for application S_APP_ID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests policy against SST_ASSET_ID_SHA224_HASH with the following
 *        permissions
 *        APP_ID       | Permissions
 *        -------------------------------------
 *        S_APP_ID | NONE
 *        S_APP_ID | REFERENCE, READ, WRITE
 *        S_APP_ID | NONE
 */
static void tfm_sst_test_4002(struct test_result_t *ret)
{
    uint32_t hdl;
    uint32_t tmp_hdl = 0;
    uint32_t app_id;
    const uint16_t asset_uuid = SST_ASSET_ID_SHA224_HASH;
    struct tfm_sst_attribs_t asset_attrs = {0};
    struct tfm_sst_buf_t data;
    enum tfm_sst_err_t err;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXX";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions
     * (REFERENCE, READ, WRITE)
     */
    app_id = S_APP_ID;

    /* Checks write permissions in create function*/
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

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the read function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Read should not fail for application S_APP_ID");
        return;
    }

    if (memcmp(read_data, "DATA", WRITE_BUF_SIZE) != 0) {
        TEST_FAIL("Read buffer contains incorrect data");
        return;
    }

    /* Gets asset's attributes. It should succeed as the app ID has REFERENCE
     * permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should read the attributes of this "
                  "file");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Asset current size should be write buffer size");
        return;
    }

    if (asset_attrs.size_max != SST_ASSET_MAX_SIZE_SHA224_HASH) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions (NONE) */
    app_id = S_APP_ID;

    err = tfm_sst_veneer_get_handle(app_id, asset_uuid, &tmp_hdl);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Get handle should fail");
        return;
    }

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to write into this "
                  "file");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the read function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to read this file");
        return;
    }

    /* Gets asset's attributes. It should fail as the app ID doesn't have
     * REFERENCE permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should not be able to read the "
                  "attributes of this file as it doesn't have permissions");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions (NONE) */
    app_id = S_APP_ID;

    err = tfm_sst_veneer_get_handle(app_id, asset_uuid, &tmp_hdl);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Get handle should fail");
        return;
    }

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to write into this "
                  "file");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the read function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to read this file");
        return;
    }

    /* Gets asset's attributes. It should fail as the app ID doesn't have
     * REFERENCE permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should not be able to read the "
                  "attributes of this file as it doesn't have permissions");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions to delete */
    app_id = S_APP_ID;

    /* Checks write permissions in delete function*/
    err = tfm_sst_veneer_delete(app_id, hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for application S_APP_ID");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests policy against SST_ASSET_ID_SHA384_HASH with the following
 *        permissions
 *        APP_ID       | Permissions
 *        -------------------------------
 *        S_APP_ID | NONE
 *        S_APP_ID | NONE
 *        S_APP_ID | REFERENCE, WRITE
 */
static void tfm_sst_test_4003(struct test_result_t *ret)
{

    uint32_t app_id;
    const uint16_t asset_uuid = SST_ASSET_ID_SHA384_HASH;
    struct tfm_sst_attribs_t asset_attrs = {0};
    struct tfm_sst_buf_t data;
    enum tfm_sst_err_t err;
    uint32_t hdl;
    uint32_t tmp_hdl;
    uint8_t wrt_data[WRITE_BUF_SIZE] = "DATA";
    uint8_t read_data[READ_BUF_SIZE] = "XXXXXXXXXXX";

    /* Prepares test context */
    if (prepare_test_ctx(ret) != 0) {
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions
     * (REFERENCE, WRITE)
     */
    app_id = S_APP_ID;

    /* Checks write permissions in create function*/
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

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Write should not fail for application S_APP_ID");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the read function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID doesn't have read permissions");
        return;
    }

    /* Gets asset's attributes. It should succeed as the app ID has REFERENCE
     * permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should read the attributes of this "
                  "file");
        return;
    }

    /* Checks attributes */
    if (asset_attrs.size_current != WRITE_BUF_SIZE) {
        TEST_FAIL("Asset current size should be write buffer size");
        return;
    }

    if (asset_attrs.size_max != SST_ASSET_MAX_SIZE_SHA384_HASH) {
        TEST_FAIL("Max size of the asset is incorrect");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions (NONE) */
    app_id = S_APP_ID;

    err = tfm_sst_veneer_get_handle(app_id, asset_uuid, &tmp_hdl);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Get handle should fail");
        return;
    }

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to write into this "
                  "file");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the read function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to read this file");
        return;
    }

    /* Gets asset's attributes. It should fail as the app ID doesn't have
     * REFERENCE permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should not be able to read the "
                  "attributes of this file as it doesn't have permissions");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions (NONE) */
    app_id = S_APP_ID;

    err = tfm_sst_veneer_get_handle(app_id, asset_uuid, &tmp_hdl);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Get handle should fail");
        return;
    }

    /* Sets data structure */
    data.data = wrt_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the write function */
    err = tfm_sst_veneer_write(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to write into this "
                  "file");
        return;
    }

    /* Sets data structure */
    data.data = read_data;
    data.size = WRITE_BUF_SIZE;
    data.offset = 0;

    /* Checks write permissions in the read function */
    err = tfm_sst_veneer_read(app_id, hdl, &data);
    if (err != TFM_SST_ERR_ASSET_NOT_FOUND) {
        TEST_FAIL("Application S_APP_ID is not allowed to read this file");
        return;
    }

    /* Gets asset's attributes. It should fail as the app ID doesn't have
     * REFERENCE permissions.
     */
    err = tfm_sst_veneer_get_attributes(app_id, hdl, &asset_attrs);
    if (err == TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Application S_APP_ID should not be able to read the "
                  "attributes of this file as it doesn't have permissions");
        return;
    }

    /* Sets application ID S_APP_ID and check its permissions to delete */
    app_id = S_APP_ID;

    /* Checks write permissions in delete function*/
    err = tfm_sst_veneer_delete(app_id, hdl);
    if (err != TFM_SST_ERR_SUCCESS) {
        TEST_FAIL("Delete should not fail for application S_APP_ID");
        return;
    }

    ret->val = TEST_PASSED;
}
