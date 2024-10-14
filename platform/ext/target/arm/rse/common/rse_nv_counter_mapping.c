/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "rse_nv_counter_mapping.h"

#include "rse_nv_counter_config.h"
#include "rse_otp_config.h"

#include <assert.h>

enum rse_nv_counter_bank_0_mapping_t {
    BANK_0_COUNTER_BL1 = 0,

    BANK_0_COUNTER_BL2,
    BANK_0_COUNTER_BL2_MAX = BANK_0_COUNTER_BL2 + RSE_NV_COUNTER_BL2_AMOUNT,

    BANK_0_COUNTER_PS = BANK_0_COUNTER_BL2_MAX,
    BANK_0_COUNTER_PS_MAX = BANK_0_COUNTER_PS + RSE_NV_COUNTER_PS_AMOUNT,

    BANK_0_COUNTER_HOST = BANK_0_COUNTER_PS_MAX,
    BANK_0_COUNTER_HOST_MAX = BANK_0_COUNTER_HOST + RSE_NV_COUNTER_HOST_AMOUNT,

    BANK_0_COUNTER_SUBPLATFORM = BANK_0_COUNTER_HOST_MAX,
    BANK_0_COUNTER_SUBPLATFORM_MAX = BANK_0_COUNTER_SUBPLATFORM + RSE_NV_COUNTER_SUBPLATFORM_AMOUNT,

    BANK_0_COUNTER_MAX = BANK_0_COUNTER_SUBPLATFORM_MAX,
};

enum rse_nv_counter_bank_1_mapping_t {
    BANK_1_COUNTER_MAX,
};

enum rse_nv_counter_bank_2_mapping_t {
    BANK_2_COUNTER_MAX,
};

enum rse_nv_counter_bank_3_mapping_t {
    BANK_3_COUNTER_MAX,
};

static inline void counter_mapping_sanity_check(void)
{
    assert(BANK_0_COUNTER_MAX <= RSE_OTP_NV_COUNTERS_BANK_0_AMOUNT);
    assert(BANK_1_COUNTER_MAX <= RSE_OTP_NV_COUNTERS_BANK_1_AMOUNT);
    assert(BANK_2_COUNTER_MAX <= RSE_OTP_NV_COUNTERS_BANK_2_AMOUNT);
    assert(BANK_3_COUNTER_MAX <= RSE_OTP_NV_COUNTERS_BANK_3_AMOUNT);
}

enum tfm_otp_element_id_t rse_get_bl1_counter(uint32_t image_id)
{
    counter_mapping_sanity_check();

    if (image_id == 0) {
        return PLAT_OTP_ID_NV_COUNTER_BANK_0_COUNTER + BANK_0_COUNTER_BL1;
    } else {
        return PLAT_OTP_ID_INVALID;
    }
}

enum tfm_otp_element_id_t rse_get_bl2_counter(uint32_t image_id)
{
    counter_mapping_sanity_check();

    if (image_id < RSE_NV_COUNTER_BL2_AMOUNT) {
        return PLAT_OTP_ID_NV_COUNTER_BANK_0_COUNTER + BANK_0_COUNTER_BL2 + image_id;
    }

    return PLAT_OTP_ID_INVALID;
}

#ifdef TFM_PARTITION_PROTECTED_STORAGE
enum tfm_otp_element_id_t rse_get_ps_counter(uint32_t ps_counter_id)
{
    counter_mapping_sanity_check();

    if (ps_counter_id < RSE_NV_COUNTER_PS_AMOUNT) {
        return PLAT_OTP_ID_NV_COUNTER_BANK_0_COUNTER + BANK_0_COUNTER_PS + ps_counter_id;
    }

    return PLAT_OTP_ID_INVALID;
}
#endif

enum tfm_otp_element_id_t rse_get_host_counter(uint32_t host_counter_id)
{
    counter_mapping_sanity_check();

    if (host_counter_id < RSE_NV_COUNTER_HOST_AMOUNT) {
        return PLAT_OTP_ID_NV_COUNTER_BANK_0_COUNTER + BANK_0_COUNTER_HOST + host_counter_id;
    }

    return PLAT_OTP_ID_INVALID;
}

enum tfm_otp_element_id_t rse_get_subplatform_counter(uint32_t subplatform_counter_id)
{
    counter_mapping_sanity_check();

    if (subplatform_counter_id < RSE_NV_COUNTER_HOST_AMOUNT) {
        return PLAT_OTP_ID_NV_COUNTER_BANK_0_COUNTER + BANK_0_COUNTER_SUBPLATFORM + subplatform_counter_id;
    }

    return PLAT_OTP_ID_INVALID;
}
