/*
 * Copyright (c) 2023-2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_kdf.h"

#include "cc3xx_rng.h"
#include "cc3xx_stdlib.h"

#include <assert.h>

/* This is a NIST SP-800-108 counter mode KDF with CMAC as the pseudo-random
 * function, using the modified Label || 0x00 || Context || [L]2 || K (0)
 * construction where K(0) = PRF(K IN, Label || 0x00 || Context || [L]2 )
 * proposed as a countermeasure for CMAC KDFs in SP-800-108 section 4.1.
 */
cc3xx_err_t cc3xx_lowlevel_kdf_cmac(
    cc3xx_aes_key_id_t key_id, const uint32_t *key,
    cc3xx_aes_keysize_t key_size,
    const uint8_t *label, size_t label_length,
    const uint8_t *context, size_t context_length,
    uint32_t *output_key, size_t out_length)
{
    uint32_t i_idx = 1;
    uint32_t l_total_length = out_length;
    uint8_t null_byte = 0;
    cc3xx_err_t err;
    uint32_t cmac_buf[AES_TAG_MAX_LEN / sizeof(uint32_t)];
    uint32_t k0[AES_TAG_MAX_LEN / sizeof(uint32_t)];
    size_t output_idx;

    /* Check alignment */
    assert(((uintptr_t)output_key & 0b11) == 0);
    assert((out_length / sizeof(cmac_buf)) != 0);

    err = cc3xx_lowlevel_aes_init(CC3XX_AES_DIRECTION_ENCRYPT, CC3XX_AES_MODE_CMAC,
                                  key_id, key, key_size, NULL, 0);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    cc3xx_lowlevel_aes_set_tag_len(AES_TAG_MAX_LEN);

    cc3xx_lowlevel_aes_update_authed_data(label, label_length);
    cc3xx_lowlevel_aes_update_authed_data(&null_byte, sizeof(null_byte));
    cc3xx_lowlevel_aes_update_authed_data(context, context_length);
    cc3xx_lowlevel_aes_update_authed_data((uint8_t *)&l_total_length,
                                 sizeof(l_total_length));
    cc3xx_lowlevel_aes_finish(k0, NULL);

    for(output_idx = 0; output_idx < out_length; output_idx += sizeof(cmac_buf)) {
        err = cc3xx_lowlevel_aes_init(CC3XX_AES_DIRECTION_ENCRYPT, CC3XX_AES_MODE_CMAC,
                                      key_id, key, key_size, NULL, 0);
        if (err != CC3XX_ERR_SUCCESS) {
            goto out;
        }

        cc3xx_lowlevel_aes_set_tag_len(AES_TAG_MAX_LEN);

        cc3xx_lowlevel_aes_update_authed_data((uint8_t *)&i_idx, sizeof(i_idx));
        cc3xx_lowlevel_aes_update_authed_data(label, label_length);
        cc3xx_lowlevel_aes_update_authed_data(&null_byte, sizeof(null_byte));
        cc3xx_lowlevel_aes_update_authed_data(context, context_length);
        cc3xx_lowlevel_aes_update_authed_data((uint8_t *)&l_total_length,
                                     sizeof(l_total_length));
        cc3xx_lowlevel_aes_update_authed_data((uint8_t *)k0, sizeof(k0));
        cc3xx_lowlevel_aes_finish(cmac_buf, NULL);

        cc3xx_dpa_hardened_word_copy((void *)output_key + output_idx, cmac_buf,
                                     sizeof(cmac_buf) / sizeof(uint32_t));
        i_idx += 1;
    }

    err = CC3XX_ERR_SUCCESS;
out:
    cc3xx_secure_erase_buffer(k0, sizeof(k0) / sizeof(uint32_t));
    cc3xx_secure_erase_buffer(cmac_buf, sizeof(cmac_buf) / sizeof(uint32_t));

    if (err != CC3XX_ERR_SUCCESS) {
        cc3xx_secure_erase_buffer(output_key, out_length / sizeof(uint32_t));
    }

    return err;
}
