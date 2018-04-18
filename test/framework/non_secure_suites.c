/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "integ_test.h"
#include "integ_test_helper.h"
#include "test_framework.h"

/* Service specific includes */
#include "test/suites/sst/non_secure/sst_ns_tests.h"
#include "test/suites/log/non_secure/log_ns_tests.h"
#include "test/suites/invert/non_secure/invert_ns_tests.h"
#include "test/suites/core/non_secure/core_ns_tests.h"

static struct test_suite_t test_suites[] = {
#ifdef SERVICES_TEST_NS
    /* Non-secure SST test cases */
    {&register_testsuite_ns_sst_interface, 0, 0, 0},
    {&register_testsuite_ns_sst_policy, 0, 0, 0},

#ifdef TFM_PARTITION_TEST_SST
    /* Non-secure SST referenced access testsuite */
    {&register_testsuite_ns_sst_ref_access, 0, 0, 0},
#endif

    /* Non-secure LOG test cases */
    {&register_testsuite_ns_log_interface, 0, 0, 0},

#ifdef TFM_PARTITION_TEST_CORE
    /* Non-secure invert test cases */
    /* Note: since this is sample code, only run if test services are enabled */
    {&register_testsuite_ns_invert_interface, 0, 0, 0},
#endif
#endif

#ifdef CORE_TEST_POSITIVE
    /* Non-secure core test cases */
    {&register_testsuite_ns_core_positive, 0, 0, 0},
#endif

#ifdef CORE_TEST_INTERACTIVE
    /* Non-secure interactive test cases */
    {&register_testsuite_ns_core_interactive, 0, 0, 0},
#endif
};

void start_integ_test(void)
{
    integ_test("Non-secure", test_suites,
               sizeof(test_suites)/sizeof(test_suites[0]));
}

/* Service stand-in for NS tests. To be called from a non-secure context */
void tfm_non_secure_client_run_tests(void)
{
    start_integ_test();
}
