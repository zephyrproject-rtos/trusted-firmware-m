/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <arm_cmse.h>

#include "tfm_svc.h"
#include "tfm_secure_api.h"
#include "tfm_internal.h"

uint8_t *tfm_scratch_area;
int32_t tfm_scratch_area_size;
nsfptr_t ns_entry;

void jump_to_ns_code(void)
{
#if TFM_LVL != 1
    /* Initialization is done, set thread mode to unprivileged. */
    CONTROL_Type ctrl;

    ctrl.w = __get_CONTROL();
    ctrl.b.nPRIV = 1;
    __set_CONTROL(ctrl.w);
#endif
    /* All changes made to memory will be effective after this point */
    __DSB();
    __ISB();

    /* Calls the non-secure Reset_Handler to jump to the non-secure binary */
    ns_entry();
}

__attribute__((naked)) void tfm_core_partition_return_svc(void)
{
    SVC(TFM_SVC_SFN_RETURN);
}

__attribute__((naked))
int32_t tfm_core_memory_permission_check(
            void *ptr, uint32_t len, int32_t access)
{
    __ASM(
        "PUSH   {r7, lr}\n"
        "SVC %0\n"
        "POP    {r7, pc}\n"
        : : "I" (TFM_SVC_MEMORY_CHECK));
}

__attribute__((naked))
int32_t tfm_core_validate_secure_caller(void)
{
    __ASM(
        "PUSH   {r7, lr}\n"
        "SVC %0\n"
        "POP    {r7, pc}\n"
        : : "I" (TFM_SVC_VALIDATE_SECURE_CALLER));
}

__attribute__((naked))
int32_t tfm_core_set_buffer_area(enum tfm_buffer_share_region_e share)
{
    __ASM(
        "PUSH   {r7, lr}\n"
        "SVC %0\n"
        "POP    {r7, pc}\n"
        : : "I" (TFM_SVC_SET_SHARE_AREA));
}
