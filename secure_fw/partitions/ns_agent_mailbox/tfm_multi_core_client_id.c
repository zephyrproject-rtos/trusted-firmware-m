/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "array.h"
#include "internal_status_code.h"
#include "ns_mailbox_client_id.h"
#include "tfm_plat_defs.h"
#include "tfm_multi_core.h"

int32_t tfm_multi_core_register_client_id_range(void *owner,
                                                uint32_t irq_source)
{
    size_t i;

    if (owner == NULL) {
        return SPM_ERROR_BAD_PARAMETERS;
    }

    for (i = 0; i < ARRAY_SIZE(ns_mailbox_client_id_info); i++) {
        if (ns_mailbox_client_id_info[i].source == irq_source) {
            if (ns_mailbox_client_id_range_owner[i] != NULL) {
                return SPM_ERROR_GENERIC;
            }

            ns_mailbox_client_id_range_owner[i] = owner;
            return SPM_SUCCESS;
        }
    }

    return SPM_ERROR_GENERIC;
}

/* Map a negative client ID in the range of the owner's defined client ID. */
int32_t tfm_multi_core_hal_client_id_translate(void *owner,
                                               int32_t client_id_in,
                                               int32_t *client_id_out)
{
    size_t i;
    int32_t base, limit;
    int32_t min_client_id;
    const size_t nr_ranges = ARRAY_SIZE(ns_mailbox_client_id_range_owner);

    if ((owner == NULL) || (client_id_out == NULL)) {
        return SPM_ERROR_BAD_PARAMETERS;
    }
    if (client_id_in >= 0) {
        return SPM_ERROR_BAD_PARAMETERS;
    }

    for (i = 0; i < nr_ranges; i++) {
        if (ns_mailbox_client_id_range_owner[i] == owner) {
            base = ns_mailbox_client_id_info[i].client_id_base;
            limit = ns_mailbox_client_id_info[i].client_id_limit;
            break;
        }
    }
    if (i == nr_ranges) {
        return SPM_ERROR_GENERIC;
    }

    /*
     * client_id_range = limit - base + 1
     * min_client_id = -client_id_range
     *
     * However client_id_range cannot be calculated directly, because if
     * base = INT32_MIN and limit = -1 then
     * client_id_range becomes INT32_MAX + 1 because
     * abs(INT32_MIN) = abs(INT32_MAX) + 1 and that causes overflow. So
     * min_client_id is calculated directly, with a reordered expression:
     * min_client_id = -(limit - base + 1) ==
     *                 -limit + base -1 ==
     *                 -(limit - base) -1
     */
    min_client_id = -1 * (limit - base) - 1;

    if (client_id_in < min_client_id) {
        return SPM_ERROR_GENERIC;
    }

    /*
     * 1 is added before client_id_in, so that no underflow happens even if
     * limit + client_id_in == INT32_MIN - 1
     */
    *client_id_out = (limit + 1) + client_id_in;
    return SPM_SUCCESS;
}
