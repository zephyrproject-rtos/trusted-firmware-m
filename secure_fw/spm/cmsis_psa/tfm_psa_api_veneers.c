/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include "security_defs.h"
#include "tfm_arch.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "tfm_svcalls.h"
#include "utilities.h"

/*
 * Use assembly to:
 * - Explicit stack usage to perform re-entrant detection.
 * - SVC here to take hardware context management advantages.
 */

__tfm_psa_secure_gateway_attributes__
uint32_t tfm_psa_framework_version_veneer(void)
{
    __ASM volatile(
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "   ldr    r2, [sp]                        \n"
            "   ldr    r3, ="M2S(STACK_SEAL_PATTERN)"  \n"
            "   cmp    r2, r3                          \n"
            "   bne    reent_panic1                    \n"
#endif
            "   svc    %0                              \n"
            "   bxns   lr                              \n"
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "reent_panic1:                             \n"
            "   svc    %1                              \n"
            "   b      .                               \n"
#endif
            : : "I" (TFM_SVC_PSA_FRAMEWORK_VERSION),
                "I" (TFM_SVC_PSA_PANIC));
}

__tfm_psa_secure_gateway_attributes__
uint32_t tfm_psa_version_veneer(uint32_t sid)
{
    __ASM volatile(
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "   ldr    r2, [sp]                        \n"
            "   ldr    r3, ="M2S(STACK_SEAL_PATTERN)"  \n"
            "   cmp    r2, r3                          \n"
            "   bne    reent_panic2                    \n"
#endif
            "   svc    %0                              \n"
            "   bxns   lr                              \n"
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "reent_panic2:                             \n"
            "   svc    %1                              \n"
            "   b      .                               \n"
#endif
            : : "I" (TFM_SVC_PSA_VERSION),
                "I" (TFM_SVC_PSA_PANIC));
}

__tfm_psa_secure_gateway_attributes__
psa_handle_t tfm_psa_connect_veneer(uint32_t sid, uint32_t version)
{
    __ASM volatile(
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "   ldr    r2, [sp]                        \n"
            "   ldr    r3, ="M2S(STACK_SEAL_PATTERN)"  \n"
            "   cmp    r2, r3                          \n"
            "   bne    reent_panic3                    \n"
#endif
            "   svc    %0                              \n"
            "   bxns   lr                              \n"
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "reent_panic3:                             \n"
            "   svc    %1                              \n"
            "   b      .                               \n"
#endif
            : : "I" (TFM_SVC_PSA_CONNECT),
                "I" (TFM_SVC_PSA_PANIC));
}

__tfm_psa_secure_gateway_attributes__
psa_status_t tfm_psa_call_veneer(psa_handle_t handle,
                                 uint32_t ctrl_param,
                                 const psa_invec *in_vec,
                                 psa_outvec *out_vec)
{
    __ASM volatile(
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "   push   {r2, r3}                        \n"
            "   ldr    r2, [sp, #8]                    \n"
            "   ldr    r3, ="M2S(STACK_SEAL_PATTERN)"  \n"
            "   cmp    r2, r3                          \n"
            "   bne    reent_panic4                    \n"
            "   pop    {r2, r3}                        \n"
#endif
            "   svc    %0                              \n"
            "   bxns   lr                              \n"
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "reent_panic4:                             \n"
            "   svc    %1                              \n"
            "   b      .                               \n"
#endif
            : : "I" (TFM_SVC_PSA_CALL),
                "I" (TFM_SVC_PSA_PANIC));
}

__tfm_psa_secure_gateway_attributes__
void tfm_psa_close_veneer(psa_handle_t handle)
{
    __ASM volatile(
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "   ldr    r2, [sp]                        \n"
            "   ldr    r3, ="M2S(STACK_SEAL_PATTERN)"  \n"
            "   cmp    r2, r3                          \n"
            "   bne    reent_panic5                    \n"
#endif
            "   svc    %0                              \n"
            "   bxns   lr                              \n"
#if !defined(__ARM_ARCH_8_1M_MAIN__)
            "reent_panic5:                             \n"
            "   svc    %1                              \n"
            "   b      .                               \n"
#endif
            : : "I" (TFM_SVC_PSA_CLOSE),
                "I" (TFM_SVC_PSA_PANIC));
}
