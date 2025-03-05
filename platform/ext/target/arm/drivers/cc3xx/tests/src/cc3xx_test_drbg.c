/*
 * Copyright (c) 2025, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_test_drbg.h"
#ifndef CC3XX_CONFIG_FILE
#include "cc3xx_config.h"
#else
#include CC3XX_CONFIG_FILE
#endif
#include "cc3xx_test_assert.h"

#include "cc3xx_test_utils.h"

/* Test data are inspired from DRBGVS test framework */

#define DRBG_CTR_RANDOM_BITS   512

void drbg_ctr_test(struct test_result_t *ret)
{
    cc3xx_err_t err;
    struct cc3xx_drbg_ctr_state_t state;

    uint8_t readback[DRBG_CTR_RANDOM_BITS / 8];
    uint8_t entropy[CC3XX_DRBG_CTR_SEEDLEN] = {
        0xed, 0x1e, 0x7f, 0x21, 0xef, 0x66, 0xea, 0x5d, 0x8e, 0x2a,
        0x85, 0xb9, 0x33, 0x72, 0x45, 0x44, 0x5b, 0x71, 0xd6, 0x39,
        0x3a, 0x4e, 0xec, 0xb0, 0xe6, 0x3c, 0x19, 0x3d, 0x0f, 0x72,
        0xf9, 0xa9};
    uint8_t entropy_reseed[CC3XX_DRBG_CTR_SEEDLEN] = {
        0x30, 0x3f, 0xb5, 0x19, 0xf0, 0xa4, 0xe1, 0x7d, 0x6d, 0xf0,
        0xb6, 0x42, 0x6a, 0xa0, 0xec, 0xb2, 0xa3, 0x60, 0x79, 0xbd,
        0x48, 0xbe, 0x47, 0xad, 0x2a, 0x8d, 0xbf, 0xe4, 0x8d, 0xa3,
        0xef, 0xad};
    uint8_t expected[DRBG_CTR_RANDOM_BITS / 8] = {
        0xf8, 0x01, 0x11, 0xd0, 0x8e, 0x87, 0x46, 0x72, 0xf3, 0x2f,
        0x42, 0x99, 0x71, 0x33, 0xa5, 0x21, 0x0f, 0x7a, 0x93, 0x75,
        0xe2, 0x2c, 0xea, 0x70, 0x58, 0x7f, 0x9c, 0xfa, 0xfe, 0xbe,
        0x0f, 0x6a, 0x6a, 0xa2, 0xeb, 0x68, 0xe7, 0xdd, 0x91, 0x64,
        0x53, 0x6d, 0x53, 0xfa, 0x02, 0x0f, 0xca, 0xb2, 0x0f, 0x54,
        0xca, 0xdd, 0xfa, 0xb7, 0xd6, 0xd9, 0x1e, 0x5f, 0xfe, 0xc1,
        0xdf, 0xd8, 0xde, 0xaa};

    err = cc3xx_lowlevel_drbg_ctr_init(&state,
                    (const uint8_t *)entropy, sizeof(entropy), NULL, 0, NULL, 0);
    TEST_ASSERT(err == CC3XX_ERR_SUCCESS, "drbg ctr init failed");

    err = cc3xx_lowlevel_drbg_ctr_reseed(&state,
                    (const uint8_t *)entropy_reseed, sizeof(entropy), NULL, 0);
    TEST_ASSERT(err == CC3XX_ERR_SUCCESS, "drbg ctr reseed failed");

    /* The DRBG requires the number of bits to generate, aligned to byte-sizes */
    err = cc3xx_lowlevel_drbg_ctr_generate(&state,
                    DRBG_CTR_RANDOM_BITS, readback, NULL, 0);
    TEST_ASSERT(err == CC3XX_ERR_SUCCESS, "drbg ctr generate failed");

    /* The DRBG requires the number of bits to generate, aligned to byte-sizes */
    err = cc3xx_lowlevel_drbg_ctr_generate(&state,
                    DRBG_CTR_RANDOM_BITS, readback, NULL, 0);
    TEST_ASSERT(err == CC3XX_ERR_SUCCESS, "drbg ctr generate failed");

    TEST_ASSERT(memcmp(readback, expected, DRBG_CTR_RANDOM_BITS / 8) == 0,
                    "random bits mismatch");

    ret->val = TEST_PASSED;
cleanup:
    cc3xx_lowlevel_drbg_ctr_uninit(&state);

    return;
}

static struct test_t drbg_ctr_tests[] = {
    /* df = derivative function, pr = prediction resistance */
    {
        &drbg_ctr_test,
        "CC3XX_DRBG_CTR_TEST",
        "CC3XX DRBG CTR test df=False pr=False",
    },
};

void add_cc3xx_drbg_tests_to_testsuite(struct test_suite_t *p_ts, uint32_t ts_size)
{
#ifdef CC3XX_CONFIG_DRBG_CTR_ENABLE
    cc3xx_add_tests_to_testsuite(drbg_ctr_tests, ARRAY_SIZE(drbg_ctr_tests), p_ts, ts_size);
#endif
}
