/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdlib.h>
#include <string.h>

#include "Driver_Flash.h"
#include "region_defs.h"
#include "image.h"
#include "fih.h"

#include "mock_tfm_plat_otp.h"

#include "unity.h"

int32_t data_read;
size_t bl1_2_len = BL1_2_CODE_SIZE;
uint8_t bl1_2_image[BL1_2_CODE_SIZE] = {0};

static int32_t UNITTEST_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    return data_read;
}

ARM_DRIVER_FLASH UNITTEST_FLASH_DEV = {
    .ReadData = UNITTEST_Flash_ReadData
};

void test_bl1_read_bl1_2_image_invalid_bl1_2_len_read(void)
{
    fih_int fih_rc;

    /* Prepare */
    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE_LEN, sizeof(uint32_t),
                                      NULL, TFM_PLAT_ERR_OTP_READ_UNSUPPORTED);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out((uint8_t *)&bl1_2_len, sizeof(bl1_2_len));

    /* Act */
    fih_rc = bl1_read_bl1_2_image(NULL);

    /* Assert */
    TEST_ASSERT_EQUAL(TFM_PLAT_ERR_OTP_READ_UNSUPPORTED, fih_rc);
}

void test_bl1_read_bl1_2_image_bl1_2_image_otp_invalid_read(void)
{
    fih_int fih_rc;

    /* Prepare */
    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE_LEN, sizeof(uint32_t),
                                      NULL, TFM_PLAT_ERR_SUCCESS);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out((uint8_t *)&bl1_2_len, sizeof(bl1_2_len));

    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE, bl1_2_len,
                                      NULL, TFM_PLAT_ERR_OTP_READ_UNSUPPORTED);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out(bl1_2_image, sizeof(bl1_2_image));

    /* Act */
    fih_rc = bl1_read_bl1_2_image(bl1_2_image);

    /* Assert */
    TEST_ASSERT_EQUAL(TFM_PLAT_ERR_OTP_READ_UNSUPPORTED, fih_rc);
}

void test_bl1_read_bl1_2_image_bl1_2_image_otp_ok(void)
{
    fih_int fih_rc;

    /* Prepare */
    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE_LEN, sizeof(uint32_t),
                                      NULL, TFM_PLAT_ERR_SUCCESS);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out((uint8_t *)&bl1_2_len, sizeof(bl1_2_len));

    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE, bl1_2_len,
                                      NULL, TFM_PLAT_ERR_SUCCESS);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out(bl1_2_image, sizeof(bl1_2_image));

    /* Act */
    fih_rc = bl1_read_bl1_2_image(bl1_2_image);

    /* Assert */
    TEST_ASSERT_EQUAL(TFM_PLAT_ERR_SUCCESS, fih_rc);
}

void test_bl1_read_bl1_2_image_bl1_2_image_flash_invalid_size(void)
{
    fih_int fih_rc;

    /* Prepare */
    data_read = 0;

    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE_LEN, sizeof(uint32_t),
                                      NULL, TFM_PLAT_ERR_SUCCESS);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out((uint8_t *)&bl1_2_len, sizeof(bl1_2_len));

    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE, bl1_2_len,
                                      NULL, TFM_PLAT_ERR_OTP_EMULATION_UNSUPPORTED);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out(bl1_2_image, sizeof(bl1_2_image));

    /* Act */
    fih_rc = bl1_read_bl1_2_image(bl1_2_image);

    /* Assert */
    TEST_ASSERT_EQUAL(TFM_PLAT_ERR_READ_BL1_2_IMAGE_FLASH_INVALID_READ, fih_rc);
}

void test_bl1_read_bl1_2_image_bl1_2_image_flash_ok(void)
{
    fih_int fih_rc;

    /* Prepare */
    data_read = BL1_2_CODE_SIZE;

    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE_LEN, sizeof(uint32_t),
                                      NULL, TFM_PLAT_ERR_SUCCESS);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out((uint8_t *)&bl1_2_len, sizeof(bl1_2_len));

    tfm_plat_otp_read_ExpectAndReturn(PLAT_OTP_ID_BL1_2_IMAGE, bl1_2_len,
                                      NULL, TFM_PLAT_ERR_OTP_EMULATION_UNSUPPORTED);
    tfm_plat_otp_read_IgnoreArg_out();
    tfm_plat_otp_read_ReturnMemThruPtr_out(bl1_2_image, sizeof(bl1_2_image));

    /* Act */
    fih_rc = bl1_read_bl1_2_image(bl1_2_image);

    /* Assert */
    TEST_ASSERT_EQUAL(TFM_PLAT_ERR_SUCCESS, fih_rc);
}
