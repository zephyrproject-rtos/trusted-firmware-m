/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test/framework/helpers.h"
#include "tfm_log_api.h"
#include "log_s_tests.h"
#include "tfm_api.h"
#include "tfm_log_veneers.h"

/*!
 * \def STR(a)
 *
 * \brief A standard stringify macro
 */
#define STR(a) _STR(a)
#define _STR(a) #a

/*!
 * \def STANDARD_LOG_ENTRY_SIZE
 *
 * \brief A log item with no payload (standard size) has
 *        a size of 36 bytes. More details can be found
 *        observing \ref tfm_log_line
 *        \ref tfm_log_tlr and \ref tfm_log_hdr
 */
#define STANDARD_LOG_ENTRY_SIZE (36)

/*!
 * \def LOCAL_BUFFER_SIZE
 *
 * \brief Size in bytes of the local buffer. Size accomodates
 *        two standard size (no payload) log items
 */
#define LOCAL_BUFFER_SIZE (72)

/*!
 * \def LOCAL_BUFFER_ITEMS
 *
 * \brief Number of items which can be held within a buffer of
 *        size LOCAL_BUFFER_SIZE
 */
#define LOCAL_BUFFER_ITEMS (2)

/*!
 * \def INITIAL_LOGGING_REQUESTS
 *
 * \brief Number of initial consecutive logging requests to
 *        perform
 */
#define INITIAL_LOGGING_REQUESTS (28)

/*!
 * \def INITIAL_LOGGING_SIZE
 *
 * \brief Size of the initial consecutive logging requests
 */
#define INITIAL_LOGGING_SIZE (1008)

/*!
 * \def TEST_FUNCTION_ID
 *
 * \brief The log line is initialized with a function ID
 *        which corresponds to the requester function ID, i.e.
 *        a dummy test function ID
 */
#define DUMMY_TEST_FUNCTION_ID (0xABCDABCD)

/*!
 * \def BASE_RETRIEVAL_LOG_INDEX
 *
 * \brief Base index from where to start elements retrieval
 */
#define BASE_RETRIEVAL_LOG_INDEX (6)

/*!
 * \def FIRST_RETRIEVAL_LOG_INDEX
 *
 * \brief Index of the first element in the log
 */
#define FIRST_RETRIEVAL_LOG_INDEX (0)

/*!
 * \def MAX_LOG_LINE_SIZE
 *
 * \brief The maximum possible size to fill a 1024 bytes
 *        log
 */
#define MAX_LOG_LINE_SIZE (1008)

/* List of tests */
static void tfm_log_test_1001(struct test_result_t *ret);

static struct test_t log_veneers_tests[] = {
    {&tfm_log_test_1001, "TFM_LOG_TEST_1001",
     "Secure functional", {0} },
};

void register_testsuite_s_log_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(log_veneers_tests) /
                 sizeof(log_veneers_tests[0]));

    set_testsuite("Audit Logging secure interface test (TFM_LOG_TEST_1XXX)",
                  log_veneers_tests, list_size, p_test_suite);
}

/**
 * \brief Functional test of the Secure interface
 *
 * \note This is a functional test only and doesn't
 *       mean to test all possible combinations of
 *       input parameters and return values.
 *       This tests will leave the log in a certain
 *       status which, in turn, will be evaluated by
 *       the Non Secure functional tests. If any tests
 *       are added here that will leave the log in a
 *       different state, Non Secure functional tests
 *       need to be amended accordingly.
 */
