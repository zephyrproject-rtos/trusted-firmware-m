/*
 * Copyright (c) 2023, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "cc3xx_hash.h"
#include "cc3xx_hmac.h"
#include "cc3xx_stdlib.h"

cc3xx_err_t cc3xx_lowlevel_hmac_compute(
    size_t tag_len,
    const uint8_t *key,
    size_t key_size,
    cc3xx_hash_alg_t alg,
    const uint8_t *data,
    size_t data_length,
    uint32_t *tag,
    size_t tag_size,
    size_t *tag_length)
{
    const uint8_t ipad = 0x36;
    cc3xx_err_t err;
    size_t idx;
    /* In case the key is higher than B, it must be hashed first */
    uint32_t hash_key_output[CC3XX_HASH_LENGTH(alg) / sizeof(uint32_t)];
    const uint8_t *p_key = key;
    size_t key_length = key_size;
    const uint8_t ixopad = 0x6a; /* 0x36 ^ 0x5c == ipad ^ opad */
    uint8_t block[CC3XX_HMAC_BLOCK_SIZE];

    memset(block, ipad, sizeof(block));
    *tag_length = 0;

    assert(tag_size >= tag_len);

    err = cc3xx_lowlevel_hash_init(alg);
    if (err != CC3XX_ERR_SUCCESS) {
        return err;
    }

    if (key_size > CC3XX_HMAC_BLOCK_SIZE) {
        /* hash the key to L bytes */
        err = cc3xx_lowlevel_hash_update(key, key_size);
        if (err != CC3XX_ERR_SUCCESS) {
            goto out;
        }
        p_key = (const uint8_t *)hash_key_output;
        key_length = CC3XX_HASH_LENGTH(alg);
    }

    cc3xx_lowlevel_hash_finish(hash_key_output, sizeof(hash_key_output));

    /* K ^ ipad */
    for (idx = 0; idx < key_length; idx++) {
        block[idx] ^= p_key[idx];
    }

    /* H(K ^ ipad) */
    err = cc3xx_lowlevel_hash_init(alg);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    err = cc3xx_lowlevel_hash_update((uint8_t *)block, CC3XX_HMAC_BLOCK_SIZE);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    /* H(K ^ ipad | data)*/
    if (data_length > 0) {
        err = cc3xx_lowlevel_hash_update(data, data_length);
        if (err != CC3XX_ERR_SUCCESS) {
            goto out;
        }
    }

    /* Produce H(K ^ ipad | data) */
    cc3xx_lowlevel_hash_finish(hash_key_output, sizeof(hash_key_output));

    /* K ^ opad */
    for (idx = 0; idx < CC3XX_HMAC_BLOCK_SIZE; idx++) {
        block[idx] ^= ixopad;
    }

    /* H( K ^ opad | H(K ^ ipad | data)) */
    err = cc3xx_lowlevel_hash_init(alg);
    if (err != CC3XX_ERR_SUCCESS) {
        return err;
    }

    err = cc3xx_lowlevel_hash_update((uint8_t *)block, CC3XX_HMAC_BLOCK_SIZE);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    err = cc3xx_lowlevel_hash_update((uint8_t *)hash_key_output, sizeof(hash_key_output));
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    cc3xx_lowlevel_hash_finish(hash_key_output, sizeof(hash_key_output));

out:
    if (err == CC3XX_ERR_SUCCESS) {
        memcpy(tag, hash_key_output, tag_len);
        *tag_length = tag_len;
    }
    cc3xx_lowlevel_hash_uninit();
    cc3xx_secure_erase_buffer((uint32_t *)block, sizeof(block) / sizeof(uint32_t));
    return err;
}

