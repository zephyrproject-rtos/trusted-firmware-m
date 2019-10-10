/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/client.h"
#include "psa_manifest/sid.h"
#include "test/framework/test_framework_helpers.h"

/* List of tests */
static void tfm_multi_core_dummy_test(struct test_result_t *ret);

static struct test_t multi_core_tests[] = {
    {&tfm_multi_core_dummy_test, "TFM_MULTI_CORE_TEST_DUMMY_TEST",
     "A dummy test in multi-core test service", {0}},
};

void register_testsuite_multi_core_ns_interface(
                                              struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(multi_core_tests) / sizeof(multi_core_tests[0]));

    set_testsuite("TF-M test cases for multi-core topology",
                  multi_core_tests, list_size, p_test_suite);
}

/**
 * \brief A dummy test case. It will be replaced by actual test cases.
 */
static void tfm_multi_core_dummy_test(struct test_result_t *ret)
{
    TEST_LOG("A fake test and of course it succeeds\r\n");

    ret->val = TEST_PASSED;
}
