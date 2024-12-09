/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto.h"

#include <string.h>
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/aes.h"
#include "mbedtls/hkdf.h"
#include "mbedtls/md.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "otp.h"

static int mbedtls_is_initialised = 0;
static uint8_t mbedtls_memory_buf[512];

static enum tfm_bl1_hash_alg_t multipart_alg = 0;
static mbedtls_sha256_context sha256_multipart_ctx;
static mbedtls_sha512_context sha384_multipart_ctx;

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
    size_t key_size;
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

    rc = bl1_otp_read_key(input_key, key_buf, sizeof(key_buf), &key_size);
    if (rc) {
        return rc;
    }


    sha256_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);

    rc = mbedtls_hkdf(sha256_info, NULL, 0, key_buf,
                      key_size, state, state_len,
                      output_key, output_length);

    return rc;
}

fih_int sha256_init()
{
    fih_int fih_rc;
    int rc;

    multipart_alg = TFM_BL1_HASH_ALG_SHA256;

    mbedtls_sha256_init(&sha256_multipart_ctx);

    rc = mbedtls_sha256_starts(&sha256_multipart_ctx, 0);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    FIH_RET(FIH_SUCCESS);

out:
    mbedtls_sha256_free(&sha256_multipart_ctx);
    multipart_alg = 0;
    FIH_RET(fih_rc);

}

fih_int sha256_finish(uint8_t *hash,
                        size_t hash_length,
                        size_t *hash_size)
{
    fih_int fih_rc;
    int rc;

    if (hash_length < 48) {
        fih_rc = FIH_FAILURE;
        goto out;
    }

    rc = mbedtls_sha256_finish(&sha256_multipart_ctx, hash);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    if (hash_size != NULL) {
        *hash_size = 48;
    }

out:
    mbedtls_sha256_free(&sha256_multipart_ctx);
    multipart_alg = 0;
    FIH_RET(fih_rc);
}

fih_int sha256_update(const uint8_t *data,
                        size_t data_length)
{
    int rc;
    fih_int fih_rc;

    rc = mbedtls_sha256_update(&sha256_multipart_ctx, data, data_length);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    FIH_RET(FIH_SUCCESS);

out:
    mbedtls_sha256_free(&sha256_multipart_ctx);
    multipart_alg = 0;
    FIH_RET(fih_rc);
}


fih_int sha384_init()
{
    fih_int fih_rc;
    int rc;

    multipart_alg = TFM_BL1_HASH_ALG_SHA384;

    mbedtls_sha512_init(&sha384_multipart_ctx);

    rc = mbedtls_sha512_starts(&sha384_multipart_ctx, 1);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    FIH_RET(FIH_SUCCESS);

out:
    mbedtls_sha512_free(&sha384_multipart_ctx);
    multipart_alg = 0;
    FIH_RET(fih_rc);

}

fih_int sha384_finish(uint8_t *hash,
                        size_t hash_length,
                        size_t *hash_size)
{
    fih_int fih_rc;
    int rc;

    if (hash_length < 48) {
        fih_rc = FIH_FAILURE;
        goto out;
    }

    rc = mbedtls_sha512_finish(&sha384_multipart_ctx, hash);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    if (hash_size != NULL) {
        *hash_size = 48;
    }

out:
    mbedtls_sha512_free(&sha384_multipart_ctx);
    multipart_alg = 0;
    FIH_RET(fih_rc);
}

fih_int sha384_update(const uint8_t *data,
                        size_t data_length)
{
    int rc;
    fih_int fih_rc;

    rc = mbedtls_sha512_update(&sha384_multipart_ctx, data, data_length);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    FIH_RET(FIH_SUCCESS);

out:
    mbedtls_sha512_free(&sha384_multipart_ctx);
    multipart_alg = 0;
    FIH_RET(fih_rc);
}


static fih_int sha256_compute(const uint8_t *data,
                           size_t data_length,
                           uint8_t *hash, size_t hash_len, size_t *hash_size)
{
    int rc = 0;
    fih_int fih_rc;
    mbedtls_sha256_context ctx;

    if (hash_len < 32) {
        FIH_RET(FIH_FAILURE);
    }

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

    if (hash_size != NULL) {
        *hash_size = 32;
    }

out:
    mbedtls_sha256_free(&ctx);
    FIH_RET(fih_rc);
}

static fih_int sha384_compute(const uint8_t *data,
                           size_t data_length,
                           uint8_t *hash, size_t hash_len, size_t *hash_size)
{
    int rc = 0;
    fih_int fih_rc;
    mbedtls_sha512_context ctx;

    if (hash_len < 48) {
        FIH_RET(FIH_FAILURE);
    }

    if (!mbedtls_is_initialised) {
        mbedtls_init(mbedtls_memory_buf, sizeof(mbedtls_memory_buf));
        mbedtls_is_initialised = 1;
    }

    mbedtls_sha512_init(&ctx);

    rc = mbedtls_sha512_starts(&ctx, 1);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    rc = mbedtls_sha512_update(&ctx, data, data_length);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    rc = mbedtls_sha512_finish(&ctx, hash);
    fih_rc = fih_int_encode_zero_equality(rc);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        goto out;
    }

    if (hash_size != NULL) {
        *hash_size = 48;
    }


out:
    mbedtls_sha512_free(&ctx);
    FIH_RET(fih_rc);
}

fih_int bl1_hash_init(enum tfm_bl1_hash_alg_t alg)
{
    switch(alg) {
    case TFM_BL1_HASH_ALG_SHA256:
        FIH_RET(sha256_init());
    case TFM_BL1_HASH_ALG_SHA384:
        FIH_RET(sha384_init());
    default:
        FIH_RET(FIH_FAILURE);
    }
}

fih_int bl1_hash_finish(uint8_t *hash,
                        size_t hash_length,
                        size_t *hash_size)
{
    switch(multipart_alg) {
    case TFM_BL1_HASH_ALG_SHA256:
        FIH_RET(sha256_finish(hash, hash_length, hash_size));
    case TFM_BL1_HASH_ALG_SHA384:
        FIH_RET(sha384_finish(hash, hash_length, hash_size));
    default:
        FIH_RET(FIH_FAILURE);
    }
}

fih_int bl1_hash_update(const uint8_t *data,
                        size_t data_length)
{
    switch(multipart_alg) {
    case TFM_BL1_HASH_ALG_SHA256:
        FIH_RET(sha256_update(data, data_length));
    case TFM_BL1_HASH_ALG_SHA384:
        FIH_RET(sha384_update(data, data_length));
    default:
        FIH_RET(FIH_FAILURE);
    }
}


fih_int bl1_hash_compute(enum tfm_bl1_hash_alg_t alg,
                         const uint8_t *data,
                         size_t data_length,
                         uint8_t *hash,
                         size_t hash_length,
                         size_t *hash_size)
{
    switch(alg) {
    case TFM_BL1_HASH_ALG_SHA256:
        FIH_RET(sha256_compute(data, data_length, hash, hash_length, hash_size));
    case TFM_BL1_HASH_ALG_SHA384:
        FIH_RET(sha384_compute(data, data_length, hash, hash_length, hash_size));
    default:
        FIH_RET(FIH_FAILURE);
    }
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
    size_t key_size;
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
        rc = bl1_otp_read_key(key_id, key_buf, sizeof(key_buf), &key_size);
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

    rc = mbedtls_aes_setkey_enc(&ctx, input_key, key_size * 8);
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
