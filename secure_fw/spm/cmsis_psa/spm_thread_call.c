/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "config_impl.h"
#include "compiler_ext_defs.h"
#include "spm_ipc.h"
#include "tfm_arch.h"
#include "utilities.h"
#include "ffm/backend.h"
#include "psa/client.h"
#include "psa/lifecycle.h"
#include "psa/service.h"

#ifdef CONFIG_TFM_PSA_API_THREAD_CALL

/* Customized ABI format */
struct spcall_abi_frame_t {
    uint32_t      a0;
    uint32_t      a1;
    uint32_t      a2;
    uint32_t      a3;
    uint32_t      unused0;
    uint32_t      unused1;
};

typedef uint32_t (*target_fn_t)(uint32_t a0, uint32_t a1,
                                uint32_t a2, uint32_t a3);

__used
void spcall_execute_c(uintptr_t fn_addr, uintptr_t frame_addr)
{
    struct spcall_abi_frame_t *p_frame =
                                  (struct spcall_abi_frame_t *)frame_addr;

    p_frame->a0 = ((target_fn_t)fn_addr)(p_frame->a0, p_frame->a1,
                                         p_frame->a2, p_frame->a3);
}

__used
void spm_interface_thread_dispatcher(uintptr_t fn_addr,
                                     uintptr_t frame_addr,
                                     uint32_t  switch_stack)
{
    arch_non_preempt_call(fn_addr, frame_addr,
                          switch_stack ? SPM_THREAD_CONTEXT->sp : 0,
                          switch_stack ? SPM_THREAD_CONTEXT->sp_limit : 0);

    if (THRD_EXPECTING_SCHEDULE()) {
        ((struct spcall_abi_frame_t *)frame_addr)->a0 =
                                        tfm_arch_trigger_pendsv();
    }
}

#endif /* CONFIG_TFM_PSA_API_THREAD_CALL */
