/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_secure_api.h"

#if defined(__ARM_ARCH_8_1M_MAIN__) || defined(__ARM_ARCH_8M_MAIN__)
__attribute__((section("SFN"), naked))
int32_t tfm_core_sfn_request(const struct tfm_sfn_req_s *desc_ptr)
{
    __ASM volatile(
        "PUSH   {r4-r12, lr}                \n"
        "SVC    %[SVC_REQ]                  \n"
        "MOV    r4,  #0                     \n"
        "MOV    r5,  r4                     \n"
        "MOV    r6,  r4                     \n"
        "MOV    r7,  r4                     \n"
        "MOV    r8,  r4                     \n"
        "MOV    r9,  r4                     \n"
        "MOV    r10, r4                     \n"
        "MOV    r11, r4                     \n"
        "BLX    lr                          \n"
        "SVC    %[SVC_RET]                  \n"
        "POP    {r4-r12, pc}                \n"
        : : [SVC_REQ] "I" (TFM_SVC_SFN_REQUEST),
            [SVC_RET] "I" (TFM_SVC_SFN_RETURN)
        );
}

__attribute__((section("SFN"), naked))
void priv_irq_handler_main(uint32_t partition_id, uint32_t unpriv_handler,
                           uint32_t irq_signal, uint32_t irq_line)
{
    __ASM(
          /* Save the callee saved registers*/
          "PUSH   {r4-r12, lr}              \n"
          /* Request SVC to configure environment for the unpriv IRQ handler */
          "SVC    %[SVC_REQ]                \n"
          /* clear the callee saved registers to prevent information leak */
          "MOV    r4,  #0                   \n"
          "MOV    r5,  r4                   \n"
          "MOV    r6,  r4                   \n"
          "MOV    r7,  r4                   \n"
          "MOV    r8,  r4                   \n"
          "MOV    r9,  r4                   \n"
          "MOV    r10, r4                   \n"
          "MOV    r11, r4                   \n"
          /* Branch to the unprivileged handler */
          "BLX    lr                        \n"
          /* Request SVC to reconfigure the environment of the interrupted
           * partition
           */
          "SVC    %[SVC_RET]                \n"
            /* restore callee saved registers and return */
          "POP    {r4-r12, pc}              \n"
          : : [SVC_REQ] "I" (TFM_SVC_DEPRIV_REQ)
            , [SVC_RET] "I" (TFM_SVC_DEPRIV_RET)
          );
}
#elif defined(__ARM_ARCH_8M_BASE__)
__attribute__((section("SFN"), naked))
int32_t tfm_core_sfn_request(const struct tfm_sfn_req_s *desc_ptr)
{
    __ASM volatile(
        "PUSH   {lr}                        \n"
        "PUSH   {r4-r7}                     \n"
        "MOV    r4, r8                      \n"
        "MOV    r5, r9                      \n"
        "MOV    r6, r10                     \n"
        "MOV    r7, r11                     \n"
        "PUSH   {r4-r7}                     \n"
        "MOV    r4, r12                     \n"
        "PUSH   {r4}                        \n"
        "SVC    %[SVC_REQ]                  \n"
        "MOVS   r4, #0                      \n"
        "MOV    r5, r4                      \n"
        "MOV    r6, r4                      \n"
        "MOV    r7, r4                      \n"
        "MOV    r8, r4                      \n"
        "MOV    r9, r4                      \n"
        "MOV    r10, r4                     \n"
        "MOV    r11, r4                     \n"
        "BLX    lr                          \n"
        "SVC    %[SVC_RET]                  \n"
        "POP    {r4}                        \n"
        "MOV    r12, r4                     \n"
        "POP    {r4-r7}                     \n"
        "MOV    r8, r4                      \n"
        "MOV    r9, r5                      \n"
        "MOV    r10, r6                     \n"
        "MOV    r11, r7                     \n"
        "POP    {r4-r7}                     \n"
        "POP    {pc}                        \n"
        : : [SVC_REQ] "I" (TFM_SVC_SFN_REQUEST),
            [SVC_RET] "I" (TFM_SVC_SFN_RETURN)
        );
}

__attribute__((section("SFN"), naked))
void priv_irq_handler_main(uint32_t partition_id, uint32_t unpriv_handler,
                           uint32_t irq_signal, uint32_t irq_line)
{
    __ASM(
          /* Save the callee saved registers*/
          "PUSH   {r4-r7, lr}               \n"
          "MOV    r4, r8                    \n"
          "MOV    r5, r9                    \n"
          "MOV    r6, r10                   \n"
          "MOV    r7, r11                   \n"
          "PUSH   {r4-r7}                   \n"
          "MOV    r4, r12                   \n"
          "PUSH   {r4}                      \n"
          /* Request SVC to configure environment for the unpriv IRQ handler */
          "SVC    %[SVC_REQ]                \n"
          /* clear the callee saved registers to prevent information leak */
          "MOVS   r4, #0                    \n"
          "MOV    r5, r4                    \n"
          "MOV    r6, r4                    \n"
          "MOV    r7, r4                    \n"
          "MOV    r8, r4                    \n"
          "MOV    r9, r4                    \n"
          "MOV    r10, r4                   \n"
          "MOV    r11, r4                   \n"
          /* Branch to the unprivileged handler */
          "BLX    lr                        \n"
          /* Request SVC to reconfigure the environment of the interrupted
           * partition
           */
          "SVC    %[SVC_RET]                \n"
          /* restore callee saved registers and return */
          "POP    {r4}                      \n"
          "MOV    r12, r4                   \n"
          "POP    {r4-r7}                   \n"
          "MOV    r8, r4                    \n"
          "MOV    r9, r5                    \n"
          "MOV    r10, r6                   \n"
          "MOV    r11, r7                   \n"
          "POP   {r4-r7, pc}                \n"
          : : [SVC_REQ] "I" (TFM_SVC_DEPRIV_REQ)
          , [SVC_RET] "I" (TFM_SVC_DEPRIV_RET)
          );
}
#endif
