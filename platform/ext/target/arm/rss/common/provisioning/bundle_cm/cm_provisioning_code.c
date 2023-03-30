/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_otp.h"
#include "rss_provisioning_bundle.h"

/* This is a stub to make the linker happy */
void __Vectors(){}

extern const struct cm_provisioning_data data;

enum tfm_plat_err_t __attribute__((section("DO_PROVISION"))) do_provision(void) {
    enum tfm_plat_err_t err;
    uint32_t new_lcs;

    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_BL2_ENCRYPTION,
                             sizeof(data.bl2_encryption_key),
                             data.bl2_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

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

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_ROTPK_0,
                             sizeof(data.bl1_rotpk_0),
                             data.bl1_rotpk_0);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_0,
                             sizeof(data.bl2_rotpk_0),
                             data.bl2_rotpk_0);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_1,
                             sizeof(data.bl2_rotpk_1),
                             data.bl2_rotpk_1);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_2,
                             sizeof(data.bl2_rotpk_2),
                             data.bl2_rotpk_2);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_3,
                             sizeof(data.bl2_rotpk_3),
                             data.bl2_rotpk_3);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_SECURE_ENCRYPTION,
                             sizeof(data.s_image_encryption_key),
                             data.s_image_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }
    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_NON_SECURE_ENCRYPTION,
                             sizeof(data.ns_image_encryption_key),
                             data.ns_image_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS && err != TFM_PLAT_ERR_UNSUPPORTED) {
        return err;
    }

    err = tfm_plat_otp_write(PLAT_OTP_ID_HUK, sizeof(data.huk), data.huk);
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
