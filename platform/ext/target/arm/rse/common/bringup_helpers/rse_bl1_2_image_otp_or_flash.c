/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "region_defs.h"
#include "tfm_plat_otp.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "region_defs.h"
#include "fih.h"
#include "device_definition.h"

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

fih_int bl1_read_bl1_2_image(uint8_t *image)
{
    fih_int fih_rc;
    enum tfm_plat_err_t plat_err;
    uint32_t bl1_2_len;

    plat_err = tfm_plat_otp_read(PLAT_OTP_ID_BL1_2_IMAGE_LEN, sizeof(bl1_2_len),
                                 (uint8_t *)&bl1_2_len);
    fih_rc = fih_int_encode_zero_equality(plat_err);
    if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
        FIH_RET(fih_rc);
    }

    plat_err = tfm_plat_otp_read(PLAT_OTP_ID_BL1_2_IMAGE, bl1_2_len, image);
    if (plat_err == TFM_PLAT_ERR_UNSUPPORTED) {
        /* If this returns unsupported, then the OTP emulation isn't large
         * enough to contain the BL1_2 image.
         */
        fih_rc = fih_int_encode(FLASH_DEV_NAME.ReadData(BL1_2_IMAGE_FLASH_OFFSET,
                                                        image,
                                                        bl1_2_len));
    } else {
        fih_rc = fih_int_encode_zero_equality(plat_err);
    }

    FIH_RET(fih_rc);
}
