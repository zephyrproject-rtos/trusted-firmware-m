/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "ipc_ns_tests.h"
#include "psa_client.h"
#include "test/framework/test_framework_helpers.h"

#define IPC_TEST_SERVICE1_SID        (0x1000)
#define IPC_TEST_SERVICE1_MIN_VER    (0x0001)

/* List of tests */
static void tfm_ipc_test_1001(struct test_result_t *ret);
static void tfm_ipc_test_1002(struct test_result_t *ret);
static void tfm_ipc_test_1003(struct test_result_t *ret);
static void tfm_ipc_test_1004(struct test_result_t *ret);

static struct test_t ipc_veneers_tests[] = {
    {&tfm_ipc_test_1001, "TFM_IPC_TEST_1001",
     "Get PSA framework version", {0} },
    {&tfm_ipc_test_1002, "TFM_IPC_TEST_1002",
     "Get minor version of a RoT Service", {0} },
    {&tfm_ipc_test_1003, "TFM_IPC_TEST_1003",
     "Connect to a RoT Service", {0} },
    {&tfm_ipc_test_1004, "TFM_IPC_TEST_1004",
     "Call a RoT Service", {0} }
};

void register_testsuite_ns_ipc_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(ipc_veneers_tests) / sizeof(ipc_veneers_tests[0]));

    set_testsuite("IPC non-secure interface test (TFM_IPC_TEST_1XXX)",
                  ipc_veneers_tests, list_size, p_test_suite);
}

/**
 * \brief Retrieve the version of the PSA Framework API.
 *
 * \note This is a functional test only and doesn't
 *       mean to test all possible combinations of
 *       input parameters and return values.
 */
static void tfm_ipc_test_1001(struct test_result_t *ret)
{
    uint32_t version;

    version = psa_framework_version();
    if (version == PSA_FRAMEWORK_VERSION) {
        TEST_LOG("The version of the PSA Framework API is %d.\r\n", version);
    } else {
        TEST_FAIL("The version of the PSA Framework API is not valid!\r\n");
        return;
    }
}

/**
 * \brief Retrieve the minor version of a RoT Service.
 */
static void tfm_ipc_test_1002(struct test_result_t *ret)
{
    uint32_t version;

    version = psa_version(IPC_TEST_SERVICE1_SID);
    if (version == PSA_VERSION_NONE) {
        TEST_FAIL("RoT Service is not implemented or caller is not authorized" \
                  "to access it!\r\n");
        return;
    } else {
        /* Valid version number */
        TEST_LOG("The minor version is %d.\r\n", version);
    }
    ret->val = TEST_PASSED;
}

/**
 * \brief Connect to a RoT Service by its SID.
 */
static void tfm_ipc_test_1003(struct test_result_t *ret)
{
    psa_handle_t handle;

    handle = psa_connect(IPC_TEST_SERVICE1_SID, IPC_TEST_SERVICE1_MIN_VER);
    if (handle > 0) {
        TEST_LOG("Connect success!\r\n");
    } else {
        TEST_FAIL("The RoT Service has refused the connection!\r\n");
        return;
    }
    psa_close(handle);
    ret->val = TEST_PASSED;
}

/**
 * \brief Call a RoT Service.
 */
static void tfm_ipc_test_1004(struct test_result_t *ret)
{
    char str1[] = "str1";
    char str2[] = "str2";
    char str3[128], str4[128];
    struct psa_invec invecs[2] = {{str1, sizeof(str1)/sizeof(char)},
                                  {str2, sizeof(str2)/sizeof(char)}};
    struct psa_outvec outvecs[2] = {{str3, sizeof(str3)/sizeof(char)},
                                    {str4, sizeof(str4)/sizeof(char)}};
    psa_handle_t handle;
    psa_status_t status;
    uint32_t min_version;

    min_version = psa_version(IPC_TEST_SERVICE1_SID);
    TEST_LOG("TFM service support minor version is %d.\r\n", min_version);
    handle = psa_connect(IPC_TEST_SERVICE1_SID, IPC_TEST_SERVICE1_MIN_VER);
    status = psa_call(handle, invecs, 2, outvecs, 2);
    if (status >= 0) {
        TEST_LOG("psa_call is successful!\r\n");
    } else if (status == PSA_DROP_CONNECTION) {
        TEST_FAIL("The connection has been dropped by the RoT Service!\r\n");
        return;
    } else {
        TEST_FAIL("psa_call is failed!\r\n");
        return;
    }
    TEST_LOG("outvec1 is: %s\r\n", outvecs[0].base);
    TEST_LOG("outvec2 is: %s\r\n", outvecs[1].base);
    psa_close(handle);
    ret->val = TEST_PASSED;
}
