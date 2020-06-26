/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 * Modifications are Copyright (c) 2018-2020 Arm Limited.
 */

#include <assert.h>
#include <stddef.h>
#include <inttypes.h>
#include <string.h>

#include "flash_map/flash_map.h"
#include "bootutil/image.h"
#include "bootutil/sha256.h"
#include "bootutil/sign_key.h"
#include "security_cnt.h"

#include "mcuboot_config/mcuboot_config.h"

#if defined(MCUBOOT_SIGN_RSA)
#include "mbedtls/rsa.h"
#endif

#include "mbedtls/asn1.h"

#include "bootutil_priv.h"

#ifdef MCUBOOT_HW_KEY
#include "platform/include/tfm_plat_crypto_keys.h"
#endif

/*
 * Compute SHA256 over the image.
 */
static int
bootutil_img_hash(int image_index,
                  struct image_header *hdr, const struct flash_area *fap,
                  uint8_t *tmp_buf, uint32_t tmp_buf_sz, uint8_t *hash_result,
                  uint8_t *seed, int seed_len)
{
    bootutil_sha256_context sha256_ctx;
    uint32_t size;
#ifndef MCUBOOT_RAM_LOADING
    uint32_t blk_sz;
    uint32_t off;
    int rc;
#endif /* MCUBOOT_RAM_LOADING */

    (void)image_index;

    bootutil_sha256_init(&sha256_ctx);

    /* in some cases (split image) the hash is seeded with data from
     * the loader image */
    if (seed && (seed_len > 0)) {
        bootutil_sha256_update(&sha256_ctx, seed, seed_len);
    }

    /* Hash is computed over image header and image itself. */
    size = BOOT_TLV_OFF(hdr);

    /* If protected TLVs are present they are also hashed. */
    size += hdr->ih_protect_tlv_size;

#ifdef MCUBOOT_RAM_LOADING
    bootutil_sha256_update(&sha256_ctx,(void*)(hdr->ih_load_addr), size);
#else
    for (off = 0; off < size; off += blk_sz) {
        blk_sz = size - off;
        if (blk_sz > tmp_buf_sz) {
            blk_sz = tmp_buf_sz;
        }
        rc = flash_area_read(fap, off, tmp_buf, blk_sz);
        if (rc) {
            return rc;
        }
        bootutil_sha256_update(&sha256_ctx, tmp_buf, blk_sz);
    }
#endif
    bootutil_sha256_finish(&sha256_ctx, hash_result);

    return 0;
}

/*
 * Currently, we only support being able to verify one type of
 * signature, because there is a single verification function that we
 * call.  List the type of TLV we are expecting.  If we aren't
 * configured for any signature, don't define this macro.
 */

#if defined(MCUBOOT_SIGN_RSA)
#    if MCUBOOT_SIGN_RSA_LEN == 2048
#        define EXPECTED_SIG_TLV IMAGE_TLV_RSA2048_PSS
#    elif MCUBOOT_SIGN_RSA_LEN == 3072
#        define EXPECTED_SIG_TLV IMAGE_TLV_RSA3072_PSS
#    else
#        error "Unsupported RSA signature length"
#    endif
#    define SIG_BUF_SIZE (MCUBOOT_SIGN_RSA_LEN / 8)
#    define EXPECTED_SIG_LEN(x) ((x) == SIG_BUF_SIZE)
#else
#    define SIG_BUF_SIZE 32 /* no signing, sha256 digest only */
#endif

