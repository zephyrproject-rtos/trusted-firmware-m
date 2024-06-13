/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_test_chacha.h"

#include "cc3xx_chacha.h"
#include "cc3xx_test_assert.h"

#include <string.h>

struct cc3xx_chacha_state_t state;

int chacha_test_lowlevel_encrypt_decrypt(struct chacha_test_data_t *data)
{
    uint8_t ciphertext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN] = {0};
    uint8_t plaintext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN] = {0};
    uint32_t tag[CC3XX_TEST_CHACHA_TAG_MAX_LEN / sizeof(uint32_t)] = {0};
    size_t ciphertext_size;
    cc3xx_err_t err;
    int rc;

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_ENCRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(ciphertext, sizeof(ciphertext));

    if (data->auth_data_len != 0) {
        cc3xx_lowlevel_chacha20_update_authed_data(data->auth_data, data->auth_data_len);
    }

    if (data->plaintext_len != 0) {
        cc3xx_lowlevel_chacha20_update(data->plaintext, data->plaintext_len);
    }

    err = cc3xx_lowlevel_chacha20_finish(tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_DECRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(plaintext, sizeof(plaintext));

    cc3xx_lowlevel_chacha20_update_authed_data(data->auth_data, data->auth_data_len);

    cc3xx_lowlevel_chacha20_update(ciphertext, data->ciphertext_len);

    err = cc3xx_lowlevel_chacha20_finish(tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(plaintext, data->plaintext,
                             data->plaintext_len) == 0);
    cc3xx_test_assert(memcmp(tag, data->tag,
                             data->tag_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

int chacha_test_lowlevel_oneshot_encrypt(struct chacha_test_data_t *data)
{
    uint32_t ciphertext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    uint32_t tag[CC3XX_TEST_CHACHA_TAG_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int rc;

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_ENCRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(ciphertext, sizeof(ciphertext));

    cc3xx_lowlevel_chacha20_update_authed_data(data->auth_data, data->auth_data_len);

    cc3xx_lowlevel_chacha20_update(data->plaintext, data->plaintext_len);

    err = cc3xx_lowlevel_chacha20_finish(tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(ciphertext, data->ciphertext,
                             data->ciphertext_len) == 0);

    cc3xx_test_assert(memcmp(tag, data->tag,
                             data->tag_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

int chacha_test_lowlevel_oneshot_decrypt(struct chacha_test_data_t *data)
{
    uint32_t plaintext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int rc;

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_DECRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(plaintext, sizeof(plaintext));

    cc3xx_lowlevel_chacha20_update_authed_data(data->auth_data, data->auth_data_len);
    cc3xx_lowlevel_chacha20_update(data->ciphertext, data->ciphertext_len);

    err = cc3xx_lowlevel_chacha20_finish(data->tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(plaintext, data->plaintext,
                             data->plaintext_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

int chacha_test_lowlevel_verify_negative(struct chacha_test_data_t *data)
{
    uint32_t plaintext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    uint32_t auth_data[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    uint32_t ciphertext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int expected_ciphertext_match = 0;
    int rc;

    memcpy(ciphertext, data->ciphertext, sizeof(data->ciphertext));
    memcpy(auth_data, data->auth_data, sizeof(data->auth_data));

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_DECRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    auth_data[0] ^= 0xFF;

    cc3xx_lowlevel_chacha20_set_output_buffer(plaintext, sizeof(plaintext));

    cc3xx_lowlevel_chacha20_update_authed_data(auth_data, data->auth_data_len);

    cc3xx_lowlevel_chacha20_update(data->ciphertext, data->ciphertext_len);

    err = cc3xx_lowlevel_chacha20_finish(data->tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_INVALID_TAG);

    cc3xx_test_assert(memcmp(plaintext, data->plaintext,
                             data->plaintext_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

static void chunked_submit(void (*submit_func)(const uint8_t *, size_t), uint8_t *data,
                    size_t length, size_t chunk_size) {
    size_t idx;

    for (idx = 0; idx + chunk_size < length; idx+=chunk_size) {
        submit_func(data + idx, chunk_size);
    }

    submit_func(data + idx, length - idx);
}

int chacha_test_lowlevel_multipart_encrypt(struct chacha_test_data_t *data,
                                           size_t chunk_size)
{
    uint32_t ciphertext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    uint32_t tag[CC3XX_TEST_CHACHA_TAG_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int rc;

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_ENCRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(ciphertext, sizeof(ciphertext));

    chunked_submit(cc3xx_lowlevel_chacha20_update_authed_data, data->auth_data,
                   data->auth_data_len, chunk_size);

    chunked_submit(cc3xx_lowlevel_chacha20_update, data->plaintext, data->plaintext_len,
                   chunk_size);

    err = cc3xx_lowlevel_chacha20_finish(tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(ciphertext, data->ciphertext,
                             data->ciphertext_len) == 0);

    cc3xx_test_assert(memcmp(tag, data->tag,
                             data->tag_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

int chacha_test_lowlevel_multipart_decrypt(struct chacha_test_data_t *data,
                                           size_t chunk_size)
{
    uint32_t plaintext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int rc;

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_DECRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(plaintext, sizeof(plaintext));

    chunked_submit(cc3xx_lowlevel_chacha20_update_authed_data, data->auth_data,
                   data->auth_data_len, chunk_size);

    chunked_submit(cc3xx_lowlevel_chacha20_update, data->ciphertext, data->ciphertext_len,
                   chunk_size);

    err = cc3xx_lowlevel_chacha20_finish(data->tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(plaintext, data->plaintext,
                             data->plaintext_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

static void chacha_saveload_update_aad(const uint8_t *data, size_t length) {
    cc3xx_lowlevel_chacha20_set_state(&state);
    cc3xx_lowlevel_chacha20_update_authed_data(data, length);
    cc3xx_lowlevel_chacha20_get_state(&state);
    cc3xx_lowlevel_chacha20_uninit();
}


static void chacha_saveload_update(const uint8_t *data, size_t length) {
    cc3xx_lowlevel_chacha20_set_state(&state);
    cc3xx_lowlevel_chacha20_update(data, length);
    cc3xx_lowlevel_chacha20_get_state(&state);
    cc3xx_lowlevel_chacha20_uninit();
}

int chacha_test_lowlevel_multipart_saveload_encrypt(struct chacha_test_data_t *data,
                                                 size_t chunk_size)
{
    uint32_t ciphertext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    uint32_t tag[CC3XX_TEST_CHACHA_TAG_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int rc;

    memset(&state, 0, sizeof(state));

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_ENCRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(ciphertext, sizeof(ciphertext));

    cc3xx_lowlevel_chacha20_get_state(&state);
    cc3xx_lowlevel_chacha20_uninit();

    chunked_submit(chacha_saveload_update_aad, data->auth_data,
                   data->auth_data_len, chunk_size);

    chunked_submit(chacha_saveload_update, data->plaintext, data->plaintext_len,
                   chunk_size);

    cc3xx_lowlevel_chacha20_set_state(&state);
    err = cc3xx_lowlevel_chacha20_finish(tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(ciphertext, data->ciphertext,
                             data->ciphertext_len) == 0);

    cc3xx_test_assert(memcmp(tag, data->tag,
                             data->tag_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

int chacha_test_lowlevel_multipart_saveload_decrypt(struct chacha_test_data_t *data,
                                                 size_t chunk_size)
{
    uint32_t plaintext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int rc;

    memset(&state, 0, sizeof(state));

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_DECRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(plaintext, sizeof(plaintext));

    cc3xx_lowlevel_chacha20_get_state(&state);
    cc3xx_lowlevel_chacha20_uninit();

    chunked_submit(chacha_saveload_update_aad, data->auth_data,
                   data->auth_data_len, chunk_size);

    chunked_submit(chacha_saveload_update, data->ciphertext, data->ciphertext_len,
                   chunk_size);

    cc3xx_lowlevel_chacha20_set_state(&state);
    err = cc3xx_lowlevel_chacha20_finish(data->tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(plaintext, data->plaintext,
                             data->plaintext_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

int chacha_test_lowlevel_oneshot_inplace_encrypt(struct chacha_test_data_t *data)
{
    uint32_t ciphertext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    uint32_t tag[CC3XX_TEST_CHACHA_TAG_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int rc;

    memcpy(ciphertext, data->plaintext, data->plaintext_len);

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_ENCRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(ciphertext, sizeof(ciphertext));

    cc3xx_lowlevel_chacha20_update_authed_data(data->auth_data, data->auth_data_len);

    cc3xx_lowlevel_chacha20_update(ciphertext, data->plaintext_len);

    err = cc3xx_lowlevel_chacha20_finish(tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(ciphertext, data->ciphertext,
                             data->ciphertext_len) == 0);

    cc3xx_test_assert(memcmp(tag, data->tag,
                             data->tag_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

int chacha_test_lowlevel_oneshot_inplace_decrypt(struct chacha_test_data_t *data)
{
    uint32_t plaintext[CC3XX_TEST_CHACHA_PLAINTEXT_MAX_LEN / sizeof(uint32_t)] = {0};
    cc3xx_err_t err;
    int rc;

    memcpy(plaintext, data->ciphertext, data->ciphertext_len);

    err = cc3xx_lowlevel_chacha20_init(CC3XX_CHACHA_DIRECTION_DECRYPT,
                        CC3XX_CHACHA_MODE_CHACHA_POLY1305,
                        (uint32_t *)data->key, 0, (uint32_t *)data->iv,
                        data->iv_len);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_lowlevel_chacha20_set_output_buffer(plaintext, sizeof(plaintext));

    cc3xx_lowlevel_chacha20_update_authed_data(data->auth_data, data->auth_data_len);

    cc3xx_lowlevel_chacha20_update(plaintext, data->ciphertext_len);

    err = cc3xx_lowlevel_chacha20_finish(data->tag, NULL);
    cc3xx_test_assert(err == CC3XX_ERR_SUCCESS);

    cc3xx_test_assert(memcmp(plaintext, data->plaintext,
                             data->plaintext_len) == 0);

    rc = 0;
cleanup:
    cc3xx_lowlevel_chacha20_uninit();

    return rc;
}

