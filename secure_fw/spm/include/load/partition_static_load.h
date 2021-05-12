/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PARTITION_STATIC_LOAD_H__
#define __PARTITION_STATIC_LOAD_H__

#include "partition_defs.h"
#include "service_defs.h"

#define PARTITION_GET_PRIOR(flag)           ((flag) & PARTITION_PRI_MASK)
#define TO_THREAD_PRIORITY(x)               (x)

#define STRID_TO_STRING_PTR(strid)          (const char *)(strid)
#define STRING_PTR_TO_STRID(str)            (uintptr_t)(str)

#define ENTRY_TO_POSITION(x)                (uintptr_t)(x)
#define POSITION_TO_ENTRY(x, t)             (t)(x)

#define PTR_TO_POSITION(x)                  (uintptr_t)(x)
#define POSITION_TO_PTR(x, t)               (t)(x)

/* Length of extendable variables in partition static type */
#define LOAD_INFO_EXT_LENGTH                2
/*
 * Argument "ps_ptr" must have be a "struct partition_load_info_t *" type and
 * must be validated before using.
 */
#define LOAD_INFSZ_BYTES(ps_ptr)                                       \
    (sizeof(*(ps_ptr)) + LOAD_INFO_EXT_LENGTH * sizeof(uintptr_t) +    \
     (ps_ptr)->ndeps * sizeof(uint32_t) +                              \
     (ps_ptr)->nservices * sizeof(struct service_load_info_t) +        \
     (ps_ptr)->nassets * sizeof(struct asset_desc_t))

/* 'Allocate' stack based on load info */
#define LOAD_ALLOCED_STACK_ADDR(ps_ptr)    (*((uintptr_t *)(ps_ptr + 1)))

#define LOAD_INFO_DEPS(ps_ptr)                                         \
    ((uintptr_t)(ps_ptr + 1) + LOAD_INFO_EXT_LENGTH * sizeof(uintptr_t))
#define LOAD_INFO_SERVICE(ps_ptr)                                      \
    ((uintptr_t)LOAD_INFO_DEPS(ps_ptr) + (ps_ptr)->ndeps * sizeof(uint32_t))
#define LOAD_INFO_ASSET(ps_ptr)                                        \
    ((uintptr_t)LOAD_INFO_SERVICE(ps_ptr) +                            \
     (ps_ptr)->nservices * sizeof(struct service_load_info_t))

#endif /* __PARTITION_STATIC_LOAD_H__ */
