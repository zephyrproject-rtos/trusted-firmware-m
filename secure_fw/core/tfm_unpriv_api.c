/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_svc.h"
#include "tfm_secure_api.h"

uint8_t *tfm_scratch_area;
int32_t tfm_scratch_area_size;

__attribute__((naked)) void tfm_core_service_return_svc(void)
{
    SVC(TFM_SVC_SERVICE_RETURN);
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
