/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "compiler_ext_defs.h"
#include "svc_num.h"
#include "psa/client.h"
#include "psa/lifecycle.h"
#include "psa/service.h"

#ifdef CONFIG_TFM_PSA_API_SUPERVISOR_CALL

__naked uint32_t psa_framework_version_svc(void)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_FRAMEWORK_VERSION));
}

__naked uint32_t psa_version_svc(uint32_t sid)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_VERSION));
}

__naked psa_handle_t psa_connect_svc(uint32_t sid, uint32_t version)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_CONNECT));
}

__naked psa_status_t tfm_psa_call_pack_svc(psa_handle_t handle,
                                           uint32_t ctrl_param,
                                           const psa_invec *in_vec,
                                           psa_outvec *out_vec)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_CALL));
}

__naked void psa_close_svc(psa_handle_t handle)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_CLOSE));
}

__naked psa_signal_t psa_wait_svc(psa_signal_t signal_mask, uint32_t timeout)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_WAIT));
}

__naked psa_status_t psa_get_svc(psa_signal_t signal, psa_msg_t *msg)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_GET));
}

__naked void psa_set_rhandle_svc(psa_handle_t msg_handle, void *rhandle)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_SET_RHANDLE));
}

__naked size_t psa_read_svc(psa_handle_t msg_handle, uint32_t invec_idx,
                            void *buffer, size_t num_bytes)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_READ));
}

__naked size_t psa_skip_svc(psa_handle_t msg_handle,
                            uint32_t invec_idx, size_t num_bytes)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_SKIP));
}

__naked void psa_write_svc(psa_handle_t msg_handle, uint32_t outvec_idx,
                           const void *buffer, size_t num_bytes)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_WRITE));
}

__naked void psa_reply_svc(psa_handle_t msg_handle, psa_status_t retval)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_REPLY));
}

__naked void psa_notify_svc(int32_t partition_id)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_NOTIFY));
}

__naked void psa_clear_svc(void)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_CLEAR));
}

__naked void psa_eoi_svc(psa_signal_t irq_signal)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_EOI));
}

__naked void psa_panic_svc(void)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_PANIC));
}

__naked void psa_irq_enable_svc(psa_signal_t irq_signal)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_IRQ_ENABLE));
}

__naked psa_irq_status_t psa_irq_disable_svc(psa_signal_t irq_signal)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_IRQ_DISABLE));
}

__naked void psa_reset_signal_svc(psa_signal_t irq_signal)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_RESET_SIGNAL));
}

__naked uint32_t psa_rot_lifecycle_state_svc(void)
{
    __asm volatile("svc     %0                              \n"
                   "bx      lr                              \n"
                   : : "I" (TFM_SVC_PSA_LIFECYCLE));
}

#endif /* CONFIG_TFM_PSA_API_SUPERVISOR_CALL */
