/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "invert_ns_tests.h"
#include "tfm_api.h"
#include "test/test_services/tfm_core_test_2/tfm_ss_core_test_2_veneers.h"
#include "test/suites/core/non_secure/core_test_api.h"

#include <stdio.h>
#include <string.h>

/* Define test suite for asset manager tests */
/* List of tests */
static void tfm_invert_test_1001(struct test_result_t *ret);
static void tfm_invert_test_1002(struct test_result_t *ret);

static struct test_t invert_veeners_tests[] = {
    {&tfm_invert_test_1001, "TFM_INVERT_TEST_1001",
     "Invert with valid buffer", {0} },
    {&tfm_invert_test_1002, "TFM_INVERT_TEST_1002",
     "Invert with invalid buffer", {0} },
};

void register_testsuite_ns_invert_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(invert_veeners_tests) /
                 sizeof(invert_veeners_tests[0]));

    set_testsuite("Invert non-secure interface tests (TFM_INVERT_TEST_1XXX)",
                  invert_veeners_tests, list_size, p_test_suite);
}

/**
 * \brief Tests invert function with valid length
 */
static void tfm_invert_test_1001(struct test_result_t *ret)
{
    int32_t err;
    int32_t res;
    int32_t len;
    int32_t i;

    uint32_t in_ptr[] = {0xBADC0FFEU, 0xDEADBEEFU};
    uint32_t out_ptr[sizeof(in_ptr) / sizeof(in_ptr[0])];

    memset(out_ptr, 0, sizeof(out_ptr));

    len = sizeof(in_ptr)/sizeof(in_ptr[0]);

    int32_t args[] = {(int32_t)&res, (int32_t)in_ptr,
                      (int32_t)out_ptr, (int32_t)len};

    err = tfm_core_test_call(tfm_core_test_2_sfn_invert, args);

    if (err != TFM_SUCCESS) {
        TEST_FAIL("Call to secure service should be successful");
        return;
    }

    if (res != 0) {
        TEST_FAIL("Invert should succeed on small buffer");
        return;
    }

    for (i = 0; i < len; ++i) {
        if (~in_ptr[i] != out_ptr[i]) {
            TEST_FAIL("Output should be the inverted version of input");
            return;
        }
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests invert function with invalid length
 */
static void tfm_invert_test_1002(struct test_result_t *ret)
{
    int32_t err;
    int32_t res;
    uint32_t *in_ptr = (uint32_t *)0xFFFFFFFFU;
    uint32_t *out_ptr = in_ptr;

    int32_t len = 1024;

    int32_t args[] = {(int32_t)&res, (int32_t)in_ptr,
                                     (int32_t)out_ptr, (int32_t)len};

    err = tfm_core_test_call(tfm_core_test_2_sfn_invert, args);

    if (err == TFM_SUCCESS) {
        TEST_FAIL("Call to secure service should not be successful");
        return;
    }

    if (res != -1) {
        TEST_FAIL("Invert should fail with large buffer. Unexpected result");
        return;
    }

    ret->val = TEST_PASSED;
}
