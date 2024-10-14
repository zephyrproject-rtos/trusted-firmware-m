/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "rse_rotpk_mapping.h"
#include "rse_otp_dev.h"

#include "rse_rotpk_config.h"

enum tfm_plat_err_t rse_rotpk_get_type(enum tfm_otp_element_id_t otp_id,
                                       enum rse_rotpk_type *type)
{

    enum tfm_otp_element_id_t rotpk_base;
    uint32_t policy_word;
    uint32_t rotpk_index;

    if (otp_id >= PLAT_OTP_ID_CM_ROTPK && otp_id < PLAT_OTP_ID_CM_ROTPK_MAX) {
        rotpk_base = PLAT_OTP_ID_CM_ROTPK;
        policy_word = P_RSE_OTP_CM->rotpk_policies;
    } else if (otp_id >= PLAT_OTP_ID_DM_ROTPK && otp_id < PLAT_OTP_ID_DM_ROTPK_MAX) {
        rotpk_base = PLAT_OTP_ID_DM_ROTPK;
        policy_word = P_RSE_OTP_DM->rotpk_policies;
    } else {
        return TFM_PLAT_ERR_ROTPK_GET_TYPE_INVALID_ID;
    }

    rotpk_index = otp_id - rotpk_base;

    *type = (policy_word >> (2 * rotpk_index)) & 0b11;
    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t rse_rotpk_get_policy(enum tfm_otp_element_id_t otp_id,
                                         enum rse_rotpk_policy *policy)
{
    enum tfm_otp_element_id_t rotpk_base;
    uint32_t policy_word;
    uint32_t rotpk_index;

    if (otp_id >= PLAT_OTP_ID_CM_ROTPK && otp_id < PLAT_OTP_ID_CM_ROTPK_MAX) {
        rotpk_base = PLAT_OTP_ID_CM_ROTPK;
        policy_word = P_RSE_OTP_CM->rotpk_policies;
    } else if (otp_id >= PLAT_OTP_ID_DM_ROTPK && otp_id < PLAT_OTP_ID_DM_ROTPK_MAX) {
        rotpk_base = PLAT_OTP_ID_DM_ROTPK;
        policy_word = P_RSE_OTP_DM->rotpk_policies;
    } else {
        return TFM_PLAT_ERR_ROTPK_GET_POLICY_INVALID_ID;
    }

    rotpk_index = otp_id - rotpk_base;

    *policy = (policy_word >> (16 + rotpk_index)) & 0b1;
    return TFM_PLAT_ERR_SUCCESS;
}


enum tfm_otp_element_id_t rse_cm_get_bl1_rotpk()
{
    return PLAT_OTP_ID_CM_ROTPK + RSE_CM_OTP_ID_FOR_BL1_IMAGE();
}

enum tfm_otp_element_id_t rse_cm_get_bl2_rotpk(uint32_t image_id)
{
    return PLAT_OTP_ID_CM_ROTPK + RSE_CM_OTP_ID_FOR_BL2_IMAGE(image_id);
}

enum tfm_otp_element_id_t rse_cm_get_host_rotpk(uint32_t host_rotpk_id)
{
    return PLAT_OTP_ID_CM_ROTPK + RSE_CM_OTP_ID_FOR_HOST(host_rotpk_id);
}

enum tfm_otp_element_id_t rse_cm_get_secure_debug_rotpk()
{
    return PLAT_OTP_ID_CM_ROTPK + RSE_CM_OTP_ID_FOR_SECURE_DEBUG();
}

enum tfm_otp_element_id_t rse_cm_get_subplatform_rotpk(uint32_t subplatform_rotpk_id)
{
    return PLAT_OTP_ID_CM_ROTPK + RSE_CM_OTP_ID_FOR_SUBPLATFORM(subplatform_rotpk_id);
}

enum tfm_otp_element_id_t rse_dm_get_bl1_rotpk()
{
    return PLAT_OTP_ID_DM_ROTPK + RSE_DM_OTP_ID_FOR_BL1_IMAGE();
}

enum tfm_otp_element_id_t rse_dm_get_bl2_rotpk(uint32_t image_id)
{
    return PLAT_OTP_ID_DM_ROTPK + RSE_DM_OTP_ID_FOR_BL2_IMAGE(image_id);
}

enum tfm_otp_element_id_t rse_dm_get_host_rotpk(uint32_t host_rotpk_id)
{
    return PLAT_OTP_ID_DM_ROTPK + RSE_DM_OTP_ID_FOR_HOST(host_rotpk_id);
}

enum tfm_otp_element_id_t rse_dm_get_secure_debug_rotpk()
{
    return PLAT_OTP_ID_DM_ROTPK + RSE_DM_OTP_ID_FOR_SECURE_DEBUG();
}

enum tfm_otp_element_id_t rse_dm_get_subplatform_rotpk(uint32_t subplatform_rotpk_id)
{
    return PLAT_OTP_ID_DM_ROTPK + RSE_DM_OTP_ID_FOR_SUBPLATFORM(subplatform_rotpk_id);
}
