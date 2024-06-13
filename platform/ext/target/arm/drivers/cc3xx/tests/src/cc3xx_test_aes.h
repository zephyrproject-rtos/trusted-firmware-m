/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_TEST_AES_H
#define CC3XX_TEST_AES_H

#include "cc3xx_aes.h"

#include "test_framework.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CC3XX_TEST_AES_PLAINTEXT_MAX_LEN 256
#define CC3XX_TEST_AES_IV_MAX_LEN 32
#define CC3XX_TEST_AES_TAG_MAX_LEN 16

struct aes_test_ciphertext_t {
    uint8_t ciphertext[CC3XX_TEST_AES_PLAINTEXT_MAX_LEN];
    size_t ciphertext_len;
    uint8_t tag[CC3XX_TEST_AES_TAG_MAX_LEN];
    size_t tag_len;
};

struct aes_test_mode_data_t {
    struct aes_test_ciphertext_t aes_128;
    struct aes_test_ciphertext_t aes_192;
    struct aes_test_ciphertext_t aes_256;
    uint8_t iv[CC3XX_TEST_AES_IV_MAX_LEN];
    size_t iv_len;
};

struct aes_test_data_t {
    uint8_t name[100];
    struct {
        uint8_t aes_128[16];
        uint8_t aes_192[24];
        uint8_t aes_256[32];
    } keys;
    struct {
        struct aes_test_mode_data_t ecb;
        struct aes_test_mode_data_t ctr;
        struct aes_test_mode_data_t cbc;
        struct aes_test_mode_data_t gcm;
        struct aes_test_mode_data_t cmac;
        struct aes_test_mode_data_t ccm;
    } modes;
    uint8_t plaintext[CC3XX_TEST_AES_PLAINTEXT_MAX_LEN];
    size_t plaintext_len;
    uint8_t auth_data[CC3XX_TEST_AES_PLAINTEXT_MAX_LEN];
    size_t auth_data_len;
};

void add_cc3xx_aes_tests_to_testsuite(struct test_suite_t *p_ts, uint32_t ts_size);

uint8_t *cc3xx_test_aes_get_key(cc3xx_aes_keysize_t key_size,
                                struct aes_test_data_t *data);
struct aes_test_mode_data_t *cc3xx_test_aes_get_mode_data(
        cc3xx_aes_mode_t mode, struct aes_test_data_t *data);
struct aes_test_ciphertext_t *cc3xx_test_aes_get_ciphertext(
        cc3xx_aes_keysize_t key_size, struct aes_test_mode_data_t *mode_data);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_TEST_AES_H */
