/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attestation_ns_tests.h"
#include "psa_initial_attestation_api.h"
#include "secure_fw/services/initial_attestation/attestation.h"
#include "../attestation_tests_common.h"
#include "bl2/include/tfm_boot_status.h"
#include "tfm_memory_utils.h"
#include "platform/include/tfm_plat_device_id.h"
#include "platform/include/tfm_plat_boot_seed.h"
#include "tfm_api.h"
#include <string.h>
#include <stdlib.h>
#include "../token_test.h"

static uint8_t token_buffer[TEST_TOKEN_SIZE];
static uint8_t challenge_buffer[TEST_CHALLENGE_OBJ_SIZE];

/* Define test suite for attestation service tests */
/* List of tests */
static void tfm_attest_test_2001(struct test_result_t *ret);
static void tfm_attest_test_2002(struct test_result_t *ret);
static void tfm_attest_test_2003(struct test_result_t *ret);
static void tfm_attest_test_2004(struct test_result_t *ret);

static struct test_t attestation_interface_tests[] = {
    {&tfm_attest_test_2001, "TFM_ATTEST_TEST_2001",
     "Get attestation token and check claims", {0} },
    {&tfm_attest_test_2002, "TFM_ATTEST_TEST_2002",
     "Negative test cases for initial attestation service", {0} },
    {&tfm_attest_test_2003, "TFM_ATTEST_TEST_2003",
     "Minimal token test for CBOR/COSE encoding", {0} },
    {&tfm_attest_test_2004, "TFM_ATTEST_TEST_2004",
     "Minimal token size test for CBOR/COSE encoding", {0} },
};

/*!
 * \brief Static function to generate a random byte stream. It is used as a
 *        challenge object to test attestation service.
 *
 * \param[in]  bytes The size of the random bytes to generate
 * \param[out] buf   Pointer to the buffer to store random bytes
 *
 */
static void generate_challenge(uint32_t bytes, uint8_t *buf)
{
    uint32_t random_value;
    uint8_t  *p_buf = buf;

    while (bytes) {
        /* FixMe: use a valid source to generate random values */
        random_value = rand();

        if (bytes >= sizeof(random_value)) {
            tfm_memcpy(p_buf, &random_value, sizeof(random_value));
            bytes -= sizeof(random_value);
            p_buf += sizeof(random_value);
        } else {
            tfm_memcpy(p_buf, &random_value, bytes);
            bytes = 0;
        }
    }
}

void
register_testsuite_ns_attestation_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(attestation_interface_tests) /
                 sizeof(attestation_interface_tests[0]));

    set_testsuite("Initial Attestation Service secure interface tests"
                  "(TFM_ATTEST_TEST_2XXX)",
                  attestation_interface_tests, list_size, p_test_suite);
}

/*!
 * \brief Positive tests for initial attestation service
 *
 *      - Calling Initial Attestation service to get token
 */
static void tfm_attest_test_2001(struct test_result_t *ret)
{
    enum psa_attest_err_t err;
    uint32_t token_size;

    generate_challenge(TEST_CHALLENGE_OBJ_SIZE, challenge_buffer);

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

    /* Get attestation token */
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
static void tfm_attest_test_2002(struct test_result_t *ret)
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

/*!
 * \brief Get minimal token, only include a hard coded challenge, but omit the
 *        rest of the claims
 *
 * Calling the minimal_test, which just retrieves a specific token:
 *  - only hard coded challenge is included
 *  - token signature is the hash of the token concatenated twice
 */
static void tfm_attest_test_2003(struct test_result_t *ret)
{
    enum psa_attest_err_t err;

    err = minimal_test();
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("CBOR/COSE minimal_test has failed");
        return;
    }

    ret->val = TEST_PASSED;
}

/*!
 * \brief Get the size of the minimal token, only include a hard coded
 *        challenge, but omit the rest of the claims
 */
static void tfm_attest_test_2004(struct test_result_t *ret)
{
    enum psa_attest_err_t err;

    err = minimal_get_size_test();
    if (err != PSA_ATTEST_ERR_SUCCESS) {
        TEST_FAIL("CBOR/COSE minimal_get_size_test has failed");
        return;
    }

    ret->val = TEST_PASSED;
}
