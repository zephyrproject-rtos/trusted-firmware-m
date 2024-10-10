/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_OTP_CHECK_CONFIG_H__
#define __RSE_OTP_CHECK_CONFIG_H__

#if RSE_OTP_CM_ROTPK_AMOUNT > 6
#error ">6 ROTPKs overflows allocated space for key types"
#endif

#if RSE_OTP_DM_ROTPK_AMOUNT > 6
#error ">6 ROTPKs overflows allocated space for key types"
#endif

#if defined(MCUBOOT_HW_KEY) != defined(RSE_OTP_CM_ROTPK_IS_HASH_NOT_KEY)
#error "MCUBOOT_HW_KEY requires RSE_OTP_CM_ROTPK_IS_HASH_NOT_KEY"
#endif

#if defined(MCUBOOT_HW_KEY) != defined(RSE_OTP_DM_ROTPK_IS_HASH_NOT_KEY)
#error "MCUBOOT_HW_KEY requires RSE_OTP_DM_ROTPK_IS_HASH_NOT_KEY"
#endif

#if defined(RSE_OTP_CM_ROTPK_IS_HASH_NOT_KEY) != defined(TFM_BL1_2_EMBED_ROTPK_IN_IMAGE)
#error "RSE_OTP_CM_ROTPK_IS_HASH_NOT_KEY requires TFM_BL1_2_EMBED_ROTPK_IN_IMAGE"
#endif

#if defined(RSE_OTP_DM_ROTPK_IS_HASH_NOT_KEY) != defined(TFM_BL1_2_EMBED_ROTPK_IN_IMAGE)
#error "RSE_OTP_DM_ROTPK_IS_HASH_NOT_KEY requires TFM_BL1_2_EMBED_ROTPK_IN_IMAGE"
#endif

#endif /* __RSE_OTP_CHECK_CONFIG_H__ */