cc3xx_err_t cc3xx_lowlevel_hmac_set_key(
    struct cc3xx_hmac_state_t *state,
    const uint8_t *key,
    size_t key_size,
    cc3xx_hash_alg_t alg)
{
    const uint8_t ipad = 0x36;
    cc3xx_err_t err;
    size_t idx;
    /* In case the key is higher than B, it must be hashed first */
    uint32_t hash_key_output[CC3XX_HMAC_BLOCK_SIZE / sizeof(uint32_t)];
    const uint8_t *p_key = key;
    size_t key_length = key_size;

    if (key_size > CC3XX_HMAC_BLOCK_SIZE) {
        err = cc3xx_lowlevel_hash_init(alg);
        if (err != CC3XX_ERR_SUCCESS) {
            return err;
        }

        /* hash the key to L bytes */
        err = cc3xx_lowlevel_hash_update(key, key_size);
        if (err != CC3XX_ERR_SUCCESS) {
            goto out;
        }
        p_key = (const uint8_t *)hash_key_output;
        key_length = CC3XX_HASH_LENGTH(alg);

        cc3xx_lowlevel_hash_finish(hash_key_output, sizeof(hash_key_output));
    }

    /* K ^ ipad */
    for (idx = 0; idx < key_length; idx++) {
        state->key[idx] = p_key[idx] ^ ipad;
    }

    memset(&state->key[key_length], ipad, CC3XX_HMAC_BLOCK_SIZE - key_length);

    /* H(K ^ ipad) */
    err = cc3xx_lowlevel_hash_init(alg);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    err = cc3xx_lowlevel_hash_update(state->key, CC3XX_HMAC_BLOCK_SIZE);

out:
    if (err == CC3XX_ERR_SUCCESS) {
        cc3xx_lowlevel_hash_get_state(&state->hash);
        state->alg = alg;
        state->tag_len = CC3XX_HASH_LENGTH(alg);
    }
    cc3xx_lowlevel_hash_uninit();
    return err;
}

void cc3xx_lowlevel_hmac_set_tag_length(
    struct cc3xx_hmac_state_t *state,
    size_t tag_len)
{
    state->tag_len = tag_len;
}

cc3xx_err_t cc3xx_lowlevel_hmac_update(
    struct cc3xx_hmac_state_t *state,
    const uint8_t *data,
    size_t data_length)
{
    cc3xx_err_t err;

    cc3xx_lowlevel_hash_set_state(&state->hash);

    /* H(K ^ ipad | data)*/
    err = cc3xx_lowlevel_hash_update(data, data_length);

    if (err == CC3XX_ERR_SUCCESS) {
        cc3xx_lowlevel_hash_get_state(&state->hash);
    }
    cc3xx_lowlevel_hash_uninit();
    return err;
}

cc3xx_err_t cc3xx_lowlevel_hmac_finish(
    struct cc3xx_hmac_state_t *state,
    uint32_t *tag,
    size_t tag_size,
    size_t *tag_len)
{
    uint32_t scratch[CC3XX_HASH_LENGTH(state->alg) / sizeof(uint32_t)];
    const uint8_t ixopad = 0x36 ^ 0x5c; /* ipad ^ opad */
    cc3xx_err_t err;
    size_t idx;

    assert(tag_size >= state->tag_len);

    cc3xx_lowlevel_hash_set_state(&state->hash);

    /* Produce H(K ^ ipad | data) */
    cc3xx_lowlevel_hash_finish(scratch, sizeof(scratch));

    /* K ^ opad */
    for (idx = 0; idx < CC3XX_HMAC_BLOCK_SIZE; idx++) {
        state->key[idx] ^= ixopad;
    }

    /* H( K ^ opad | H(K ^ ipad | data)) */
    err = cc3xx_lowlevel_hash_init(state->alg);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    err = cc3xx_lowlevel_hash_update(state->key, CC3XX_HMAC_BLOCK_SIZE);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    err = cc3xx_lowlevel_hash_update((const uint8_t *)scratch, sizeof(scratch));
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    cc3xx_lowlevel_hash_finish(scratch, sizeof(scratch));

    if (tag_len != NULL) {
        *tag_len = state->tag_len;
    }

out:
    if (err == CC3XX_ERR_SUCCESS) {
        memcpy(tag, scratch, state->tag_len);
        cc3xx_lowlevel_hash_get_state(&state->hash);
    }
    cc3xx_lowlevel_hash_uninit();
    return err;
}
