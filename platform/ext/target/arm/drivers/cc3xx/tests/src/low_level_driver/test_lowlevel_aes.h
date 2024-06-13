/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TEST_LOWLEVEL_AES_H
#define TEST_LOWLEVEL_AES_H

#include "cc3xx_test_aes.h"

#ifdef __cplusplus
extern "C" {
#endif

int aes_test_lowlevel_encrypt_decrypt(struct aes_test_data_t *data,
                                      cc3xx_aes_mode_t mode,
                                      cc3xx_aes_keysize_t key_size);

int aes_test_lowlevel_oneshot_decrypt(struct aes_test_data_t *data,
                                      cc3xx_aes_mode_t mode,
                                      cc3xx_aes_keysize_t key_size);

int aes_test_lowlevel_oneshot_encrypt(struct aes_test_data_t *data,
                                      cc3xx_aes_mode_t mode,
                                      cc3xx_aes_keysize_t key_size);

int aes_test_lowlevel_verify_negative(struct aes_test_data_t *data,
                                      cc3xx_aes_mode_t mode,
                                      cc3xx_aes_keysize_t key_size);

int aes_test_lowlevel_multipart_decrypt(struct aes_test_data_t *data,
                                        cc3xx_aes_mode_t mode,
                                        cc3xx_aes_keysize_t key_size,
                                        size_t chunk_size);

int aes_test_lowlevel_multipart_encrypt(struct aes_test_data_t *data,
                                        cc3xx_aes_mode_t mode,
                                        cc3xx_aes_keysize_t key_size,
                                        size_t chunk_size);

int aes_test_lowlevel_multipart_saveload_decrypt(struct aes_test_data_t *data,
                                                 cc3xx_aes_mode_t mode,
                                                 cc3xx_aes_keysize_t key_size,
                                                 size_t chunk_size);

int aes_test_lowlevel_multipart_saveload_encrypt(struct aes_test_data_t *data,
                                                 cc3xx_aes_mode_t mode,
                                                 cc3xx_aes_keysize_t key_size,
                                                 size_t chunk_size);

int aes_test_lowlevel_oneshot_inplace_decrypt(struct aes_test_data_t *data,
                                              cc3xx_aes_mode_t mode,
                                              cc3xx_aes_keysize_t key_size);

int aes_test_lowlevel_oneshot_inplace_encrypt(struct aes_test_data_t *data,
                                              cc3xx_aes_mode_t mode,
                                              cc3xx_aes_keysize_t key_size);

#ifdef __cplusplus
}
#endif

#endif /* TEST_LOWLEVEL_AES_H */
