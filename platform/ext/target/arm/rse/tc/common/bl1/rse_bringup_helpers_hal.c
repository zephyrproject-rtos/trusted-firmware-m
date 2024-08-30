/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rse_bringup_helpers_hal.h"

uint32_t rse_bringup_helpers_hal_get_pins_offset(void)
{
    /* FIXME set this properly */
    return 0;
}

void *rse_bringup_helpers_hal_get_vm0_exec_address(void)
{
    /* FIXME set this properly */
    return VM0_BASE_S;
}

void *rse_bringup_helpers_hal_get_qpsi_exec_address(void)
{
    /* FIXME set this properly */
    return FLASH_BASE_ADDRESS;
}

void *rse_bringup_helpers_hal_get_side_band_exec_address(void)
{
    /* FIXME set this properly */
    return 0;
}
