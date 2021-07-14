/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "psa_manifest/sid.h"
#include "psa/client.h"
#include "test_framework.h"
#include "tfm_peripherals_def.h"
#include "tfm_plat_test.h"
#ifdef TFM_ENABLE_SLIH_TEST
#include "tfm_slih_test_service_types.h"
#endif /* TFM_ENABLE_SLIH_TEST */
#ifdef TFM_ENABLE_FLIH_TEST
#include "tfm_flih_test_service_types.h"
#endif /* TFM_ENABLE_FLIH_TEST */

#ifdef TFM_ENABLE_SLIH_TEST
/*
 * Test process:
 *    - NSPE starts testing
 *    - Test Partition starts timer
 *    - Test Partition waits for the timer signal
 *    - Test Partition receives the signal and stop timer
 *    - Test Partition returns to NSPE
 *    - Test finishes
 */
static void tfm_irq_test_slih_case_1(struct test_result_t *ret) {
    psa_status_t status;

    status = psa_call(TFM_SLIH_TEST_CASE_HANDLE,
                      TFM_SLIH_TEST_CASE_1, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("TFM_NS_IRQ_TEST_SLIH_HANDLING FAILED\r\n");
        return;
    }

    ret->val = TEST_PASSED;
}
#endif /* TFM_ENABLE_SLIH_TEST */

#ifdef TFM_ENABLE_FLIH_TEST
/*
 * Test process:
 *    - NSPE starts testing
 *    - Test Partition starts timer
 *    - Test Partition waits for the timer to be trigger for a certain count by
 *      reading the global count in a while loop
 *    - In the handling function, the count is increased
 *    - The count reaches the value and test Partition stops timer
 *    - Test Partition returns to NSPE
 *    - Test finishes
 */
static void tfm_irq_test_flih_case_1(struct test_result_t *ret) {
    psa_status_t status;

    status = psa_call(TFM_FLIH_TEST_CASE_HANDLE,
                      TFM_FLIH_TEST_CASE_1, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("TFM_NS_IRQ_TEST_FLIH not returning signal, FAILED\r\n");
        return;
    }

    ret->val = TEST_PASSED;
}

/*
 * Test process:
 *    - NSPE starts testing
 *    - Test Partition starts timer
 *    - Test Partition waits for the timer signal
 *    - In the handling function, the timer trigger count is increased
 *    - The count reaches a certain value and the ISR returns PSA_FLIH_SIGNAL
 *    - Test Partition receives the signal, stops timer and returns to NSPE
 *    - Test finishes
 */
static void tfm_irq_test_flih_case_2(struct test_result_t *ret) {
    psa_status_t status;

    status = psa_call(TFM_FLIH_TEST_CASE_HANDLE,
                      TFM_FLIH_TEST_CASE_2, NULL, 0, NULL, 0);
    if (status != PSA_SUCCESS) {
        TEST_FAIL("TFM_NS_IRQ_TEST_FLIH returning signal FAILED\r\n");
        return;
    }

    ret->val = TEST_PASSED;
}
#endif /* TFM_ENABLE_FLIH_TEST */

static struct test_t irq_test_cases[] = {
#ifdef TFM_ENABLE_SLIH_TEST
    {&tfm_irq_test_slih_case_1, "TFM_NS_IRQ_TEST_SLIH_1001",
     "SLIH HANDLING Case 1", {TEST_PASSED}},
#endif /* TFM_ENABLE_SLIH_TEST */
#ifdef TFM_ENABLE_FLIH_TEST
    {&tfm_irq_test_flih_case_1, "TFM_NS_IRQ_TEST_FLIH_1101",
     "FLIH HANDLING not returning signal", {TEST_PASSED}},
    {&tfm_irq_test_flih_case_2, "TFM_NS_IRQ_TEST_FLIH_1102",
     "FLIH HANDLING returning Signal", {TEST_PASSED}},
#endif /* TFM_ENABLE_FLIH_TEST */
};

void register_testsuite_irq_test(struct test_suite_t *p_test_suite)
{
    uint32_t list_size;

    list_size = (sizeof(irq_test_cases) / sizeof(irq_test_cases[0]));

    set_testsuite("TFM IRQ Test (TFM_IRQ_TEST_1xxx)",
                  irq_test_cases, list_size, p_test_suite);
}
