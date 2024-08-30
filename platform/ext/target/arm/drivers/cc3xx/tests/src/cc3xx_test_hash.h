/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_TEST_HASH_H
#define CC3XX_TEST_HASH_H

#include "cc3xx_hash.h"

#include "test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

struct hash_test_data_t {
    uint8_t name[100];
    struct {
        uint8_t sha256[SHA256_OUTPUT_SIZE];
        uint8_t sha224[SHA224_OUTPUT_SIZE];
        uint8_t sha1[SHA1_OUTPUT_SIZE];
    } output;
    size_t input_size;
    uint8_t input[];
};

void add_cc3xx_hash_tests_to_testsuite(struct test_suite_t *p_ts, uint32_t ts_size);

uint8_t *output_from_alg_and_data(cc3xx_hash_alg_t alg,
                                  struct hash_test_data_t *data);
size_t hash_size_from_alg(cc3xx_hash_alg_t alg);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_TEST_HASH_H */
