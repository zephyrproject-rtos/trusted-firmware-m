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

/* List of tests */
static void tfm_log_test_1001(struct test_result_t *ret);

static struct test_t log_veneers_tests[] = {
    {&tfm_log_test_1001, "TFM_LOG_TEST_1001",
     "Add an entry to the log", {0} },
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
 * \brief Use the secure only API to add an entry to the log
 */
static void tfm_log_test_1001(struct test_result_t *ret)
{
    enum tfm_log_err err;
    uint8_t local_buffer[24];
    struct tfm_log_line *line = (struct tfm_log_line *)
                                                  &local_buffer[0];

    /* Fill the log_line with random values */
    line->size = 20;
    line->function_id = 0xABCDABCD;
    line->arg[0] = 0x0;
    line->arg[1] = 0x1;
    line->arg[2] = 0x2;
    line->arg[3] = 0x3;

    /* The line doesn't contain any payload */
    err = tfm_log_veneer_add_line(line);
    if (err != TFM_LOG_ERR_SUCCESS) {
        TEST_FAIL("Log line addition has returned an error");
        return;
    }

    ret->val = TEST_PASSED;
}
