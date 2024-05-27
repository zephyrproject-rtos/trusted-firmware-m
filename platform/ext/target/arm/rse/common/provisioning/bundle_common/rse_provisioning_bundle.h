/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_PROVISIONING_BUNDLE_H__
#define __RSE_PROVISIONING_BUNDLE_H__

#include "stdint.h"
#include "region_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MCUBOOT_SIGN_EC384
#define BL2_ROTPK_HASH_SIZE     (48)
#define BL2_ROTPK_KEY_SIZE      (100) /* Size must be aligned to 4 Bytes */
#else
#define BL2_ROTPK_HASH_SIZE     (32)
#define BL2_ROTPK_KEY_SIZE      (68)  /* Size must be aligned to 4 Bytes */
#endif /* MCUBOOT_SIGN_EC384 */

#ifdef MCUBOOT_BUILTIN_KEY
#define PROV_ROTPK_DATA_SIZE    BL2_ROTPK_KEY_SIZE
#else
#define PROV_ROTPK_DATA_SIZE    BL2_ROTPK_HASH_SIZE
#endif /* MCUBOOT_BUILTIN_KEY */

#define CM_BUNDLE_MAGIC 0xAAAAC0DEFEEDAAAA
#define DM_BUNDLE_MAGIC 0xAAAABEEFFEEDAAAA

struct __attribute__((__packed__)) cm_provisioning_bundle {
    /* This section is authenticated */
    uint64_t magic;
    /* This section is encrypted */
    uint8_t code[PROVISIONING_BUNDLE_CODE_SIZE];
    union __attribute__((__packed__)) {
        struct __attribute__((__packed__)) cm_provisioning_data {
            uint8_t bl1_2_image_hash[32];
            uint8_t bl1_2_image[BL1_2_CODE_SIZE];
            uint8_t dma_otp_ics[OTP_DMA_ICS_SIZE];
            uint8_t guk[32];
            uint32_t cca_system_properties;
        } values;
        uint8_t _pad[PROVISIONING_BUNDLE_VALUES_SIZE];
    };
    uint8_t data[PROVISIONING_BUNDLE_DATA_SIZE];
    /* This section is metadata */
    uint32_t iv[3];
    uint32_t tag[4];
    uint64_t magic2;
};

struct __attribute__((__packed__)) dm_provisioning_bundle {
    /* This section is authenticated */
    uint64_t magic;
    /* This section is encrypted */
    uint8_t code[PROVISIONING_BUNDLE_CODE_SIZE];
    union __attribute__((__packed__)) {
        struct __attribute__((__packed__)) dm_provisioning_data {
            uint32_t rse_id;
            uint32_t rse_to_rse_sender_routing_table[RSE_AMOUNT];
            uint32_t rse_to_rse_receiver_routing_table[RSE_AMOUNT];

            uint8_t bl1_rotpk_0[56];
            uint8_t bl2_encryption_key[32];
            uint8_t bl2_rotpk[MCUBOOT_IMAGE_NUMBER][PROV_ROTPK_DATA_SIZE];
            uint8_t s_image_encryption_key[32];
            uint8_t ns_image_encryption_key[32];

            uint8_t host_rotpk_s[96];
            uint8_t host_rotpk_ns[96];
            uint8_t host_rotpk_cca[96];
            uint8_t implementation_id[32];
            uint8_t verification_service_url[32];
            uint8_t profile_definition[32];
            uint8_t secure_debug_pk[32];
        } values;
        uint8_t _pad[PROVISIONING_BUNDLE_VALUES_SIZE];
    };
    uint8_t data[PROVISIONING_BUNDLE_DATA_SIZE];
    /* This section is metadata */
    uint32_t iv[3];
    uint32_t tag[4];
    uint64_t magic2;
};

#ifdef __cplusplus
}
#endif

#endif /* __RSE_PROVISIONING_BUNDLE_H__ */
