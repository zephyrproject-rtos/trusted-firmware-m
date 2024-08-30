/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_TEST_CHACHA_H
#define CC3XX_TEST_CHACHA_H

#include "cc3xx_chacha.h"

#include "test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN 384
#define CC3XX_TEST_CHACHA_IV_MAX_LEN 32
#define CC3XX_TEST_CHACHA_TAG_MAX_LEN 16

struct chacha_test_data_t {
    uint8_t name[100];
    uint8_t key[32];
    uint8_t iv[CC3XX_TEST_CHACHA_IV_MAX_LEN];
    size_t iv_len;
    uint8_t ciphertext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN];
    size_t ciphertext_len;
    uint8_t tag[CC3XX_TEST_CHACHA_TAG_MAX_LEN];
    size_t tag_len;
    uint8_t plaintext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN];
    size_t plaintext_len;
    uint8_t auth_data[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN];
    size_t auth_data_len;
};

void add_cc3xx_chacha_tests_to_testsuite(struct test_suite_t *p_ts, uint32_t ts_size);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_TEST_CHACHA_H */