#ifdef EXPECTED_SIG_TLV
#ifdef MCUBOOT_HW_KEY
extern unsigned int pub_key_len;
static int
bootutil_find_key(uint8_t image_id, uint8_t *key, uint16_t key_len)
{
    bootutil_sha256_context sha256_ctx;
    uint8_t hash[32];
    uint8_t key_hash[32];
    uint32_t key_hash_size= sizeof(key_hash);
    enum tfm_plat_err_t plat_err;

    bootutil_sha256_init(&sha256_ctx);
    bootutil_sha256_update(&sha256_ctx, key, key_len);
    bootutil_sha256_finish(&sha256_ctx, hash);

    plat_err = tfm_plat_get_rotpk_hash(image_id, key_hash, &key_hash_size);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return -1;
    }
    if (!boot_secure_memequal(hash, key_hash, key_hash_size)) {
        bootutil_keys[0].key = key;
        pub_key_len = key_len;
        return 0;
    }
    return -1;
}
#else /* !MCUBOOT_HW_KEY */
static int
bootutil_find_key(uint8_t *keyhash, uint8_t keyhash_len)
{
    bootutil_sha256_context sha256_ctx;
    int i;
    const struct bootutil_key *key;
    uint8_t hash[32];

    if (keyhash_len > 32) {
        return -1;
    }

    for (i = 0; i < bootutil_key_cnt; i++) {
        key = &bootutil_keys[i];
        bootutil_sha256_init(&sha256_ctx);
        bootutil_sha256_update(&sha256_ctx, key->key, *key->len);
        bootutil_sha256_finish(&sha256_ctx, hash);
        if (!boot_secure_memequal(hash, keyhash, keyhash_len)) {
            return i;
        }
    }
    return -1;
}
#endif
#endif

/**
 * Reads the value of an image's security counter.
 *
 * @param hdr           Pointer to the image header structure.
 * @param fap           Pointer to a description structure of the image's
 *                      flash area.
 * @param security_cnt  Pointer to store the security counter value.
 *
 * @return              0 on success; nonzero on failure.
 */
int32_t
bootutil_get_img_security_cnt(struct image_header *hdr,
                              const struct flash_area *fap,
                              uint32_t *img_security_cnt)
{
    struct image_tlv_iter it;
    uint32_t off;
    uint16_t len;
    int32_t rc;

    if ((hdr == NULL) ||
        (fap == NULL) ||
        (img_security_cnt == NULL)) {
        /* Invalid parameter. */
        return BOOT_EBADARGS;
    }

    /* The security counter TLV is in the protected part of the TLV area. */
    if (hdr->ih_protect_tlv_size == 0) {
        return BOOT_EBADIMAGE;
    }

    rc = bootutil_tlv_iter_begin(&it, hdr, fap, IMAGE_TLV_SEC_CNT, true);
    if (rc) {
        return rc;
    }

    /* Traverse through the protected TLV area to find
     * the security counter TLV.
     */

    rc = bootutil_tlv_iter_next(&it, &off, &len, NULL);
    if (rc != 0) {
        /* Security counter TLV has not been found. */
        return -1;
    }

    if (len != sizeof(*img_security_cnt)) {
        /* Security counter is not valid. */
        return BOOT_EBADIMAGE;
    }

    rc = LOAD_IMAGE_DATA(hdr, fap, off, img_security_cnt, len);
    if (rc != 0) {
        return BOOT_EFLASH;
    }

    return 0;
}

/*
 * Verify the integrity of the image.
 * Return non-zero if image could not be validated/does not validate.
 */
