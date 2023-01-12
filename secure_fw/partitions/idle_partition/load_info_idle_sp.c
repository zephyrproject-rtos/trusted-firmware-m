/*
 * Copyright (c) 2021-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <stddef.h>
#include "spm_ipc.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/asset_defs.h"

#if TFM_LVL == 3
#define TFM_SP_IDLE_NASSETS     (1)
#endif

#define IDLE_SP_STACK_SIZE      (0x100)

struct partition_tfm_sp_idle_load_info_t {
    /* common length load data */
    struct partition_load_info_t    load_info;
    /* per-partition variable length load data */
    uintptr_t                       stack_addr;
    uintptr_t                       heap_addr;
#if TFM_LVL == 3
    struct asset_desc_t             assets[TFM_SP_IDLE_NASSETS];
#endif
} __attribute__((aligned(4)));

/* Entrypoint function declaration */
extern void tfm_idle_thread(void);
/* Stack */
uint8_t idle_sp_stack[IDLE_SP_STACK_SIZE] __attribute__((aligned(8)));

/* Partition load, deps, service load data. Put to a dedicated section. */
#if defined(__ICCARM__)
#pragma location = ".part_load_priority_lowest"
__root
#endif
const struct partition_tfm_sp_idle_load_info_t
    tfm_sp_idle_load __attribute__((used, section(".part_load_priority_lowest"))) = {
    .load_info = {
        .psa_ff_ver                 = 0x0101 | PARTITION_INFO_MAGIC,
        .pid                        = TFM_SP_IDLE_ID,
        .flags                      = PARTITION_PRI_LOWEST | PARTITION_MODEL_IPC
                                      | PARTITION_MODEL_PSA_ROT,
        .entry                      = ENTRY_TO_POSITION(tfm_idle_thread),
        .stack_size                 = IDLE_SP_STACK_SIZE,
        .heap_size                  = 0,
        .ndeps                      = 0,
        .nservices                  = 0,
#if TFM_LVL == 3
        .nassets                    = TFM_SP_IDLE_NASSETS,
#else
        .nassets                    = 0,
#endif
    },
    .stack_addr                     = (uintptr_t)idle_sp_stack,
    .heap_addr                      = 0,
#if TFM_LVL == 3
    .assets                         = {
        {
            .mem.start              = (uintptr_t)idle_sp_stack,

            .mem.limit              = (uintptr_t)&idle_sp_stack[IDLE_SP_STACK_SIZE],
            .attr                   = ASSET_ATTR_READ_WRITE,
        },
    },
#endif
};

/* Placeholder for partition runtime space. Do not reference it. */
#if defined(__ICCARM__)
#pragma location = ".bss.part_runtime_priority_lowest"
__root
#endif
static struct partition_t tfm_idle_partition_runtime_item
    __attribute__((used, section(".bss.part_runtime_priority_lowest")));
