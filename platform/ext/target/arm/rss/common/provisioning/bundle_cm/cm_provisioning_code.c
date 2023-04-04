/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_otp.h"
#include "rss_provisioning_bundle.h"
#include "trng.h"

/* This is a stub to make the linker happy */
void __Vectors(){}

extern const struct cm_provisioning_data data;

enum tfm_plat_err_t __attribute__((section("DO_PROVISION"))) do_provision(void) {
    enum tfm_plat_err_t err;
    uint32_t new_lcs;
    uint32_t bl1_2_len = sizeof(data.bl1_2_image);
    uint8_t huk[32];
    int32_t int_err;

    err = tfm_plat_otp_write(PLAT_OTP_ID_GUK,
                             sizeof(data.guk),
                             data.guk);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_2_IMAGE_HASH,
                             sizeof(data.bl1_2_image_hash),
                             data.bl1_2_image_hash);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_2_IMAGE,
                             sizeof(data.bl1_2_image),
                             data.bl1_2_image);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_2_IMAGE_LEN,
                             sizeof(bl1_2_len),
                             (uint8_t *)&bl1_2_len);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_CCA_SYSTEM_PROPERTIES,
                             sizeof(data.cca_system_properties),
                             (uint8_t*)&data.cca_system_properties);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    int_err = bl1_trng_generate_random(huk, sizeof(huk));
    if (int_err != 0) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_HUK, sizeof(huk), huk);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    new_lcs = PLAT_OTP_LCS_PSA_ROT_PROVISIONING;
    err = tfm_plat_otp_write(PLAT_OTP_ID_LCS, sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return err;
}
