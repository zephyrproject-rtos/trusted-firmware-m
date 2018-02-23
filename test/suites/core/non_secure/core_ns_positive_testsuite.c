/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "core_ns_tests.h"
#include "tfm_api.h"
#include "test/test_services/tfm_core_test/tfm_ss_core_test_veneers.h"
#include "test/test_services/tfm_core_test_2/tfm_ss_core_test_2_veneers.h"
#include "test/suites/core/non_secure/core_test_api.h"
#include "test/test_services/tfm_core_test/core_test_defs.h"

#include <stdio.h>
#include <string.h>

/* Define test suite for core tests */
/* List of tests */

#define TOSTRING(x) #x
#define CORE_TEST_DESCRIPTION(number, fn, description) \
    {fn, "TFM_CORE_TEST_"TOSTRING(number),\
     description, {0} }

static void tfm_core_test_ns_thread(struct test_result_t *ret);
static void tfm_core_test_ns_svc(struct test_result_t *ret);
static void tfm_core_test_check_init(struct test_result_t *ret);
static void tfm_core_test_recursion(struct test_result_t *ret);
static void tfm_core_test_permissions(struct test_result_t *ret);
static void tfm_core_test_mpu_access(struct test_result_t *ret);
static void tfm_core_test_buffer_check(struct test_result_t *ret);
static void tfm_core_test_ss_to_ss(struct test_result_t *ret);
static void tfm_core_test_share_change(struct test_result_t *ret);
static void tfm_core_test_ss_to_ss_buffer(struct test_result_t *ret);
static void tfm_core_test_two_sfn_one_svc(struct test_result_t *ret);
static void tfm_core_test_peripheral_access(struct test_result_t *ret);

static struct test_t core_tests[] = {
CORE_TEST_DESCRIPTION(CORE_TEST_ID_NS_THREAD, tfm_core_test_ns_thread,
    "Test service request from NS thread mode"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_NS_SVC, tfm_core_test_ns_svc,
    "Test service request from NS SVC"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_CHECK_INIT, tfm_core_test_check_init,
    "Test the success of service init"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_RECURSION, tfm_core_test_recursion,
    "Test direct recursion of secure services"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_MEMORY_PERMISSIONS,
    tfm_core_test_permissions,
    "Test secure service memory access permissions"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_MPU_ACCESS, tfm_core_test_mpu_access,
    "Test secure service MPU accesses"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_BUFFER_CHECK, tfm_core_test_buffer_check,
    "Test secure service buffer accesses"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_SS_TO_SS, tfm_core_test_ss_to_ss,
    "Test secure service to service call"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_SHARE_REDIRECTION,
    tfm_core_test_share_change,
    "Test secure service share change request"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_SS_TO_SS_BUFFER,
    tfm_core_test_ss_to_ss_buffer,
    "Test secure service to service call with buffer handling"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_TWO_SFN_ONE_SVC,
    tfm_core_test_two_sfn_one_svc,
    "Test multiple service calls from single SVC"),
CORE_TEST_DESCRIPTION(CORE_TEST_ID_PERIPHERAL_ACCESS,
    tfm_core_test_peripheral_access,
    "Test service peripheral access"),
};

void register_testsuite_ns_core_positive(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(core_tests) / sizeof(core_tests[0]));

    set_testsuite("Core non-secure positive tests (TFM_CORE_TEST_1XXX)",
                  core_tests, list_size, p_test_suite);
}

/* Args that may or may not be used for service calls */
int32_t args[] = {0};

