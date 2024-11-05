/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef BL1_2_IMAGE_H
#define BL1_2_IMAGE_H

#include <stddef.h>
#include <stdint.h>
#include "region_defs.h"
#include "cmsis_compiler.h"
#include "mbedtls/lms.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BL1_2_IMAGE_DECRYPT_MAGIC_EXPECTED 0xDEADBEEF

#define BL1_ROTPK_MAX_SIZE MBEDTLS_LMS_PUBLIC_KEY_LEN(MBEDTLS_LMS_SHA256_M32_H10)
#define BL1_SIG_MAX_SIZE 1452
#define PAD_SIZE (BL1_HEADER_SIZE - CTR_IV_LEN - 1452 - \
                  sizeof(struct tfm_bl1_image_version_t) - 2 * sizeof(uint32_t))

#define BL1_CONFIG_BL1_2_MAX_SIGNERS 2
#define BL1_CONFIG_BL1_2_EMBED_ROTPK_IN_IMAGE

__PACKED_STRUCT tfm_bl1_image_version_t {
    uint8_t  major;
    uint8_t  minor;
    uint16_t revision;
    uint32_t build_num;
};

__PACKED_STRUCT bl1_2_image_t {
    __PACKED_STRUCT  {
        uint8_t ctr_iv[CTR_IV_LEN];
        uint8_t sig[BL1_SIG_MAX_SIZE];
        __PACKED_STRUCT {
#ifdef BL1_CONFIG_BL1_2_EMBED_ROTPK_IN_IMAGE
            uint8_t rotpk[BL1_ROTPK_MAX_SIZE];
            size_t rotpk_len;
#endif
            uint8_t sig[BL1_SIG_MAX_SIZE];
            size_t sig_len;
        } signers[BL1_CONFIG_BL1_2_MAX_SIGNERS];
    } header;
    __PACKED_STRUCT {
        struct tfm_bl1_image_version_t version;
        uint32_t security_counter;

        __PACKED_STRUCT {
            uint32_t decrypt_magic;
            uint8_t pad[PAD_SIZE];
            uint8_t data[IMAGE_BL2_CODE_SIZE];
        } encrypted_data;
    } protected_values;
};

uint32_t bl1_image_get_flash_offset(uint32_t image_id);

fih_int bl1_image_copy_to_sram(uint32_t image_id, uint8_t *out);

#ifdef __cplusplus
}
#endif

#endif /* BL1_2_IMAGE_H */
