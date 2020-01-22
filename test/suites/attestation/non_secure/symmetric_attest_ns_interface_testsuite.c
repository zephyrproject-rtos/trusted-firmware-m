/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "attestation_ns_tests.h"
#include "psa/initial_attestation.h"
#include "secure_fw/partitions/initial_attestation/attestation.h"
#include "../attestation_tests_common.h"
#include "../attest_token_test_values.h"
#include "../attest_token_test.h"

/* Define test suite for attestation service tests */
/* List of tests */
static void tfm_attest_test_2001(struct test_result_t *ret);

static struct test_t attestation_interface_tests[] = {
    {&tfm_attest_test_2001, "TFM_ATTEST_TEST_2001",
     "Symmetric key algorithm based Initial Attestation test", {0} },
};

void
register_testsuite_ns_attestation_interface(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(attestation_interface_tests) /
                 sizeof(attestation_interface_tests[0]));

    set_testsuite("Symmetric key algorithm based Initial Attestation Service "
                  "non-secure interface tests (TFM_ATTEST_TEST_2XXX)",
                  attestation_interface_tests, list_size, p_test_suite);
}

/*!
 * \brief Get an IAT with symmetric key algorithm based Initial Attestation.
 */
static void tfm_attest_test_2001(struct test_result_t *ret)
{
    int32_t err;

    err = decode_test_symmetric_initial_attest();
    if (err != 0) {
        TEST_LOG("tfm_attest_test_2001() returned: %d\r\n", err);
        TEST_FAIL("Attest token tfm_attest_test_2001() has failed");
        return;
    }

    ret->val = TEST_PASSED;
}
