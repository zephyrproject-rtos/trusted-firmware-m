/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_plat_otp.h"

#ifndef __RSE_NV_COUNTER_MAPPING_H__
#define __RSE_NV_COUNTER_MAPPING_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum tfm_otp_element_id_t rse_get_bl1_counter(uint32_t image_id);
enum tfm_otp_element_id_t rse_get_bl2_counter(uint32_t image_id);
enum tfm_otp_element_id_t rse_get_ps_counter(uint32_t image_id);
enum tfm_otp_element_id_t rse_get_host_counter(uint32_t host_counter_id);
enum tfm_otp_element_id_t rse_get_subplatform_counter(uint32_t host_counter_id);

#ifdef __cplusplus
}
#endif

#endif /* __RSE_NV_COUNTER_MAPPING_H__ */
