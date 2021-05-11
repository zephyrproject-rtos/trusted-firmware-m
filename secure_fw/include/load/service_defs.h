/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SERVICE_DEFS_H__
#define __SERVICE_DEFS_H__

#include <stdint.h>
#include "psa/service.h"

/*
 * Service static data - flags
 * bit 7-0: version policy
 * bit 8: 1 - NS accessible, 0 - NS not accessible
 * bit 9: 1 - stateless, 0 - connection-based
 */
#define SERVICE_FLAG_NS_ACCESSIBLE              (1U << 8)
#define SERVICE_FLAG_STATELESS                  (1U << 9)

#define TFM_VERSION_POLICY_RELAXED              (0x0)
#define TFM_VERSION_POLICY_STRICT               (0x1)
#define SERVICE_VERSION_POLICY_MASK             (0xFF)

#define SERVICE_IS_NS_ACCESSIBLE(flag)          \
    ((flag) & SERVICE_FLAG_NS_ACCESSIBLE)
#define SERVICE_IS_STATELESS(flag)              \
    ((flag) & SERVICE_FLAG_STATELESS)
#define SERVICE_GET_VERSION_POLICY(flag)        \
    ((flag) & SERVICE_VERSION_POLICY_MASK)

/* Common service structure type */
struct service_load_info_t {
    uintptr_t       name_strid;         /* String ID for name               */
    psa_signal_t    signal;             /* Service signal                   */
    uint32_t        sid;                /* Service ID                       */
    uint32_t        flags;              /* Flags                            */
    uint32_t        version;            /* Service version                  */
} __attribute__((aligned(4)));

#endif /* __SERVICE_DEFS_H__ */
