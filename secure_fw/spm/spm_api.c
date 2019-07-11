/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* All the APIs defined in this file are common for library and IPC model. */

#include <stdio.h>
#include <string.h>
#include "spm_api.h"
#include "tfm_spm_hal.h"
#include "tfm_memory_utils.h"
#include "spm_db.h"
#include "tfm_internal.h"
#include "tfm_api.h"
#include "tfm_nspm.h"
#include "secure_fw/core/tfm_core.h"
#include "tfm_peripherals_def.h"
#include "spm_partition_defs.h"

#define NON_SECURE_INTERNAL_PARTITION_DB_IDX 0
#define TFM_CORE_INTERNAL_PARTITION_DB_IDX   1

/* Define SPM DB structure */
#include "secure_fw/services/tfm_spm_db.inc"

uint32_t get_partition_idx(uint32_t partition_id)
{
    uint32_t i;

    if (partition_id == INVALID_PARTITION_ID) {
        return SPM_INVALID_PARTITION_IDX;
    }

    for (i = 0; i < g_spm_partition_db.partition_count; ++i) {
        if (g_spm_partition_db.partitions[i].static_data.partition_id ==
                partition_id) {
            return i;
        }
    }
    return SPM_INVALID_PARTITION_IDX;
}

enum spm_err_t tfm_spm_db_init(void)
{
    struct spm_partition_desc_t *part_ptr;
#ifndef TFM_PSA_API
    static uint32_t ns_interrupt_ctx_stack[
           sizeof(struct interrupted_ctx_stack_frame_t)/sizeof(uint32_t)] = {0};
    static uint32_t tfm_core_interrupt_ctx_stack[
           sizeof(struct interrupted_ctx_stack_frame_t)/sizeof(uint32_t)] = {0};
#endif /* !defined(TFM_PSA_API) */

    /* This function initialises partition db */

    /* There are a few partitions that are used by TF-M internally.
     * These are explicitly added to the partition db here.
     */

    /* For the non secure Execution environment */
#if TFM_PSA_API
    extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Base[];
    extern uint32_t Image$$ARM_LIB_STACK$$ZI$$Limit[];
    uint32_t psp_stack_bottom = (uint32_t)Image$$ARM_LIB_STACK$$ZI$$Base;
    uint32_t psp_stack_top    = (uint32_t)Image$$ARM_LIB_STACK$$ZI$$Limit;
#endif

    part_ptr = &(g_spm_partition_db.partitions[
                                         NON_SECURE_INTERNAL_PARTITION_DB_IDX]);
    part_ptr->static_data.partition_id = TFM_SP_NON_SECURE_ID;
#ifdef TFM_PSA_API
    part_ptr->static_data.partition_flags = SPM_PART_FLAG_APP_ROT |
                                            SPM_PART_FLAG_IPC;
    part_ptr->static_data.partition_priority = TFM_PRIORITY_LOW;
    part_ptr->static_data.partition_init = tfm_nspm_thread_entry;
#else
    part_ptr->static_data.partition_flags = 0;
#endif

#if TFM_PSA_API
    part_ptr->memory_data.stack_bottom = psp_stack_bottom;
    part_ptr->memory_data.stack_top    = psp_stack_top;
    /* Since RW, ZI and stack are configured as one MPU region, configure
     * RW start address to psp_stack_bottom to get RW access to stack
     */
    part_ptr->memory_data.rw_start     = psp_stack_bottom;
#endif

#ifndef TFM_PSA_API
    part_ptr->runtime_data.partition_state = SPM_PARTITION_STATE_UNINIT;
    part_ptr->runtime_data.ctx_stack_ptr = ns_interrupt_ctx_stack;
#endif /* !defined(TFM_PSA_API) */

    tfm_nspm_configure_clients();

#ifndef TFM_PSA_API
    /* For the TF-M core environment itself */
    part_ptr = &(g_spm_partition_db.partitions[
                                           TFM_CORE_INTERNAL_PARTITION_DB_IDX]);
    part_ptr->static_data.partition_id = TFM_SP_CORE_ID;
    part_ptr->static_data.partition_flags =
                    SPM_PART_FLAG_APP_ROT | SPM_PART_FLAG_PSA_ROT;
    part_ptr->runtime_data.partition_state = SPM_PARTITION_STATE_UNINIT;
    part_ptr->runtime_data.ctx_stack_ptr = tfm_core_interrupt_ctx_stack;
#endif /* !defined(TFM_PSA_API) */

    g_spm_partition_db.is_init = 1;

    return SPM_ERR_OK;
}

uint32_t tfm_spm_partition_get_partition_id(uint32_t partition_idx)
{
    return g_spm_partition_db.partitions[partition_idx].static_data.
            partition_id;
}

uint32_t tfm_spm_partition_get_flags(uint32_t partition_idx)
{
    return g_spm_partition_db.partitions[partition_idx].static_data.
            partition_flags;
}

__attribute__((section("SFN")))
void tfm_spm_partition_change_privilege(uint32_t privileged)
{
    CONTROL_Type ctrl;

    ctrl.w = __get_CONTROL();

    if (privileged == TFM_PARTITION_PRIVILEGED_MODE) {
        ctrl.b.nPRIV = 0;
    } else {
        ctrl.b.nPRIV = 1;
    }

    __set_CONTROL(ctrl.w);
}
