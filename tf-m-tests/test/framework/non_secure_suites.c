/*
 * Copyright (c) 2017-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test_framework_integ_test.h"
#include "test_framework_integ_test_helper.h"
#include "test_framework.h"

/* Service specific includes */
#if defined(TFM_PARTITION_PROTECTED_STORAGE) || defined(FORWARD_PROT_MSG)
#include "ps_ns_tests.h"
#endif
#if defined(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE) || defined(FORWARD_PROT_MSG)
#include "its_ns_tests.h"
#endif
#if defined(TFM_PARTITION_CRYPTO) || defined(FORWARD_PROT_MSG)
#include "crypto_ns_tests.h"
#endif
#if defined(TFM_PARTITION_FIRMWARE_UPDATE)
#include "fwu_ns_tests.h"
#endif
#if defined(TFM_PARTITION_INITIAL_ATTESTATION) || defined(FORWARD_PROT_MSG)
#include "attest_ns_tests.h"
#include "qcbor_ns_tests.h"
#ifndef SYMMETRIC_INITIAL_ATTESTATION
#include "t_cose_ns_tests.h"
#endif /* !SYMMETRIC_INITIAL_ATTESTATION */
#endif
#if defined(TFM_PARTITION_PLATFORM) || defined(FORWARD_PROT_MSG)
#include "platform_ns_tests.h"
#endif
#include "core_ns_tests.h"
#ifdef TFM_PSA_API
#include "ipc_ns_tests.h"
#else
#ifdef TFM_PARTITION_AUDIT_LOG
#include "audit_ns_tests.h"
#endif
#endif /* TFM_PSA_API */
#ifdef TFM_MULTI_CORE_TOPOLOGY
#include "multi_core_ns_test.h"
#endif /* TFM_MULTI_CORE_TOPOLOGY */
#ifdef TFM_FUZZER_TOOL_TESTS
#include "tf_fuzz_testsuite.h"
#endif /* TFM_FUZZER_TOOL_TESTS */
#if defined(TFM_ENABLE_SLIH_TEST) || defined(TFM_ENABLE_FLIH_TEST)
#include "irq_testsuite.h"
#endif

static struct test_suite_t test_suites[] = {
    /* List test cases which are compliant with level 1 isolation */

#if defined(TFM_PARTITION_PROTECTED_STORAGE) || defined(FORWARD_PROT_MSG)
    {&register_testsuite_ns_psa_ps_interface, 0, 0, 0},
#endif

#if defined(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE) || defined(FORWARD_PROT_MSG)
    /* Non-secure ITS test cases */
    {&register_testsuite_ns_psa_its_interface, 0, 0, 0},
#endif

#if defined(TFM_PARTITION_CRYPTO) || defined(FORWARD_PROT_MSG)
    /* Non-secure Crypto test cases */
    {&register_testsuite_ns_crypto_interface, 0, 0, 0},
#endif

#if defined(TFM_PARTITION_PLATFORM) || defined(FORWARD_PROT_MSG)
    /* Non-secure platform service test cases */
    {&register_testsuite_ns_platform_interface, 0, 0, 0},
#endif

#if defined(TFM_PARTITION_INITIAL_ATTESTATION) || defined(FORWARD_PROT_MSG)
    /* Non-secure initial attestation service test cases */
    {&register_testsuite_ns_attestation_interface, 0, 0, 0},

    /* Non-secure QCBOR library test cases */
    {&register_testsuite_ns_qcbor, 0, 0, 0},

#ifndef SYMMETRIC_INITIAL_ATTESTATION
    /* Non-secure T_COSE library test cases */
    {&register_testsuite_ns_t_cose, 0, 0, 0},
#endif /* !SYMMETRIC_INITIAL_ATTESTATION */
#endif

#ifdef TFM_PARTITION_AUDIT_LOG
    /* Non-secure Audit Logging test cases */
    {&register_testsuite_ns_audit_interface, 0, 0, 0},
#endif

#ifdef TFM_PARTITION_FIRMWARE_UPDATE
    /* Non-secure Firmware Update test cases */
    {&register_testsuite_ns_psa_fwu_interface, 0, 0, 0},
#endif

/* Non-secure core test cases */
{&register_testsuite_ns_core_positive, 0, 0, 0},

#ifdef CORE_TEST_INTERACTIVE
    /* Non-secure interactive test cases */
    {&register_testsuite_ns_core_interactive, 0, 0, 0},
#endif

#ifdef TFM_PSA_API
    /* Non-secure IPC test cases */
    {&register_testsuite_ns_ipc_interface, 0, 0, 0},
#endif

#ifdef TFM_MULTI_CORE_TOPOLOGY
    /* Multi-core topology test cases */
    {&register_testsuite_multi_core_ns_interface, 0, 0, 0},
#endif

#ifdef TFM_FUZZER_TOOL_TESTS
    {&register_testsuite_tf_fuzz_test, 0, 0, 0},
#endif /* TFM_FUZZER_TOOL_TESTS */

#if defined(TFM_ENABLE_SLIH_TEST) || defined(TFM_ENABLE_FLIH_TEST)
    {&register_testsuite_irq_test, 0, 0, 0},
#endif

    /* End of test suites */
    {0, 0, 0, 0}
};

enum test_suite_err_t start_integ_test(void)
{
    return integ_test("Non-secure", test_suites);
}

/* Service stand-in for NS tests. To be called from a non-secure context */
enum test_suite_err_t tfm_non_secure_client_run_tests(void)
{
    return start_integ_test();
}
