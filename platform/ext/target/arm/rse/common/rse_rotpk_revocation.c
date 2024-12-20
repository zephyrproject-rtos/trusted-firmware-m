/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rse_rotpk_revocation.h"

#include "tfm_plat_nv_counters.h"
#include "tfm_plat_otp.h"
#include "fatal_error.h"
#include "rse_otp_dev.h"
#include "rse_zero_count.h"

static enum tfm_plat_err_t revoke_rotpks(enum tfm_nv_counter_t counter_id)
{
    enum tfm_plat_err_t err;
    uint32_t current_revocation_idx;

    err = tfm_plat_read_nv_counter(counter_id, sizeof(current_revocation_idx),
                                   (uint8_t *)&current_revocation_idx);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = tfm_plat_increment_nv_counter(counter_id);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    /* Now reinit the OTP so that it picks up the new ROTPK area */
    return tfm_plat_otp_init();
}

enum tfm_plat_err_t rse_revoke_cm_rotpks()
{
    return revoke_rotpks(PLAT_NV_COUNTER_CM_ROTPK_REPROVISIONING);
}

enum tfm_plat_err_t rse_revoke_dm_rotpks()
{
    return revoke_rotpks(PLAT_NV_COUNTER_DM_ROTPK_REPROVISIONING);
}

enum tfm_plat_err_t rse_update_cm_rotpks(uint32_t policies, uint8_t *rotpks, size_t rotpks_len)
{
    enum tfm_plat_err_t err;
    enum lcm_error_t lcm_err;
    uint32_t zero_count;

    if (rotpks_len != RSE_OTP_CM_ROTPK_AMOUNT * RSE_OTP_CM_ROTPK_SIZE) {
        FATAL_ERR(TFM_PLAT_ERR_CM_ROTPK_UPDATE_INVALID_SIZE);
        return TFM_PLAT_ERR_CM_ROTPK_UPDATE_INVALID_SIZE;
    }

    /* If they're not already revoked, revoke them */
    if (P_RSE_OTP_CM_ROTPK->zero_count != 0) {
        err = rse_revoke_cm_rotpks();
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }
    }

    lcm_err = lcm_otp_write(&LCM_DEV_S, OTP_OFFSET(P_RSE_OTP_CM_ROTPK->rotpk),
                            rotpks_len, rotpks);
    if (lcm_err != LCM_ERROR_NONE) {
        return (enum tfm_plat_err_t)lcm_err;
    }

    err = rse_count_zero_bits((uint32_t *)P_RSE_OTP_CM_ROTPK->cm_rotpk_policies,
                              sizeof(struct rse_otp_cm_rotpk_area_t) - 64, /* FixMe: Check why 64 bits are excluded */
                              &zero_count);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    lcm_err = lcm_otp_write(&LCM_DEV_S, OTP_OFFSET(P_RSE_OTP_CM_ROTPK->zero_count),
                            sizeof(zero_count), (uint8_t *)&zero_count);
    if (lcm_err != LCM_ERROR_NONE) {
        return (enum tfm_plat_err_t)lcm_err;
    }

    /* Now reinit the OTP so that it picks up new keys */
    return tfm_plat_otp_init();
}

enum tfm_plat_err_t rse_update_dm_rotpks(uint32_t policies, uint8_t *rotpks, size_t rotpks_len)
{
    enum tfm_plat_err_t err;
    enum lcm_error_t lcm_err;

    if (rotpks_len != RSE_OTP_DM_ROTPK_AMOUNT * RSE_OTP_DM_ROTPK_SIZE) {
        FATAL_ERR(TFM_PLAT_ERR_DM_ROTPK_UPDATE_INVALID_SIZE);
        return TFM_PLAT_ERR_DM_ROTPK_UPDATE_INVALID_SIZE;
    }

    /* If they're not already revoked, revoke them */
    if (P_RSE_OTP_DM_ROTPK->zero_count != 0) {
        return rse_revoke_cm_rotpks();
    } else {
        /* FixMe: Define how to behave when zero_count == 0 */
    }

    return TFM_PLAT_ERR_SUCCESS;
}
