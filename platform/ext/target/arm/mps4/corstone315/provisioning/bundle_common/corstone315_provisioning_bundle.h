/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CORSTONE315_PROVISIONING_BUNDLE_H__
#define __CORSTONE315_PROVISIONING_BUNDLE_H__

#include "stdint.h"
#include "region_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MCUBOOT_SIGN_EC384
#define BL2_ROTPK_HASH_SIZE     (48)
#else
#define BL2_ROTPK_HASH_SIZE     (32)
#endif

#define CM_BUNDLE_MAGIC 0xC0DEFEED
#define DM_BUNDLE_MAGIC 0xBEEFFEED

struct __attribute__((__packed__)) cm_provisioning_bundle {
    /* This section is authenticated */
    uint32_t magic;
    /* This section is encrypted */
    uint8_t code[PROVISIONING_BUNDLE_CODE_SIZE];
    union __attribute__((__packed__)) {
        struct __attribute__((__packed__)) cm_provisioning_data {
            uint8_t bl1_2_image_hash[32];
            uint8_t bl2_image_hash[32];
            uint8_t bl1_2_image[BL1_2_CODE_SIZE];
            uint32_t sam_config[OTP_SAM_CONFIGURATION_SIZE_BYTES];
            uint8_t guk[32];
        } values;
        uint8_t _pad[PROVISIONING_BUNDLE_VALUES_SIZE];
    };
    uint8_t data[PROVISIONING_BUNDLE_DATA_SIZE];
    /* This section is metadata */
    uint32_t tag[4];
    uint32_t magic2;
};

struct __attribute__((__packed__)) dm_provisioning_bundle {
    /* This section is authenticated */
    uint32_t magic;
    /* This section is encrypted */
    uint8_t code[PROVISIONING_BUNDLE_CODE_SIZE];
    union __attribute__((__packed__)) {
        struct __attribute__((__packed__)) dm_provisioning_data {
            uint8_t bl1_rotpk_0[56];
            uint8_t bl2_encryption_key[32];
            uint8_t bl2_rotpk[MCUBOOT_IMAGE_NUMBER][BL2_ROTPK_HASH_SIZE];

            uint8_t implementation_id[32];
            uint8_t verification_service_url[32];
            uint8_t profile_definition[32];
            uint8_t secure_debug_pk[32];
            uint8_t boot_seed[32];
            uint8_t cert_ref[32];
            uint8_t entropy_seed[64];
        } values;
        uint8_t _pad[PROVISIONING_BUNDLE_VALUES_SIZE];
    };
    uint8_t data[PROVISIONING_BUNDLE_DATA_SIZE];
    /* This section is metadata */
    uint32_t tag[4];
    uint32_t magic2;
};

#ifdef __cplusplus
}
#endif

#endif /* __CORSTONE315_PROVISIONING_BUNDLE_H__ */
