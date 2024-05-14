/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_otp.h"
#include "rse_provisioning_bundle.h"

#include "tfm_hal_device_header.h"

#include "trng.h"
#include "log.h"

/* This is a stub to make the linker happy */
void __Vectors(){}

extern const struct dm_provisioning_data data;

enum tfm_plat_err_t __attribute__((section("DO_PROVISION"))) do_provision(void) {
    enum tfm_plat_err_t err;
    uint32_t new_lcs;
    uint8_t __ALIGNED(8) generated_key_buf[32];
    int32_t int_err;

    BL1_LOG("[INF] Generating runtime OTP encryption key\r\n");
    int_err = bl1_trng_generate_random(generated_key_buf,
                                       sizeof(generated_key_buf));
    if (int_err != 0) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    BL1_LOG("[INF] Provisioning runtime OTP encryption key\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_RUNTIME_OTP_ENCRYPTION_KEY,
                             sizeof(generated_key_buf), generated_key_buf);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning RSE ID\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_RSE_ID,
                             sizeof(data.rse_id),
                             (const uint8_t *)&data.rse_id);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning BL2 encryption key\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_BL2_ENCRYPTION,
                             sizeof(data.bl2_encryption_key),
                             data.bl2_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning BL1 ROTPK\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_ROTPK_0,
                             sizeof(data.bl1_rotpk_0),
                             data.bl1_rotpk_0);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning BL2 ROTPKs\r\n");
    /* Assumes BL2 counters are contiguous*/
    for (int idx = 0; idx < MCUBOOT_IMAGE_NUMBER; idx++) {
        err = tfm_plat_otp_write(PLAT_OTP_ID_BL2_ROTPK_0 + idx,
                                 sizeof(data.bl2_rotpk[idx]),
                                 data.bl2_rotpk[idx]);
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    BL1_LOG("[INF] Provisioning secure image encryption key\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_SECURE_ENCRYPTION,
                             sizeof(data.s_image_encryption_key),
                             data.s_image_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    BL1_LOG("[INF] Provisioning non-secure image encryption key\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_KEY_NON_SECURE_ENCRYPTION,
                             sizeof(data.ns_image_encryption_key),
                             data.ns_image_encryption_key);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning host secure ROTPK\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_S,
                             sizeof(data.host_rotpk_s),
                             data.host_rotpk_s);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning host non-secure ROTPK\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_NS,
                             sizeof(data.host_rotpk_ns),
                             data.host_rotpk_ns);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning host CCA ROTPK\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_HOST_ROTPK_CCA,
                             sizeof(data.host_rotpk_cca),
                             data.host_rotpk_cca);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning attestation implementation ID\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_IMPLEMENTATION_ID,
                             sizeof(data.implementation_id),
                             data.implementation_id);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    BL1_LOG("[INF] Provisioning attestation verification service URL\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_VERIFICATION_SERVICE_URL,
                             sizeof(data.verification_service_url),
                             data.verification_service_url);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
    BL1_LOG("[INF] Provisioning attestation profile definition\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_PROFILE_DEFINITION,
                             sizeof(data.profile_definition),
                             data.profile_definition);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning secure debug public key\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_SECURE_DEBUG_PK,
                             sizeof(data.secure_debug_pk),
                             data.secure_debug_pk);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

#if RSE_AMOUNT > 1
    BL1_LOG("[INF] Provisioning sender routing table\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_RSE_TO_RSE_SENDER_ROUTING_TABLE,
                             sizeof(data.rse_to_rse_sender_routing_table),
                             (uint8_t *)data.rse_to_rse_sender_routing_table);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning receiver routing table\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_RSE_TO_RSE_RECEIVER_ROUTING_TABLE,
                             sizeof(data.rse_to_rse_receiver_routing_table),
                             (uint8_t *)data.rse_to_rse_receiver_routing_table);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }
#endif /* RSE_AMOUNT > 1 */

    BL1_LOG("[INF] Transitioning to SE LCS\r\n");
    new_lcs = PLAT_OTP_LCS_SECURED;
    err = tfm_plat_otp_write(PLAT_OTP_ID_LCS,
                             sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return err;
}