static void tfm_log_test_1001(struct test_result_t *ret)
{
    enum tfm_log_err err;
    uint8_t local_buffer[LOCAL_BUFFER_SIZE], idx = 0;
    struct tfm_log_line *line = (struct tfm_log_line *)
                                                  &local_buffer[0];
    struct tfm_log_info info;
    struct tfm_log_line *retrieved_buffer;

    /* Fill the log with 28 lines, each line is 36 bytes
     * we end up filling the log without wrapping
     */
    for (idx=0; idx<INITIAL_LOGGING_REQUESTS; idx++) {
        line->size = sizeof(struct tfm_log_line) - 4;
        line->function_id = DUMMY_TEST_FUNCTION_ID;
        line->arg[0] = 0x1 + idx*10;
        line->arg[1] = 0x2 + idx*10;
        line->arg[2] = 0x3 + idx*10;
        line->arg[3] = 0x4 + idx*10;

        /* The line doesn't contain any payload */
        err = tfm_log_veneer_add_line(line);
        if (err != TFM_LOG_ERR_SUCCESS) {
            TEST_FAIL("Log line addition has returned an error");
            return;
        }
    }

    /* Get the log size */
    err = tfm_log_veneer_get_info(&info);
    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Getting log info has returned error");
        return;
    }

    if (info.size != INITIAL_LOGGING_SIZE) {
        TEST_FAIL("Expected log size is " STR(INITIAL_LOGGING_SIZE));
        return;
    }

    if (info.num_items != INITIAL_LOGGING_REQUESTS) {
        TEST_FAIL("Expected log items are " STR(INITIAL_LOGGING_REQUESTS));
        return;
    }

    /* Retrieve two log items starting from a given index */
    err = tfm_log_veneer_retrieve(LOCAL_BUFFER_SIZE,
                                  BASE_RETRIEVAL_LOG_INDEX,
                                  &local_buffer[0],
                                  &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Retrieve indexes 6 and 7 has returned an error");
        return;
    }

    if (info.size != LOCAL_BUFFER_SIZE) {
        TEST_FAIL("Expected log size is " STR(LOCAL_BUFFER_SIZE));
        return;
    }

    if (info.num_items != LOCAL_BUFFER_ITEMS) {
        TEST_FAIL("Expected log items are " STR(LOCAL_BUFFER_ITEMS));
        return;
    }

    /* Inspect the content of the second log line retrieved */
    retrieved_buffer = (struct tfm_log_line *)
                           &local_buffer[8+STANDARD_LOG_ENTRY_SIZE];

    if (retrieved_buffer->arg[0] != (0x1 + (BASE_RETRIEVAL_LOG_INDEX+1)*10)) {
        TEST_FAIL("Unexpected argument in the index 7 entry");
        return;
    }

    /* Retrieve the last two log items */
    err = tfm_log_veneer_retrieve(LOCAL_BUFFER_SIZE,
                                  TFM_LOG_READ_RECENT,
                                  &local_buffer[0],
                                  &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Retrieve of last two log entries has returned error");
        return;
    }

    /* Inspect the first line retrieved in the local buffer */
    retrieved_buffer = (struct tfm_log_line *) &local_buffer[8];

    if (info.size != LOCAL_BUFFER_SIZE) {
        TEST_FAIL("Expected log size is " STR(LOCAL_BUFFER_SIZE));
        return;
    }

    if (info.num_items != LOCAL_BUFFER_ITEMS) {
        TEST_FAIL("Expected log items are " STR(LOCAL_BUFFER_ITEMS));
        return;
    }

    if (retrieved_buffer->arg[0] != (0x1 + (INITIAL_LOGGING_REQUESTS-2)*10)) {
        TEST_FAIL("Unexpected argument in the second last entry");
        return;
    }

    /* Retrieve the first log item */
    err = tfm_log_veneer_retrieve(STANDARD_LOG_ENTRY_SIZE,
                                  FIRST_RETRIEVAL_LOG_INDEX,
                                  &local_buffer[0],
                                  &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Retrieve of the first log entry has returned error");
        return;
    }

    if (info.size != STANDARD_LOG_ENTRY_SIZE) {
        TEST_FAIL("Expected log size is " STR(STANDARD_LOG_ENTRY_SIZE));
        return;
    }

    if (info.num_items != 1) {
        TEST_FAIL("Number of items read is different from 1");
        return;
    }

    if (retrieved_buffer->arg[0] != (0x1 + FIRST_RETRIEVAL_LOG_INDEX*10)) {
        TEST_FAIL("Unexpected argument in the first entry");
        return;
    }

    /* Retrieve the last log item. Provide a buffer size which is slightly
     * bigger than the size of a single log entry
     */
    err = tfm_log_veneer_retrieve(STANDARD_LOG_ENTRY_SIZE+4,
                                  TFM_LOG_READ_RECENT,
                                  &local_buffer[0],
                                  &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Retrieve of last two log entries has returned error");
        return;
    }

    if (info.size != STANDARD_LOG_ENTRY_SIZE) {
        TEST_FAIL("Expected log size is " STR(STANDARD_LOG_ENTRY_SIZE));
        return;
    }

    if (info.num_items != 1) {
        TEST_FAIL("Number of items read is different from 1");
        return;
    }

    if (retrieved_buffer->arg[0] != (0x1 + (INITIAL_LOGGING_REQUESTS-1)*10)) {
        TEST_FAIL("Unexpected argument in the second last entry");
        return;
    }

    /* Fill one more log line, this will wrap */
    line->size = sizeof(struct tfm_log_line) - 4;
    line->function_id = DUMMY_TEST_FUNCTION_ID;
    line->arg[0] = 0x1 + INITIAL_LOGGING_REQUESTS*10;
    line->arg[1] = 0x2 + INITIAL_LOGGING_REQUESTS*10;
    line->arg[2] = 0x3 + INITIAL_LOGGING_REQUESTS*10;
    line->arg[3] = 0x4 + INITIAL_LOGGING_REQUESTS*10;

    /* The addition of this new log item will wrap the log ending */
    err = tfm_log_veneer_add_line(line);
    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log line addition has returned an error");
        return;
    }

    /* Retrieve the last two items */
    err = tfm_log_veneer_retrieve(LOCAL_BUFFER_SIZE,
                                  TFM_LOG_READ_RECENT,
                                  &local_buffer[0],
                                  &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Retrieve of last two log entries has returned error");
        return;
    }

    if (info.size != LOCAL_BUFFER_SIZE) {
        TEST_FAIL("Expected log size is " STR(LOCAL_BUFFER_SIZE));
        return;
    }

    if (info.num_items != LOCAL_BUFFER_ITEMS) {
        TEST_FAIL("Expected log items are " STR(LOCAL_BUFFER_ITEMS));
        return;
    }

    if (retrieved_buffer->arg[0] != (0x1 + (INITIAL_LOGGING_REQUESTS-1)*10)) {
        TEST_FAIL("Unexpected argument in the second last entry");
        return;
    }

    /* Inspect the second line retrieved in the local buffer */
    retrieved_buffer = (struct tfm_log_line *)
                           &local_buffer[8+STANDARD_LOG_ENTRY_SIZE];

    if (retrieved_buffer->arg[0] != (0x1 + INITIAL_LOGGING_REQUESTS*10)) {
        TEST_FAIL("Unexpected argument in the last entry");
        return;
    }

    /* Fill now one big line that will invalidate all existing lines */
    line->size = MAX_LOG_LINE_SIZE;
    line->function_id = DUMMY_TEST_FUNCTION_ID;
    line->arg[0] = 0x1 + ((INITIAL_LOGGING_REQUESTS+1)*10);
    line->arg[1] = 0x2 + ((INITIAL_LOGGING_REQUESTS+1)*10);
    line->arg[2] = 0x3 + ((INITIAL_LOGGING_REQUESTS+1)*10);
    line->arg[3] = 0x4 + ((INITIAL_LOGGING_REQUESTS+1)*10);

    /* The line has maximum possible payload for log size of 1024 */
    err = tfm_log_veneer_add_line(line);
    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log line addition has returned an error");
        return;
    }

    /* Try to retrieve the maximum possible size that fits our buffer.
     * As there is just one big line filling the whole space, nothing
     * will be returned
     */
    err = tfm_log_veneer_retrieve(LOCAL_BUFFER_SIZE,
                                  TFM_LOG_READ_RECENT,
                                  &local_buffer[0],
                                  &info);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Retrieve of last two log entries has returned error");
        return;
    }

    if (info.size != 0) {
        TEST_FAIL("Retrieved log size has unexpected size instead of 0");
        return;
    }

    if (info.num_items != 0) {
        TEST_FAIL("Number of items read is different from 0");
        return;
    }

    /* Add two standard length lines again */
    for (idx=0; idx<2; idx++) {
        line->size = sizeof(struct tfm_log_line) - 4;
        line->function_id = DUMMY_TEST_FUNCTION_ID;
        line->arg[0] = 0x1 + (INITIAL_LOGGING_REQUESTS+2+idx)*10;
        line->arg[1] = 0x2 + (INITIAL_LOGGING_REQUESTS+2+idx)*10;
        line->arg[2] = 0x3 + (INITIAL_LOGGING_REQUESTS+2+idx)*10;
        line->arg[3] = 0x4 + (INITIAL_LOGGING_REQUESTS+2+idx)*10;

        /* The line doesn't contain any payload */
        err = tfm_log_veneer_add_line(line);
        if (err != TFM_LOG_ERR_SUCCESS) {
            TEST_FAIL("Log line addition has returned an error");
            return;
        }
    }

    ret->val = TEST_PASSED;
}
