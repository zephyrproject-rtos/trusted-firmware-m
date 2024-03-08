/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_otp.h"
#include "rse_provisioning_bundle.h"

#include "flash_layout.h"
#include "Driver_Flash.h"
#include "tfm_hal_device_header.h"

#include "device_definition.h"

#include "trng.h"
#include "log.h"

/* This is a stub to make the linker happy */
void __Vectors(){}

extern const struct cm_provisioning_data data;

#ifdef RSE_BRINGUP_OTP_EMULATION
/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

static tfm_plat_err_t flash_write(uint8_t *buf, uint32_t size, uint32_t offset)
{
    int32_t int_err;
    ARM_FLASH_CAPABILITIES driver_capabilities = FLASH_DEV_NAME.GetCapabilities();
    ARM_FLASH_INFO *flash_info = FLASH_DEV_NAME.GetInfo();
    uint32_t data_width_byte[] = {
        sizeof(uint8_t),
        sizeof(uint16_t),
        sizeof(uint32_t),
    };
    uint8_t data_width = data_width_byte[driver_capabilities.data_width];

    if (size % flash_info->sector_size != 0) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    if (offset % flash_info->sector_size != 0) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    for (int idx = offset; idx < offset + size; idx += flash_info->sector_size) {
        int_err = FLASH_DEV_NAME.EraseSector(idx);
        if (int_err != 0) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    int_err = FLASH_DEV_NAME.ProgramData(offset, buf, size / data_width);
    if (int_err != 0) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
#endif /* RSE_BRINGUP_OTP_EMULATION */

enum tfm_plat_err_t __attribute__((section("DO_PROVISION"))) do_provision(void) {
    enum tfm_plat_err_t err;
    uint32_t new_lcs;
    uint32_t bl1_2_len = sizeof(data.bl1_2_image);
    uint8_t __ALIGNED(INTEGRITY_CHECKER_REQUIRED_ALIGNMENT) generated_key_buf[32];
    int32_t int_err;
    enum integrity_checker_error_t ic_err;

    BL1_LOG("[INF] Provisioning GUK\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_GUK,
                             sizeof(data.guk),
                             data.guk);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning BL1_2 image\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_2_IMAGE,
                             sizeof(data.bl1_2_image),
                             data.bl1_2_image);
    if (err != TFM_PLAT_ERR_SUCCESS) {
#ifdef RSE_BRINGUP_OTP_EMULATION
        if (err == TFM_PLAT_ERR_UNSUPPORTED) {
            err = flash_write((uint8_t *)data.bl1_2_image, bl1_2_len,
                              BL1_2_IMAGE_FLASH_OFFSET);
            if (err != TFM_PLAT_ERR_SUCCESS) {
                return err;
            }
        }
#endif /* RSE_BRINGUP_OTP_EMULATION */
        return err;
    }

    BL1_LOG("[INF] Provisioning BL1_2 image length\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_2_IMAGE_LEN,
                             sizeof(bl1_2_len),
                             (uint8_t *)&bl1_2_len);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning BL1_2 image hash\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_BL1_2_IMAGE_HASH,
                             sizeof(data.bl1_2_image_hash),
                             data.bl1_2_image_hash);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning CCA system properties\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_CCA_SYSTEM_PROPERTIES,
                             sizeof(data.cca_system_properties),
                             (uint8_t*)&data.cca_system_properties);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Provisioning DMA ICS\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_DMA_ICS,
                             sizeof(data.dma_otp_ics),
                             (uint8_t*)&data.dma_otp_ics);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    int_err = bl1_trng_generate_random(generated_key_buf,
                                       sizeof(generated_key_buf));
    if (int_err != 0) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    BL1_LOG("[INF] Provisioning HUK\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_HUK,
                             sizeof(generated_key_buf), generated_key_buf);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Generating ROM OTP encryption key\r\n");
    int_err = bl1_trng_generate_random(generated_key_buf,
                                       sizeof(generated_key_buf));
    if (int_err != 0) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    BL1_LOG("[INF] Provisioning ROM OTP encryption key\r\n");
    err = tfm_plat_otp_write(PLAT_OTP_ID_ROM_OTP_ENCRYPTION_KEY,
                             sizeof(generated_key_buf), generated_key_buf);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    BL1_LOG("[INF] Transitioning to DM LCS\r\n");
    new_lcs = PLAT_OTP_LCS_PSA_ROT_PROVISIONING;
    err = tfm_plat_otp_write(PLAT_OTP_ID_LCS, sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return err;
}
