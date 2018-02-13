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
#include "test/suites/sst/secure/sst_tests.h"
#include "test/suites/invert/secure/invert_s_tests.h"
#include "secure_fw/services/secure_storage/sst_core_interface.h"

static struct test_suite_t test_suites[] = {
#ifdef SERVICES_TEST_S
    /* Secure SST test cases */
    {&register_testsuite_s_sst_sec_interface, 0, 0, 0},
    {&register_testsuite_s_sst_reliability, 0, 0, 0},

#ifdef TFM_PARTITION_TEST_CORE
    /* Secure invert test cases */
    /* Note: since this is sample code, only run if test services are enabled */
    {&register_testsuite_s_invert_interface, 0, 0, 0},
#endif
#endif /* SERVICES_TEST_S */
};

static void setup_integ_test(void)
{
    /* Left empty intentionally, currently implemented
     * test suites require no setup
     */
}

static void tear_down_integ_test(void)
{
    /* Leave the SST area clean after execute the tests */
    sst_object_wipe_all();
    sst_object_prepare();
}

void start_integ_test(void)
{
    setup_integ_test();
    integ_test("Secure",
               test_suites,
               sizeof(test_suites)/sizeof(test_suites[0]));
    tear_down_integ_test();
}
