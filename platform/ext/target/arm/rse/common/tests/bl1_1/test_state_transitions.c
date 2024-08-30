/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "test_state_transitions.h"

#include "cc3xx_drv.h"
#include "device_definition.h"
#include "tfm_plat_otp.h"

void rse_test_virgin_to_tci_or_pci(struct test_result_t *ret)
{
    enum lcm_tp_mode_t tp_mode;
    enum lcm_error_t lcm_err;
    cc3xx_err_t cc_err;
    uint32_t random_word;

    cc_err = cc3xx_lowlevel_rng_get_random(&random_word, sizeof(random_word),
                                           CC3XX_RNG_CRYPTOGRAPHICALLY_SECURE);
    if (cc_err != CC3XX_ERR_SUCCESS) {
        TEST_FAIL("RNG coin flip failed");
        return;
    }

    if (random_word & 0b1) {
        tp_mode = LCM_TP_MODE_PCI;
    } else {
        tp_mode = LCM_TP_MODE_TCI;
    }

    printf_set_color(MAGENTA);
    TEST_LOG("Entering %s mode\r\n", tp_mode == LCM_TP_MODE_PCI ? "PCI" : "TCI");
    printf_set_color(DEFAULT);

    lcm_err = lcm_set_tp_mode(&LCM_DEV_S, tp_mode);
    if (lcm_err != LCM_ERROR_NONE) {
        TEST_FAIL("LCM TP mode set failed");
        return;
    }

    tfm_hal_system_reset();

    ret->val = TEST_PASSED;
    return;
}

void rse_test_cm_sp_enable(struct test_result_t *ret)
{
    enum lcm_error_t lcm_err;

    lcm_err = lcm_set_sp_enabled(&LCM_DEV_S);
    if (lcm_err != LCM_ERROR_NONE) {
        TEST_FAIL("LCM CM SP mode enable failed");
        return;
    }

    ret->val = TEST_PASSED;
    return;
}

void rse_test_cm_to_dm(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;
    uint32_t new_lcs;

    new_lcs = PLAT_OTP_LCS_PSA_ROT_PROVISIONING;
    plat_err = tfm_plat_otp_write(PLAT_OTP_ID_LCS, sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        TEST_FAIL("LCM CM to DM transition failed");
        return;
    }

    tfm_hal_system_reset();

    ret->val = TEST_PASSED;
    return;
}

void rse_test_dm_sp_enable(struct test_result_t *ret)
{
    enum lcm_error_t lcm_err;

    lcm_err = lcm_set_sp_enabled(&LCM_DEV_S);
    if (lcm_err != LCM_ERROR_NONE) {
        TEST_FAIL("LCM DM SP mode enable failed");
        return;
    }

    ret->val = TEST_PASSED;
    return;
}

void rse_test_dm_to_se(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;
    uint32_t new_lcs;

    new_lcs = PLAT_OTP_LCS_SECURED;
    plat_err = tfm_plat_otp_write(PLAT_OTP_ID_LCS, sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        TEST_FAIL("LCM DM to SE transition failed");
        return;
    }

    tfm_hal_system_reset();

    ret->val = TEST_PASSED;
    return;
}

void rse_test_se_to_rma(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;
    uint32_t new_lcs;

    new_lcs = PLAT_OTP_LCS_DECOMMISSIONED;
    plat_err = tfm_plat_otp_write(PLAT_OTP_ID_LCS, sizeof(new_lcs),
                             (uint8_t*)&new_lcs);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        TEST_FAIL("LCM SE to RMA transition failed");
        return;
    }

    tfm_hal_system_reset();

    ret->val = TEST_PASSED;
    return;
}
