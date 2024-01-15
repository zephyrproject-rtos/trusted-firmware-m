/*
 * Copyright (c) 2021-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto.h"

#include <string.h>
#include "mbedtls/sha256.h"
#include "mbedtls/aes.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/md.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "otp.h"

static int mbedtls_is_initialised = 0;
static uint8_t mbedtls_memory_buf[512];

static void mbedtls_init(uint8_t mbedtls_memory_buf[], size_t size)
{
    mbedtls_memory_buffer_alloc_init(mbedtls_memory_buf,
                                     size);
}

int32_t bl1_derive_key(enum tfm_bl1_key_id_t input_key, const uint8_t *label,
                       size_t label_length, const uint8_t *context,
                       size_t context_length, uint8_t *output_key,
                       size_t output_length)
{
    int rc = 0;
    uint8_t state[64] = {0};
    uint8_t key_buf[32] = {0};
    uint32_t state_len = context_length + label_length;
    const mbedtls_md_info_t *sha256_info = NULL;

    if (state_len > sizeof(state)) {
        return -1;
    }

    memcpy(state, label, label_length);
    memcpy(&state[label_length], context, context_length);

    if (!mbedtls_is_initialised) {
        mbedtls_init(mbedtls_memory_buf, sizeof(mbedtls_memory_buf));
        mbedtls_is_initialised = 1;
    }

    rc = bl1_otp_read_key(input_key, key_buf);
    if (rc) {
        return rc;
    }


    sha256_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    rc = mbedtls_hkdf(sha256_info, NULL, 0, key_buf,
                      sizeof(key_buf), state, state_len,
                      output_key, output_length);

    return rc;
}

int32_t bl1_sha256_compute(const uint8_t *data,
                           size_t data_length,
                           uint8_t *hash)
{
    int rc = 0;
    fih_int fih_rc;
    mbedtls_sha256_context ctx;

    if (!mbedtls_is_initialised) {
        mbedtls_init(mbedtls_memory_buf, sizeof(mbedtls_memory_buf));
        mbedtls_is_initialised = 1;
    }

    mbedtls_sha256_init(&ctx);

    rc = mbedtls_sha256_starts(&ctx, 0);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    rc = mbedtls_sha256_update(&ctx, data, data_length);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    rc = mbedtls_sha256_finish(&ctx, hash);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

out:
    mbedtls_sha256_free(&ctx);
    FIH_RET(fih_rc);
}

int32_t bl1_aes_256_ctr_decrypt(enum tfm_bl1_key_id_t key_id,
                                const uint8_t *key_material,
                                uint8_t *counter,
                                const uint8_t *ciphertext,
                                size_t ciphertext_length,
                                uint8_t *plaintext)
{
    int rc = 0;
    uint8_t stream_block[16];
    uint8_t key_buf[32];
    mbedtls_aes_context ctx;
    size_t nc_off = 0;
    const uint8_t *input_key = key_buf;

    if (ciphertext_length == 0) {
        return 0;
    }

    if (ciphertext == NULL || plaintext == NULL || counter == NULL) {
        return -2;
    }

    if (key_material == NULL) {
        rc = bl1_otp_read_key(key_id, key_buf);
        if (rc) {
            return rc;
        }
    } else {
        input_key = key_material;
    }


    if (!mbedtls_is_initialised) {
        mbedtls_init(mbedtls_memory_buf, sizeof(mbedtls_memory_buf));
        mbedtls_is_initialised = 1;
    }

    mbedtls_aes_init(&ctx);

    rc = mbedtls_aes_setkey_enc(&ctx, input_key, 256);
    if (rc) {
        goto out;
    }

    rc = mbedtls_aes_crypt_ctr(&ctx, ciphertext_length, &nc_off, counter,
                               stream_block, ciphertext, plaintext);
    if (rc) {
        goto out;
    }

out:
    mbedtls_aes_free(&ctx);

    memset(key_buf, 0, 32);
    memset(stream_block, 0, 16);

    return rc;
}
