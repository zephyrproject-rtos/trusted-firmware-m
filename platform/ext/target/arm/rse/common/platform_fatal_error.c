/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "fatal_error.h"

#include "device_definition.h"

static bool prevent_recursion = false;

bool log_error_permissions_check(uint32_t err, bool is_fatal)
{
    enum lcm_tp_mode_t tp_mode = LCM_TP_MODE_PCI;
    enum lcm_lcs_t lcs = LCM_LCS_SE;
    enum lcm_bool_t sp_enabled = LCM_TRUE;

    /* Avoid issues where one of the following functions also throws a fatal
     * error.
     */
    if (prevent_recursion) {
        return false;
    }

    prevent_recursion = true;

    if (lcm_get_tp_mode(&LCM_DEV_S, &tp_mode) != LCM_ERROR_NONE) {
        goto out;
    }

    if (lcm_get_lcs(&LCM_DEV_S, &lcs) != LCM_ERROR_NONE) {
        goto out;
    }

    if (lcm_get_sp_enabled(&LCM_DEV_S, &sp_enabled) != LCM_ERROR_NONE) {
        goto out;
    }

    if (tp_mode == LCM_TP_MODE_TCI || (lcs != LCM_LCS_SE && sp_enabled != LCM_TRUE)) {
        prevent_recursion = false;
        return true;
    }
out:
    prevent_recursion = false;

    return false;
}
