/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "ipc_ns_tests.h"
#include "psa_client.h"
#include "test/framework/test_framework_helpers.h"

#define IPC_TEST_MIN_SID        (01)

/* List of tests */
static void tfm_ipc_test_1001(struct test_result_t *ret);

static struct test_t ipc_veneers_tests[] = {
    {&tfm_ipc_test_1001, "TFM_IPC_TEST_1001", "Non Secure functional", {0} },
};

void register_testsuite_ns_ipc_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(ipc_veneers_tests) / sizeof(ipc_veneers_tests[0]));

    set_testsuite("IPC non-secure interface test (TFM_IPC_TEST_1XXX)",
                  ipc_veneers_tests, list_size, p_test_suite);
}

/**
 * \brief Functional test of NS API
 * \note This is a functional test only and doesn't
 *       mean to test all possible combinations of
 *       input parameters and return values.
 */
static void tfm_ipc_test_1001(struct test_result_t *ret)
{
    uint32_t min_version;

    min_version = psa_version(IPC_TEST_MIN_SID);
    printf("TFM service support min version is %d.\r\n", min_version);
    ret->val = TEST_PASSED;
}
