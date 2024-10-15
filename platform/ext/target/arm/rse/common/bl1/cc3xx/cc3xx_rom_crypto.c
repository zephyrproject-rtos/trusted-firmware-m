/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto.h"

#include <stdint.h>
#include <string.h>

#include "region_defs.h"
#include "tfm_plat_defs.h"
#include "device_definition.h"
#include "otp.h"
#include "fih.h"
#include "cc3xx_drv.h"
#include "kmu_drv.h"

#define KEY_DERIVATION_MAX_BUF_SIZE 128

fih_int bl1_sha256_init(void)
{
    fih_int fih_rc;

    fih_rc = fih_int_encode_zero_equality(cc3xx_lowlevel_hash_init(CC3XX_HASH_ALG_SHA256));
    if(fih_not_eq(fih_rc, FIH_SUCCESS)) {
        FIH_RET(fih_rc);
    }

    return FIH_SUCCESS;
}

fih_int bl1_sha256_finish(uint8_t *hash)
{
    uint32_t tmp_buf[32 / sizeof(uint32_t)];

    cc3xx_lowlevel_hash_finish(tmp_buf, 32);

    memcpy(hash, tmp_buf, sizeof(tmp_buf));

    return FIH_SUCCESS;
}

fih_int bl1_sha256_update(uint8_t *data, size_t data_length)
{
    fih_int fih_rc;

    fih_rc = fih_int_encode_zero_equality(cc3xx_lowlevel_hash_update(data,
                                                                     data_length));
    if(fih_not_eq(fih_rc, FIH_SUCCESS)) {
        FIH_RET(fih_rc);
    }

    return FIH_SUCCESS;
}

fih_int bl1_sha256_compute(const uint8_t *data,
                           size_t data_length,
                           uint8_t *hash)
{
    uint32_t tmp_buf[32 / sizeof(uint32_t)];
    fih_int fih_rc;

    if (data == NULL || hash == NULL) {
        FIH_RET(TFM_PLAT_ERR_ROM_CRYPTO_SHA256_COMPUTE_INVALID_INPUT);
    }

    fih_rc = fih_int_encode_zero_equality(cc3xx_lowlevel_hash_init(CC3XX_HASH_ALG_SHA256));
    if(fih_not_eq(fih_rc, FIH_SUCCESS)) {
        FIH_RET(fih_rc);
    }

    fih_rc = fih_int_encode_zero_equality(cc3xx_lowlevel_hash_update(data,
                                                                     data_length));
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        FIH_RET(fih_rc);
    }
    cc3xx_lowlevel_hash_finish(tmp_buf, 32);

    memcpy(hash, tmp_buf, sizeof(tmp_buf));

    FIH_RET(FIH_SUCCESS);
}

static fih_int bl1_key_to_kmu_key(enum tfm_bl1_key_id_t key_id,
                                  enum kmu_hardware_keyslot_t *cc3xx_key_type,
                                  uint8_t **key_buf, size_t key_buf_size)
{
    fih_int fih_rc;

    switch(key_id) {
    case TFM_BL1_KEY_HUK:
        *cc3xx_key_type = KMU_HW_SLOT_HUK;
        *key_buf = NULL;
        break;
    case TFM_BL1_KEY_GUK:
        *cc3xx_key_type = KMU_HW_SLOT_GUK;
        *key_buf = NULL;
        break;
    default:
        FIH_CALL(bl1_otp_read_key, fih_rc, key_id, *key_buf);
        *cc3xx_key_type = 0;
        if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
            memset(*key_buf, 0, key_buf_size);
            FIH_RET(fih_rc);
        }
        break;
    }

    FIH_RET(FIH_SUCCESS);
}

int32_t bl1_aes_256_ctr_decrypt(enum tfm_bl1_key_id_t key_id,
                                const uint8_t *key_material,
                                uint8_t *counter,
                                const uint8_t *ciphertext,
                                size_t ciphertext_length,
                                uint8_t *plaintext)
{
    enum kmu_hardware_keyslot_t kmu_key_slot;
    uint32_t key_buf[32 / sizeof(uint32_t)];
    uint8_t *input_key = (uint8_t *)key_buf;
    fih_int fih_rc;
    cc3xx_err_t cc_err;

    if (ciphertext_length == 0) {
        return TFM_PLAT_ERR_SUCCESS;
    }

    if (counter == NULL || ciphertext == NULL || plaintext == NULL) {
        return TFM_PLAT_ERR_ROM_CRYPTO_AES256_CTR_DECRYPT_INVALID_INPUT;
    }

    if ((uintptr_t)counter & 0x3) {
        return TFM_PLAT_ERR_ROM_CRYPTO_AES256_CTR_DECRYPT_INVALID_ALIGNMENT;
    }

    if (key_material == NULL) {
        fih_rc = bl1_key_to_kmu_key(key_id, &kmu_key_slot, &input_key,
                                    sizeof(key_buf));
        if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
            return fih_int_decode(fih_rc);
        }
    } else {
        input_key = (uint8_t *)key_material;
        kmu_key_slot = 0;
    }

    cc_err = cc3xx_lowlevel_aes_init(CC3XX_AES_DIRECTION_DECRYPT, CC3XX_AES_MODE_CTR,
                                     kmu_key_slot, (uint32_t *)input_key,
                                     CC3XX_AES_KEYSIZE_256, (uint32_t *)counter, 16);
    if (cc_err != CC3XX_ERR_SUCCESS) {
        return cc_err;
    }

    cc3xx_lowlevel_aes_set_output_buffer(plaintext, ciphertext_length);
    cc3xx_lowlevel_aes_update(ciphertext, ciphertext_length);
    cc3xx_lowlevel_aes_finish(NULL, NULL);

    return TFM_PLAT_ERR_SUCCESS;
}

int32_t bl1_derive_key(enum tfm_bl1_key_id_t key_id, const uint8_t *label,
                       size_t label_length, const uint8_t *context,
                       size_t context_length, uint8_t *output_key,
                       size_t output_length)
{
    enum kmu_hardware_keyslot_t kmu_key_slot;
    uint32_t key_buf[32 / sizeof(uint32_t)];
    uint8_t *input_key = (uint8_t *)key_buf;
    fih_int fih_rc;
    cc3xx_err_t cc_err;

    fih_rc = bl1_key_to_kmu_key(key_id, &kmu_key_slot, &input_key, sizeof(key_buf));
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        return fih_int_decode(fih_rc);
    }

    cc_err = cc3xx_lowlevel_kdf_cmac(kmu_key_slot, (uint32_t *)input_key,
                                     CC3XX_AES_KEYSIZE_256, label, label_length, context,
                                     context_length, (uint32_t *)output_key, output_length);
    if (cc_err != CC3XX_ERR_SUCCESS) {
        return cc_err;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
