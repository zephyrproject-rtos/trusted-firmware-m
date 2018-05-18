/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test/framework/helpers.h"
#include "tfm_log_api.h"
#include "log_ns_tests.h"
#include "tfm_api.h"
#include "secure_fw/services/audit_logging/log_core.h"

#include "../log_tests_common.h"

/*!
 * \def EMPTY_RETRIEVED_LOG_SIZE
 *
 * \brief Log size when the retrieved buffer is empty
 */
#define EMPTY_RETRIEVED_LOG_SIZE (0)

/*!
 * \def EMPTY_RETRIEVED_LOG_ITEMS
 *
 * \brief Number of log items when retrieved buffer is empty
 */
#define EMPTY_RETRIEVED_LOG_ITEMS (0)

/*!
 * \def SINGLE_RETRIEVED_LOG_SIZE
 *
 * \brief Log size when the retrieved buffer has 1 item
 *        of standard size (no payload)
 */
#define SINGLE_RETRIEVED_LOG_SIZE (STANDARD_LOG_ENTRY_SIZE)

/*!
 * \def SINGLE_RETRIEVED_LOG_ITEMS
 *
 * \brief Number of log items when retrieved buffer has 1 item
 */
#define SINGLE_RETRIEVED_LOG_ITEMS (1)

/*!
 * \def SECOND_ELEMENT_START_INDEX
 *
 * \brief Index of the second item in the log
 */
#define SECOND_ELEMENT_START_INDEX (1)

/* List of tests */
static void tfm_log_test_1001(struct test_result_t *ret);

static struct test_t log_veneers_tests[] = {
    {&tfm_log_test_1001, "TFM_LOG_TEST_1001",
     "Non Secure functional", {0} },
};

void register_testsuite_ns_log_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(log_veneers_tests) /
                 sizeof(log_veneers_tests[0]));

    set_testsuite("Audit Logging non-secure interface test (TFM_LOG_TEST_1XXX)",
                  log_veneers_tests, list_size, p_test_suite);
}

/**
 * \brief Functional test of NS API
 *
 * \note This is a functional test only and doesn't
 *       mean to test all possible combinations of
 *       input parameters and return values.
 *       This tests the current status of the log as
 *       it's been left from the Secure tests. In case
 *       other tests are added in the Secure test suite,
 *       the status of the log will change and these
 *       tests may start failing.
 */
static void tfm_log_test_1001(struct test_result_t *ret)
{
    enum tfm_log_err err;

    uint8_t local_buffer[LOCAL_BUFFER_SIZE];
    uint32_t size, rem_items;

    struct tfm_log_info info;
    struct tfm_log_line *retrieved_buffer;

    /* Get the log size (current state) */
    err = tfm_log_get_info(&info);
    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    if (info.size != INITIAL_LOG_SIZE) {
        TEST_FAIL("Stored size different from " STR(INITIAL_LOG_SIZE));
        return;
    }

    if (info.num_items != INITIAL_LOG_ITEMS) {
        TEST_FAIL("Stored items number different from " STR(INITIAL_LOG_ITEMS));
        return;
    }

    /* Log contains 2 items. Retrieve into buffer which is able to contain the
     * the full contents of the log
     */
    size = INITIAL_LOG_SIZE;

    err = tfm_log_retrieve(size,
                           TFM_LOG_READ_RECENT,
                           &local_buffer[0],
                           &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log retrieval from NS returned error");
        return;
    }

    if (info.size != INITIAL_LOG_SIZE) {
        TEST_FAIL("Expected log size to retrieve is " STR(INITIAL_LOG_SIZE));
        return;
    }

    if (info.num_items != INITIAL_LOG_ITEMS) {
        TEST_FAIL("Expected number of items read is " STR(INITIAL_LOG_ITEMS));
        return;
    }

    /* Retrieve into a 16-byte buffer. It's not enough to store a single
     * item so the provided buffer must be empty after retrieval. We
     * check the info structure to count how many items and bytes have
     * been returned, and if they're zeros items / zero bytes, there is
     * no point in checking the contents of the local_buffer.
     */
    size = 16;

    err = tfm_log_retrieve(size,
                           TFM_LOG_READ_RECENT,
                           &local_buffer[0],
                           &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log retrieval from NS returned error");
        return;
    }

    if (info.size != EMPTY_RETRIEVED_LOG_SIZE) {
        TEST_FAIL("Expected log size is " STR(EMPTY_RETRIEVED_LOG_SIZE));
        return;
    }

    if (info.num_items != EMPTY_RETRIEVED_LOG_ITEMS) {
        TEST_FAIL("Expected read items are " STR(EMPTY_RETRIEVED_LOG_ITEMS));
        return;
    }

    /* Retrieve into a 70-byte buffer: only last entry fits and is retrieved */
    size = 70;

    err = tfm_log_retrieve(size,
                           TFM_LOG_READ_RECENT,
                           &local_buffer[0],
                           &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log retrieval from NS returned error");
        return;
    }

    if (info.size != SINGLE_RETRIEVED_LOG_SIZE) {
        TEST_FAIL("Expected log size is " STR(SINGLE_RETRIEVED_LOG_SIZE));
        return;
    }

    if (info.num_items != SINGLE_RETRIEVED_LOG_ITEMS) {
        TEST_FAIL("Expected read items are " STR(SINGLE_RETRIEVED_LOG_ITEMS));
        return;
    }

    /* Retrieve into a buffer which can hold a single element, but start from
     * the second element that is stored in the log
     */
    size = STANDARD_LOG_ENTRY_SIZE;

    err = tfm_log_retrieve(size,
                           SECOND_ELEMENT_START_INDEX,
                           &local_buffer[0],
                           &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log retrieval from NS returned error");
        return;
    }

    if (info.size != SINGLE_RETRIEVED_LOG_SIZE) {
        TEST_FAIL("Expected log size is " STR(SINGLE_RETRIEVED_LOG_SIZE));
        return;
    }

    if (info.num_items != SINGLE_RETRIEVED_LOG_ITEMS) {
        TEST_FAIL("Expected read items are " STR(SINGLE_RETRIEVED_LOG_ITEMS));
        return;
    }

    /* Inspect the contents of the retrieved buffer, i.e. check the
     * retrieved log line contents
     */
    retrieved_buffer = (struct tfm_log_line *)
                           &local_buffer[offsetof(struct log_hdr, size)];

    if (retrieved_buffer->arg[0] != SECOND_ELEMENT_EXPECTED_CONTENT) {
        TEST_FAIL("Unexpected argument in the first entry");
        return;
    }

    /* Delete one element in the log */
    err = tfm_log_delete_items(1, &rem_items);
    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log item deletion from NS returned error");
        return;
    }

    if (rem_items != 1) {
        TEST_FAIL("Unexpected number of deleted items different than 1");
        return;
    }

    /* Try to delete two elements in the log. The log has just one element, so
     * check that the number of deleted items is less than requested
     */
    err = tfm_log_delete_items(2, &rem_items);
    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log item deletion from NS returned error");
        return;
    }

    if (rem_items != 1) {
        TEST_FAIL("Unexpected number of deleted items different than 1");
        return;
    }

    ret->val = TEST_PASSED;
}
