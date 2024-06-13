/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_tests.h"

#include "cc3xx_test_hash.h"
#include "cc3xx_test_aes.h"
#include "cc3xx_test_chacha.h"
#include "cc3xx_test_pka.h"
#include "cc3xx_test_ecc.h"
#include "cc3xx_test_ecdsa.h"

void add_cc3xx_tests_to_testsuite(struct test_suite_t *p_ts, uint32_t ts_size)
{
#ifdef TEST_CC3XX
    add_cc3xx_hash_tests_to_testsuite(p_ts, ts_size);
    add_cc3xx_aes_tests_to_testsuite(p_ts, ts_size);
    add_cc3xx_chacha_tests_to_testsuite(p_ts, ts_size);
    add_cc3xx_pka_tests_to_testsuite(p_ts, ts_size);
    add_cc3xx_ecc_tests_to_testsuite(p_ts, ts_size);
    add_cc3xx_ecdsa_tests_to_testsuite(p_ts, ts_size);
#endif /* TEST_CC3XX */
}