int
bootutil_img_validate(int image_index,
                      struct image_header *hdr, const struct flash_area *fap,
                      uint8_t *tmp_buf, uint32_t tmp_buf_sz, uint8_t *seed,
                      int seed_len, uint8_t *out_hash)
{
    uint32_t off;
    uint16_t len;
    uint8_t type;
    int sha256_valid = 0;
#ifdef EXPECTED_SIG_TLV
    int valid_signature = 0;
    int key_id = -1;
#ifdef MCUBOOT_HW_KEY
    /* Few extra bytes for encoding and for public exponent */
    uint8_t key_buf[SIG_BUF_SIZE + 24];
#endif
#endif
    struct image_tlv_iter it;
    uint8_t buf[SIG_BUF_SIZE];
    uint8_t hash[32] = {0};
    uint32_t security_cnt;
    uint32_t img_security_cnt;
    int32_t security_counter_valid = 0;
    int rc;

    rc = bootutil_img_hash(image_index, hdr, fap, tmp_buf,
            tmp_buf_sz, hash, seed, seed_len);
    if (rc) {
        return rc;
    }

    if (out_hash) {
        memcpy(out_hash, hash, 32);
    }

    rc = bootutil_tlv_iter_begin(&it, hdr, fap, IMAGE_TLV_ANY, false);
    if (rc) {
        return rc;
    }

    /*
     * Traverse through all of the TLVs, performing any checks we know
     * and are able to do.
     */
    while (true) {
        rc = bootutil_tlv_iter_next(&it, &off, &len, &type);
        if (rc < 0) {
            return -1;
        } else if (rc > 0) {
            break;
        }

        if (type == IMAGE_TLV_SHA256) {
            /*
             * Verify the SHA256 image hash.  This must always be
             * present.
             */
            if (len != sizeof(hash)) {
                return -1;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, buf, sizeof(hash));
            if (rc) {
                return rc;
            }
            if (boot_secure_memequal(hash, buf, sizeof(hash))) {
                return -1;
            }

            sha256_valid = 1;
#ifdef EXPECTED_SIG_TLV
#ifndef MCUBOOT_HW_KEY
        } else if (type == IMAGE_TLV_KEYHASH) {
            /*
             * Determine which key we should be checking.
             */
            if (len > 32) {
                return -1;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, buf, len);
            if (rc) {
                return rc;
            }
            key_id = bootutil_find_key(buf, len);
            /*
             * The key may not be found, which is acceptable.  There
             * can be multiple signatures, each preceded by a key.
             */
#else /* MCUBOOT_HW_KEY */
        } else if (type == IMAGE_TLV_KEY) {
            /*
             * Determine which key we should be checking.
             */
            if (len > sizeof(key_buf)) {
                return -1;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, key_buf, len);
            if (rc) {
                return rc;
            }
            key_id = bootutil_find_key(image_index, key_buf, len);
            /*
             * The key may not be found, which is acceptable.  There
             * can be multiple signatures, each preceded by a key.
             */
#endif /* MCUBOOT_HW_KEY */
        } else if (type == EXPECTED_SIG_TLV) {
            /* Ignore this signature if it is out of bounds. */
            if (key_id < 0 || key_id >= bootutil_key_cnt) {
                key_id = -1;
                continue;
            }
            if (!EXPECTED_SIG_LEN(len) || len > sizeof(buf)) {
                return -1;
            }
            rc = LOAD_IMAGE_DATA(hdr, fap, off, buf, len);
            if (rc) {
                return -1;
            }
            rc = bootutil_verify_sig(hash, sizeof(hash), buf, len, key_id);
            if (rc == 0) {
                valid_signature = 1;
            }
            key_id = -1;
#endif
        } else if (type == IMAGE_TLV_SEC_CNT) {
            /*
             * Verify the image's security counter.
             * This must always be present.
             */
            if (len != sizeof(img_security_cnt)) {
                /* Security counter is not valid. */
                return -1;
            }

            rc = LOAD_IMAGE_DATA(hdr, fap, off, &img_security_cnt, len);
            if (rc) {
                return rc;
            }

            rc = boot_nv_security_counter_get(image_index, &security_cnt);
            if (rc) {
                return rc;
            }

            /* Compare the new image's security counter value against the
             * stored security counter value.
             */
            if (img_security_cnt < security_cnt) {
                /* The image's security counter is not accepted. */
                return -1;
            }

            /* The image's security counter has been successfully verified. */
            security_counter_valid = 1;
        }
    }

    if (!sha256_valid || !security_counter_valid) {
        return -1;
    }

#ifdef EXPECTED_SIG_TLV
    if (!valid_signature) {
        return -1;
    }
#endif

    return 0;
}
