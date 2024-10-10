/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_OTP_DEV_H__
#define __RSE_OTP_DEV_H__

#include "device_definition.h"
#include "rse_otp_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RSE_OTP_HAS_CM_AREA
extern struct rse_otp_area_info_t cm_area_info;
extern struct rse_otp_area_info_t cm_rotpk_area_info;
#endif
#ifdef RSE_OTP_HAS_BL1_2
extern struct rse_otp_area_info_t bl1_2_area_info;
#endif
#ifdef RSE_OTP_HAS_DM_AREA
extern struct rse_otp_area_info_t dm_area_info;
extern struct rse_otp_area_info_t dm_rotpk_area_info;
#endif
#ifdef RSE_OTP_HAS_DYNAMIC_AREA
extern struct rse_otp_area_info_t dynamic_area_info;
#endif
#ifdef RSE_OTP_HAS_SOC_AREA
extern const struct rse_otp_area_info_t soc_area_info;
#endif

#define OTP_BASE_S (LCM_BASE_S + 0x1000)

#define OTP_OFFSET(x) (((uintptr_t)&x) - OTP_BASE_S)

#define AREA_PTR(name) \
    ((name ## _area_info.offset != 0 && name ## _area_info.size != 0) ? \
        ((volatile const struct rse_otp_ ## name ## _area_t *)(OTP_BASE_S + name ## _area_info.offset)) \
        : NULL)

#define P_RSE_OTP_HEADER   ((volatile struct rse_otp_header_area_t *)(OTP_BASE_S))
#define P_RSE_OTP_CM       AREA_PTR(cm)
#define P_RSE_OTP_BL1_2    AREA_PTR(bl1_2)
#define P_RSE_OTP_DM       AREA_PTR(dm)
#define P_RSE_OTP_DYNAMIC  AREA_PTR(dynamic)
#define P_RSE_OTP_SOC      AREA_PTR(soc)

#define P_RSE_OTP_CM_ROTPK AREA_PTR(cm_rotpk)
#define P_RSE_OTP_DM_ROTPK AREA_PTR(dm_rotpk)

#ifdef __cplusplus
}
#endif

#endif /* __RSE_OTP_DEV_H__ */
