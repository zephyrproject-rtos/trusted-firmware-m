/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "combined_provisioning.h"
#include "device_definition.h"

/* This is a stub to make the linker happy */
void __Vectors(){}

enum tfm_plat_err_t __attribute__((section("DO_PROVISION"))) do_provision(void) {
    enum lcm_error_t lcm_err;
    enum lcm_lcs_t lcs;

    lcm_err = lcm_get_lcs(&LCM_DEV_S, &lcs);
    if (lcm_err != LCM_ERROR_NONE) {
        return (enum tfm_plat_err_t)lcm_err;
    }

    switch(lcs) {
    case LCM_LCS_CM:
        return do_cm_provision();
    case LCM_LCS_DM:
        return do_dm_provision();
    default:
        return TFM_PLAT_ERR_PROVISIONING_BUNDLE_COMBINED_INVALID_LCS;
    }
}
