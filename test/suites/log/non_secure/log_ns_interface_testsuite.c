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

/* List of tests */
static void tfm_log_test_1001(struct test_result_t *ret);

static struct test_t log_veneers_tests[] = {
    {&tfm_log_test_1001, "TFM_LOG_TEST_1001",
     "Retrieve log", {0} },
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
 * \brief Use the NS API to retrieve the log
 */
static void tfm_log_test_1001(struct test_result_t *ret)
{
    enum tfm_log_err err;

    uint8_t local_buffer[4];
    uint32_t size, log_size;

    /* Retrieve 4 bytes at most */
    size = 4;

    err = tfm_log_retrieve(size, &local_buffer[0], &log_size);

    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log retrieval has returned an error");
        return;
    }

    ret->val = TEST_PASSED;
}
