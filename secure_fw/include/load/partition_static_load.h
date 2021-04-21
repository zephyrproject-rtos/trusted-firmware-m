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
#define STATIC_INFO_EXT_LENGTH              2
/*
 * Argument "ps_ptr" must have be a "struct partition_static_info_t *" type and
 * must be validated before using.
 */
#define STATIC_INFSZ_BYTES(ps_ptr)                                      \
    (sizeof(*(ps_ptr)) + STATIC_INFO_EXT_LENGTH * sizeof(uintptr_t) +   \
     (ps_ptr)->ndeps * sizeof(uint32_t) +                               \
     (ps_ptr)->nservices * sizeof(struct service_static_info_t))

#define STATIC_INF_DEPS(ps_ptr)                                         \
    ((uintptr_t)(ps_ptr)->vars + STATIC_INFO_EXT_LENGTH * sizeof(uintptr_t))
#define STATIC_INF_SERVICE(ps_ptr)                                      \
    ((uintptr_t)STATIC_INF_DEPS(ps_ptr) + (ps_ptr)->ndeps * sizeof(uint32_t))

#endif /* __PARTITION_STATIC_LOAD_H__ */
