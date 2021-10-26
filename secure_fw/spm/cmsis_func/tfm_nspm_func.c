/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_spm_hal.h"
#include "tfm_arch.h"
#include "arch.h"

void configure_ns_code(void)
{
    /* SCB_NS.VTOR points to the Non-secure vector table base address */
    SCB_NS->VTOR = tfm_spm_hal_get_ns_VTOR();

    /* Setups Main stack pointer of the non-secure code */
    uint32_t ns_msp = tfm_spm_hal_get_ns_MSP();

    __TZ_set_MSP_NS(ns_msp);

    /* Get the address of non-secure code entry point to jump there */
    uint32_t entry_ptr = tfm_spm_hal_get_ns_entry_point();

    /* Clears LSB of the function address to indicate the function-call
     * will perform the switch from secure to non-secure
     */
    ns_entry = (nsfptr_t)cmse_nsfptr_create(entry_ptr);
}
