/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rse_boot_state.h"

#include "cc3xx_drv.h"
#include "tfm_plat_otp.h"
#include "device_definition.h"

extern uint8_t computed_bl1_2_hash[];

int rse_get_boot_state(uint8_t *state, size_t state_buf_len,
                       size_t *state_size, boot_state_include_mask mask)
{
    cc3xx_err_t err;

    enum tfm_plat_err_t plat_err;
    enum plat_otp_lcs_t lcs;
    enum lcm_tp_mode_t tp_mode;
    uint32_t reprovisioning_bits;

    if (state_buf_len < 32) {
        return 1;
    }

    err = cc3xx_lowlevel_hash_init(CC3XX_HASH_ALG_SHA256);
    if (err != CC3XX_ERR_SUCCESS) {
        return -1;
    }

    if (mask & RSE_BOOT_STATE_INCLUDE_TP_MODE) {
        lcm_get_tp_mode(&LCM_DEV_S, &tp_mode);

        err = cc3xx_lowlevel_hash_update((uint8_t *)&tp_mode, sizeof(tp_mode));
        if (err != CC3XX_ERR_SUCCESS) {
            return -1;
        }
    }

    if (mask & RSE_BOOT_STATE_INCLUDE_LCS) {
        plat_err = tfm_plat_otp_read(PLAT_OTP_ID_LCS, sizeof(lcs), (uint8_t *)&lcs);
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return -2;
        }

        err = cc3xx_lowlevel_hash_update((uint8_t *)&lcs, sizeof(lcs));
        if (err != CC3XX_ERR_SUCCESS) {
            return -1;
        }
    }

    if (mask & RSE_BOOT_STATE_INCLUDE_REPROVISIONING_BITS) {
        plat_err = tfm_plat_otp_read(PLAT_OTP_ID_REPROVISIONING_BITS,
                               sizeof(reprovisioning_bits),
                               (uint8_t *)&reprovisioning_bits);
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return -2;
        }

        err = cc3xx_lowlevel_hash_update((uint8_t *)&reprovisioning_bits, sizeof(reprovisioning_bits));
        if (err != CC3XX_ERR_SUCCESS) {
            return -1;
        }
    }

    if (mask & RSE_BOOT_STATE_INCLUDE_BL1_2_HASH) {
        err = cc3xx_lowlevel_hash_update(computed_bl1_2_hash, 32);
        if (err != CC3XX_ERR_SUCCESS) {
            return -1;
        }
    }

    cc3xx_lowlevel_hash_finish((uint32_t *)state, SHA256_OUTPUT_SIZE);
    *state_size = SHA256_OUTPUT_SIZE;

    return 0;
}

