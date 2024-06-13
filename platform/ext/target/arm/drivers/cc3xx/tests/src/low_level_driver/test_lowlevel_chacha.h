/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TEST_LOWLEVEL_CHACHA_H
#define TEST_LOWLEVEL_CHACHA_H

#include "cc3xx_test_chacha.h"

#ifdef __cplusplus
extern "C" {
#endif

int chacha_test_lowlevel_encrypt_decrypt(struct chacha_test_data_t *data);

int chacha_test_lowlevel_oneshot_decrypt(struct chacha_test_data_t *data);

int chacha_test_lowlevel_oneshot_encrypt(struct chacha_test_data_t *data);

int chacha_test_lowlevel_verify_negative(struct chacha_test_data_t *data);

int chacha_test_lowlevel_multipart_decrypt(struct chacha_test_data_t *data,
                                           size_t chunk_size);

int chacha_test_lowlevel_multipart_encrypt(struct chacha_test_data_t *data,
                                           size_t chunk_size);

int chacha_test_lowlevel_multipart_saveload_decrypt(struct chacha_test_data_t *data,
                                                    size_t chunk_size);

int chacha_test_lowlevel_multipart_saveload_encrypt(struct chacha_test_data_t *data,
                                                    size_t chunk_size);

int chacha_test_lowlevel_oneshot_inplace_decrypt(struct chacha_test_data_t *data);

int chacha_test_lowlevel_oneshot_inplace_encrypt(struct chacha_test_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* TEST_LOWLEVEL_CHACHA_H */
