/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "compiler_ext_defs.h"
#include "ffm/psa_api.h"
#include "spm_ipc.h"
#include "svc_num.h"
#include "tfm_psa_call_pack.h"
#include "psa/client.h"
#include "psa/lifecycle.h"
#include "psa/service.h"

#ifdef CONFIG_TFM_PSA_API_THREAD_CALL

__section(".psa_interface_thread_call")
__naked static uint32_t psa_interface_unified_abi(uint32_t r0)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "movs   r2, #1                                      \n"
        "bl     %a0                                         \n"
        "pop    {r0-r4, pc}                                 \n"
        :: "i" (spm_interface_thread_dispatcher)
    );
}

__section(".psa_interface_thread_call")
__naked uint32_t psa_framework_version_thread(void)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_client_psa_framework_version),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked uint32_t psa_version_thread(uint32_t sid)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_client_psa_version),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked psa_handle_t psa_connect_thread(uint32_t sid, uint32_t version)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_client_psa_connect),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked psa_status_t tfm_psa_call_pack_thread(psa_handle_t handle,
                                              uint32_t ctrl_param,
                                              const psa_invec *in_vec,
                                              psa_outvec *out_vec)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_client_psa_call),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_close_thread(psa_handle_t handle)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_client_psa_close),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked psa_signal_t psa_wait_thread(psa_signal_t signal_mask,
                                     uint32_t timeout)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_wait),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked psa_status_t psa_get_thread(psa_signal_t signal, psa_msg_t *msg)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_get),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_set_rhandle_thread(psa_handle_t msg_handle, void *rhandle)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_set_rhandle),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked size_t psa_read_thread(psa_handle_t msg_handle, uint32_t invec_idx,
                               void *buffer, size_t num_bytes)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_read),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked size_t psa_skip_thread(psa_handle_t msg_handle,
                               uint32_t invec_idx, size_t num_bytes)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_skip),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_write_thread(psa_handle_t msg_handle, uint32_t outvec_idx,
                              const void *buffer, size_t num_bytes)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_write),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_reply_thread(psa_handle_t msg_handle, psa_status_t status)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_reply),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_notify_thread(int32_t partition_id)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_notify),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_clear_thread(void)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_clear),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_eoi_thread(psa_signal_t irq_signal)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_eoi),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_panic_thread(void)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_panic),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked psa_irq_status_t psa_irq_disable_thread(psa_signal_t irq_signal)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_irq_disable),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_irq_enable_thread(psa_signal_t irq_signal)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_irq_enable),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked void psa_reset_signal_thread(psa_signal_t irq_signal)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_partition_psa_reset_signal),
           "i" (psa_interface_unified_abi)
    );
}

__section(".psa_interface_thread_call")
__naked uint32_t psa_rot_lifecycle_state_thread(void)
{
    __asm volatile(
#if !defined(__ICCARM__)
        ".syntax unified                                    \n"
#endif
        "push   {r0-r4, lr}                                 \n"
        "ldr    r0, =%a0                                    \n"
        "mov    r1, sp                                      \n"
        "b      %a1                                         \n"
        :: "i" (tfm_spm_get_lifecycle_state),
           "i" (psa_interface_unified_abi)
    );
}

#endif /* CONFIG_TFM_PSA_API_THREAD_CALL */
