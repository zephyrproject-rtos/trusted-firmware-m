/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __ASSET_DEFS_H__
#define __ASSET_DEFS_H__

#include <stdint.h>

/* Memory-based object attributes */
#define ASSET_MEM_RD_BIT            (1U << 0)  /* 1: readable memory address */
#define ASSET_MEM_WR_BIT            (1U << 1)  /* 1: writable memory address */
#define ASSET_MEM_PPB_BIT           (1U << 2)  /* 1: PPB indicator           */

/* Customized objects such as device or symbol. Refer by 'dev' in the union. */
#define ASSET_DEV_REF_BIT           (1U << 3)

#define PTR_TO_REFERENCE(x)         (uintptr_t)(x)
#define REFERENCE_TO_PTR(x, t)      (t)(x)

struct asset_desc_t {
    union {
        struct {                            /* Memory asset type        */
            uintptr_t addr_x;
            uintptr_t addr_y;
        } mem;
        struct {                            /* Device asset type        */
            uintptr_t addr_ref;
            uintptr_t reserved;
        } dev;
    };
    uint32_t attr;                          /* Asset attributes         */
} __attribute__((aligned(4)));

#endif /* __ASSET_DEFS_H__ */
