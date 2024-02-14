/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __KMU_SLOT_IDS_H__
#define __KMU_SLOT_IDS_H__

#include "kmu_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

enum kmu_slot_id_t {
    KMU_SLOT_CM_PROVISIONING_KEY = KMU_USER_SLOT_MIN,
    _KMU_AEAD_RESERVED_SLOT_CM_PROVISIONING_KEY,
    /* The DM provisioning key is used for AEAD, so requires two contiguous
     * slots. Only the first should be used for calls, the key loader and
     * derivation code will transparently use the second where necessary.
     */
    KMU_SLOT_DM_PROVISIONING_KEY,
    _KMU_AEAD_RESERVED_SLOT_DM_PROVISIONING_KEY,
};

#ifdef __cplusplus
}
#endif

#endif /* __KMU_SLOT_IDS_H__ */
