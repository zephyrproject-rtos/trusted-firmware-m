/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/***** WARNING: This file SHOULD BE CHANGED according to storage template *****/

#include <stdint.h>
#include <stddef.h>
#include "region.h"
#include "region_defs.h"
#include "spm_partition_defs.h"
#include "load/partition_static_load.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/asset_defs.h"
#include "psa_manifest/pid.h"
#include "psa_manifest/sid.h"

#define TFM_SP_NS_PROXY_NDEPS                                   (0)
#define TFM_SP_NS_PROXY_NSERVS                                  (0)
#if TFM_LVL == 3
#define TFM_SP_NS_PROXY_NASSETS                                 (1)
#endif

/* Memory region declaration */
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Limit);

/* Entrypoint function declaration */
extern void tfm_nspm_thread_entry(void);

struct partition_tfm_sp_ns_proxy_load_info_t {
    /* common length data */
    struct partition_load_info_t  cmn_info;
    /* per-partition variable length data */
    uintptr_t                       stack_pos;
    uintptr_t                       heap_pos;
#if TFM_LVL == 3
    struct asset_desc_t             assets[TFM_SP_NS_PROXY_NASSETS];
#endif
} __attribute__((aligned(4)));

/* Partition static, deps, service static data. Put to a dedicated section. */
const struct partition_tfm_sp_ns_proxy_load_info_t
    tfm_sp_ns_proxy_static __attribute__((used, section(".partition_info"))) = {
    .cmn_info = {
        .psa_ff_ver                 = 0x0100 | PARTITION_INFO_MAGIC,
        .pid                        = TFM_SP_NON_SECURE_ID,
        .flags                      = PARTITION_PRI_LOWEST | SPM_PART_FLAG_IPC
#if TFM_MULTI_CORE_TOPOLOGY
                                    | SPM_PART_FLAG_PSA_ROT
#endif
                                    ,
        .entry                      = ENTRY_TO_POSITION(tfm_nspm_thread_entry),
        .stack_size                 = S_PSP_STACK_SIZE,
        .heap_size                  = 0,
        .ndeps                      = TFM_SP_NS_PROXY_NDEPS,
        .nservices                  = TFM_SP_NS_PROXY_NSERVS,
#if TFM_LVL == 3
        .nassets                    = TFM_SP_NS_PROXY_NASSETS,
#endif
    },
    .stack_pos                      = PART_REGION_ADDR(ARM_LIB_STACK, $$ZI$$Base),
    .heap_pos                       = 0,
#if TFM_LVL == 3
    .assets                         = {
        {
            .mem.addr_x             = PART_REGION_ADDR(ARM_LIB_STACK, $$ZI$$Base),
            .mem.addr_y             = PART_REGION_ADDR(ARM_LIB_STACK, $$ZI$$Limit),
            .attr                   = ASSET_MEM_RD_BIT | ASSET_MEM_WR_BIT,
        },
    },
#endif
};