static void tfm_core_test_ns_thread(struct test_result_t *ret)
{
    int32_t err = tfm_core_test_sfn(CORE_TEST_ID_NS_THREAD, 0, 0, 0);

    if (err == TFM_SUCCESS) {
        TEST_FAIL("TFM Core allowed service to run.");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_ns_svc(struct test_result_t *ret)
{
    args[0] = CORE_TEST_ID_NS_SVC;

    int32_t err = tfm_core_test_svc(tfm_core_test_sfn, args);

    if (err != TFM_SUCCESS) {
        TEST_FAIL("Service failed to run.");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_peripheral_access(struct test_result_t *ret)
{
    args[0] = CORE_TEST_ID_PERIPHERAL_ACCESS;

    int32_t err = tfm_core_test_svc(tfm_core_test_sfn, args);

    if ((err != TFM_SUCCESS) && (err < TFM_PARTITION_SPECIFIC_ERROR_MIN)) {
        TEST_FAIL("TFM Core returned error.");
        return;
    }

    switch (err) {
    case CORE_TEST_ERRNO_SUCCESS:
        ret->val = TEST_PASSED;
        return;
    case CORE_TEST_ERRNO_PERIPHERAL_ACCESS_FAILED:
        TEST_FAIL("Service peripheral access failed.");
        return;
    default:
        TEST_FAIL("Unexpected return value received.");
        return;
    }
}

/*
 * \brief Tests whether the initialisation of the service was successful.
 *
 */
static void tfm_core_test_check_init(struct test_result_t *ret)
{
    int32_t err;

    err = tfm_core_test_svc(tfm_core_test_sfn_init_success, args);

    if ((err != TFM_SUCCESS) && (err < TFM_PARTITION_SPECIFIC_ERROR_MIN)) {
        TEST_FAIL("TFM Core returned error.");
        return;
    }

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to initialise test service.");
        return;
    }

    ret->val = TEST_PASSED;
}

/**
 * \brief Tests what happens when a service calls itself directly
 *
 * \return Returns \ref CORE_TEST_ERRNO_SUCCESS
 */
static void tfm_core_test_recursion(struct test_result_t *ret)
{
    int32_t err;

    /* Initialize depth argument to 0 */
    args[0] = 0;
    err = tfm_core_test_svc(tfm_core_test_sfn_direct_recursion, args);

    if (err != TFM_SUCCESS && err < 256) {
        TEST_FAIL("TFM Core returned error.");
        return;
    }

    switch (err) {
    case CORE_TEST_ERRNO_SUCCESS:
        ret->val = TEST_PASSED;
        return;
    case CORE_TEST_ERRNO_SP_RECURSION_NOT_REJECTED:
        TEST_FAIL("TF-M Core failed to reject recursive service call.");
        return;
    default:
        TEST_FAIL("Unexpected return value received.");
        return;
    }
}

static char *error_to_string(const char *desc, int32_t err)
{
    static char info[80];

    sprintf(info, "%s. Error code: %d, extra data: %d",
        desc,
        CORE_TEST_ERROR_GET_CODE(err),
        CORE_TEST_ERROR_GET_EXTRA(err));
    return info;
}

static void tfm_core_test_mpu_access(struct test_result_t *ret)
{
    int32_t err;

    int32_t args[] = {
        CORE_TEST_ID_MPU_ACCESS,
        (int32_t)args,
        /* Make sure the pointer in code segment is word-aligned */
        (int32_t)tfm_core_test_mpu_access & (~(0x3)),
        (int32_t)args};

    err = tfm_core_test_svc(tfm_core_test_sfn, args);

    if (err != TFM_SUCCESS && err < TFM_PARTITION_SPECIFIC_ERROR_MIN) {
        TEST_FAIL("TFM Core returned error.");
        return;
    }

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        char *info = error_to_string(
            "Service memory accesses configured incorrectly.", err);
        TEST_FAIL(info);
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_permissions(struct test_result_t *ret)
{
    int32_t err;

    int32_t args[] = {
        CORE_TEST_ID_MEMORY_PERMISSIONS,
        (int32_t)args,
        /* Make sure the pointer in code segment is word-aligned */
        (int32_t)tfm_core_test_mpu_access & (~(0x3)),
        (int32_t)args};

    err = tfm_core_test_svc(tfm_core_test_sfn, args);

    if (err != TFM_SUCCESS && err < TFM_PARTITION_SPECIFIC_ERROR_MIN) {
        TEST_FAIL("TFM Core returned error.");
        return;
    }

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        char *info = error_to_string(
            "Service memory accesses configured incorrectly.", err);
        TEST_FAIL(info);
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_buffer_check(struct test_result_t *ret)
{
    int32_t res, i;

    uint32_t inbuf[] = {1, 2, 3, 4, 0xAAAFFF, 0xFFFFFFFF};
    uint32_t outbuf[16] = {0};
    int32_t result;
    int32_t args[] = {(int32_t)&result, (int32_t)inbuf,
                  (int32_t)outbuf, (int32_t)sizeof(inbuf) >> 2};

    res = tfm_core_test_svc(tfm_core_test_2_sfn_invert, args);
    if ((res != TFM_SUCCESS) && (res < TFM_PARTITION_SPECIFIC_ERROR_MIN)) {
        TEST_FAIL("Call to secure service should be successful.");
        return;
    }
    if ((result == 0) && (res == TFM_SUCCESS)) {
        for (i = 0; i < sizeof(inbuf) >> 2; i++) {
            if (outbuf[i] != ~inbuf[i]) {
                TEST_FAIL("Secure function failed to modify buffer.");
                return;
            }
        }
        for (; i < sizeof(outbuf) >> 2; i++) {
            if (outbuf[i] != 0) {
                TEST_FAIL("Secure function buffer access overflow.");
                return;
            }
        }
    } else {
        TEST_FAIL("Secure service returned error.");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_ss_to_ss(struct test_result_t *ret)
{
    int32_t err;

    args[0] = CORE_TEST_ID_SS_TO_SS;
    err = tfm_core_test_svc(tfm_core_test_sfn, args);

    if (err != TFM_SUCCESS && err < TFM_PARTITION_SPECIFIC_ERROR_MIN) {
        TEST_FAIL("Call to secure service should be successful.");
        return;
    }

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("The internal service call failed.");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_share_change(struct test_result_t *ret)
{
    int32_t err;

    args[0] = CORE_TEST_ID_SHARE_REDIRECTION;
    err = tfm_core_test_svc(tfm_core_test_sfn, args);

    if ((err != TFM_SUCCESS) && (err < TFM_PARTITION_SPECIFIC_ERROR_MIN)) {
        TEST_FAIL("TFM Core returned error.");
        return;
    }

    if (err != CORE_TEST_ERRNO_SUCCESS) {
        TEST_FAIL("Failed to redirect share region in service.");
        return;
    }

    ret->val = TEST_PASSED;
}

static void tfm_core_test_ss_to_ss_buffer(struct test_result_t *ret)
{
    int32_t res, i;

    uint32_t inbuf[] = {1, 2, 3, 4, 0xAAAFFF, 0xFFFFFFFF};
    uint32_t outbuf[16] = {0};
    int32_t args[] = {CORE_TEST_ID_SS_TO_SS_BUFFER, (int32_t)inbuf,
                  (int32_t)outbuf, (int32_t)sizeof(inbuf) >> 2};

    res = tfm_core_test_svc(tfm_core_test_sfn, args);
    if ((res != TFM_SUCCESS) && (res < TFM_PARTITION_SPECIFIC_ERROR_MIN)) {
        TEST_FAIL("Call to secure service should be successful.");
        return;
    }
    switch (res) {
    case TFM_SUCCESS:
        for (i = 0; i < sizeof(inbuf) >> 2; i++) {
            if (outbuf[i] != ~inbuf[i]) {
                TEST_FAIL("Secure function failed to modify buffer.");
                return;
            }
        }
        for (; i < sizeof(outbuf) >> 2; i++) {
            if (outbuf[i] != 0) {
                TEST_FAIL("Secure function buffer access overflow.");
                return;
            }
        }
        ret->val = TEST_PASSED;
        return;
    case CORE_TEST_ERRNO_INVALID_BUFFER:
        TEST_FAIL("NS buffer rejected by TF-M core.");
        return;
    case CORE_TEST_ERRNO_SLAVE_SP_CALL_FAILURE:
        TEST_FAIL("Slave service call failed.");
        return;
    case CORE_TEST_ERRNO_SLAVE_SP_BUFFER_FAILURE:
        TEST_FAIL("Slave secure function failed to modify buffer.");
        return;
    default:
        TEST_FAIL("Secure service returned error.");
        return;
    }
}

static void tfm_core_test_two_sfn_one_svc(struct test_result_t *ret)
{
    int32_t err;

    args[0] = CORE_TEST_ID_TWO_SFN_ONE_SVC;
    err = tfm_core_test_multiple_calls_svc(tfm_core_test_sfn, args);

    if ((err != TFM_SUCCESS) && (err < TFM_PARTITION_SPECIFIC_ERROR_MIN)) {
        TEST_FAIL("TFM Core returned error.");
        return;
    }

    switch (err) {
    case TFM_SUCCESS:
        ret->val = TEST_PASSED;
        return;
    case CORE_TEST_ERRNO_FIRST_CALL_FAILED:
        TEST_FAIL("First call failed.");
        return;
    case CORE_TEST_ERRNO_SECOND_CALL_FAILED:
        TEST_FAIL("Second call failed.");
        return;
    default:
        TEST_FAIL("Secure service returned unknown error.");
        return;
    }
}
