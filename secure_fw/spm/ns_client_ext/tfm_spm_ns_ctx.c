/*
 * Copyright (c) 2021-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_nspm.h"
#include "tfm_ns_ctx.h"
#include "tfm_ns_client_ext.h"
#include "utilities.h"
#include "tfm_arch.h"
#include "tfm_hal_platform.h"

#define DEFAULT_NS_CLIENT_ID ((int32_t)-1)

struct client_id_region_t {
    int32_t base;
    int32_t limit;
};

static struct client_id_region_t client_id_regions;

void tz_ns_agent_register_client_id_range(int32_t client_id_base,
                                       int32_t client_id_limit)
{
    client_id_regions.base = client_id_base;
    client_id_regions.limit = client_id_limit;
}

static int32_t tz_ns_agent_client_id_translate(int32_t client_id_in, int32_t *client_id_out)
{
    int32_t base, limit;
    int32_t min_client_id;

    if (client_id_out == NULL) {
        return -1;
    }
    if (client_id_in >= 0) {
        return -1;
    }

    base = client_id_regions.base;
    limit = client_id_regions.limit;

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
        return -1;
    }

    /*
     * 1 is added before client_id_in, so that no underflow happens even if
     * limit + client_id_in == INT32_MIN - 1
     */
    *client_id_out = (limit + 1) + client_id_in;
    return 0;
}

int32_t tfm_nspm_get_current_client_id(void)
{
    int32_t input_client_id, translated_client_id;

#ifdef TFM_NS_MANAGE_NSID
    input_client_id = get_nsid_from_active_ns_ctx();
#else
    input_client_id = DEFAULT_NS_CLIENT_ID;
#endif
    if (tz_ns_agent_client_id_translate(input_client_id, &translated_client_id) != 0) {
        return TFM_NS_CLIENT_INVALID_ID;
    }
    return translated_client_id;
}

void tfm_nspm_ctx_init(void)
{
#ifdef TFM_PARTITION_NS_AGENT_TZ
    /* SCB_NS.VTOR points to the Non-secure vector table base address */
    SCB_NS->VTOR = tfm_hal_get_ns_VTOR();

    /* Setups Main stack pointer of the non-secure code */
    __TZ_set_MSP_NS(tfm_hal_get_ns_MSP());
#endif

#ifdef TFM_NS_MANAGE_NSID
    if (!init_ns_ctx()) {
        tfm_core_panic();
    }
#endif
}
