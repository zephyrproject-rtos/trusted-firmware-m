/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_ROTPK_MAPPING_H__
#define __RSE_ROTPK_MAPPING_H__

#include "tfm_plat_otp.h"

enum rse_rotpk_type {
    RSE_ROTPK_TYPE_ECDSA = 0b00,
    RSE_ROTPK_TYPE_LMS   = 0b01,
};

enum rse_rotpk_policy {
    RSE_ROTPK_POLICY_SIG_OPTIONAL = 0b00,
    RSE_ROTPK_POLICY_SIG_REQUIRED = 0b01,
};

#ifdef __cplusplus
extern "C" {
#endif

enum tfm_plat_err_t rse_rotpk_get_type(enum tfm_otp_element_id_t otp_id,
                                       enum rse_rotpk_type *type);

enum tfm_plat_err_t rse_rotpk_get_policy(enum tfm_otp_element_id_t otp_id,
                                         enum rse_rotpk_policy *policy);

enum tfm_otp_element_id_t rse_cm_get_bl1_rotpk();
enum tfm_otp_element_id_t rse_cm_get_bl2_rotpk(uint32_t image_id);
enum tfm_otp_element_id_t rse_cm_get_host_rotpk(uint32_t host_rotpk_id);
enum tfm_otp_element_id_t rse_cm_get_secure_debug_rotpk();
enum tfm_otp_element_id_t rse_cm_get_subplatform_rotpk(uint32_t subplatform_rotpk_id);

enum tfm_otp_element_id_t rse_dm_get_bl1_rotpk();
enum tfm_otp_element_id_t rse_dm_get_bl2_rotpk(uint32_t image_id);
enum tfm_otp_element_id_t rse_dm_get_host_rotpk(uint32_t host_rotpk_id);
enum tfm_otp_element_id_t rse_dm_get_secure_debug_rotpk();
enum tfm_otp_element_id_t rse_dm_get_subplatform_rotpk(uint32_t subplatform_rotpk_id);

#ifdef __cplusplus
}
#endif

#endif /* __RSE_ROTPK_MAPPING_H__ */
