/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attestation_s_tests.h"
#include "psa_initial_attestation_api.h"
#include "secure_fw/services/initial_attestation/attestation.h"
#include "../attestation_tests_common.h"
#include "secure_utilities.h"
#include "platform/include/tfm_plat_device_id.h"
#include "platform/include/tfm_plat_boot_seed.h"
#include <string.h>
#include <stdlib.h>

static uint8_t token_buffer[TEST_TOKEN_SIZE];
static const uint8_t challenge_buffer[TEST_CHALLENGE_OBJ_SIZE] =
                                     {CHALLENGE_FOR_TEST};

/* Define test suite for attestation service tests */
/* List of tests */
static void tfm_attest_test_1001(struct test_result_t *ret);
static void tfm_attest_test_1002(struct test_result_t *ret);

static struct test_t attestation_interface_tests[] = {
    {&tfm_attest_test_1001, "TFM_ATTEST_TEST_1001",
     "Get attestation token and check claims", {0} },
    {&tfm_attest_test_1002, "TFM_ATTEST_TEST_1002",
     "Negative test cases for initial attestation service", {0} },
};

void
register_testsuite_s_attestation_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(attestation_interface_tests) /
                 sizeof(attestation_interface_tests[0]));

    set_testsuite("Initial Attestation Service secure interface tests"
                  "(TFM_ATTEST_TEST_1XXX)",
                  attestation_interface_tests, list_size, p_test_suite);
}

/*!
 * \brief Positive tests for initial attestation service
 *
 *      - Calling Initial Attestation service to get token
 */
static void tfm_attest_test_1001(struct test_result_t *ret)
{
    enum psa_attest_err_t err;
    uint32_t token_size;

    /* Get attestation token size */
    err = psa_initial_attest_get_token_size(TEST_CHALLENGE_OBJ_SIZE,
                                            &token_size);
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("Get token size failed");
        return;
    }

    if (token_size > TEST_TOKEN_SIZE) {
        TEST_FAIL("Allocated buffer is smaller than required");
        return;
    }

    /* Get attestation token
     * FixMe: Hard coded challenge is used, because currently there is no
     *        support for random source(RNG, rand(), etc.) on secure side.
     */
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TEST_CHALLENGE_OBJ_SIZE,
                                       token_buffer,
                                       &token_size);
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("Get token failed");
        return;
    }

    ret->val = TEST_PASSED;
}

/*!
 * \brief Negative tests for initial attestation service
 *
 *    - Calling initial attestation service with bigger challenge object than
 *      allowed.
 *    - Calling initial attestation service with smaller buffer size than the
 *      expected size of the token.
 */
static void tfm_attest_test_1002(struct test_result_t *ret)
{
    enum psa_attest_err_t err;
    uint32_t token_size = TEST_TOKEN_SIZE;

    /* Call with with bigger challenge object than allowed */
    err = psa_initial_attest_get_token(challenge_buffer,
                                       INVALID_CHALLENGE_OBJECT_SIZE,
                                       token_buffer,
                                       &token_size);

    if (err != PSA_ATTEST_ERR_INVALID_INPUT) {
        TEST_FAIL("Attestation should fail with too big challenge object");
        return;
    }

    /* Call with smaller buffer size than size of test token */
    token_size = TOO_SMALL_TOKEN_BUFFER;
    err = psa_initial_attest_get_token(challenge_buffer,
                                       TEST_CHALLENGE_OBJ_SIZE,
                                       token_buffer,
                                       &token_size);

    if (err != PSA_ATTEST_ERR_TOKEN_BUFFER_OVERFLOW) {
        TEST_FAIL("Attestation should fail with too small token buffer");
        return;
    }

    ret->val = TEST_PASSED;
}
