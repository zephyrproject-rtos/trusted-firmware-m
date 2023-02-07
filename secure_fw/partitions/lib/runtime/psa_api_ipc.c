/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "psa/client.h"
#include "tfm_psa_call_pack.h"
#include "sprt_partition_metadata_indicator.h"
#include "runtime_defs.h"

psa_status_t tfm_psa_call_pack(psa_handle_t handle,
                               uint32_t ctrl_param,
                               const psa_invec *in_vec,
                               psa_outvec *out_vec)
{
    return PART_METADATA()->psa_fns->psa_call(handle, ctrl_param,
                                              in_vec, out_vec);
}

#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1
psa_handle_t psa_connect(uint32_t sid, uint32_t version)
{
    return PART_METADATA()->psa_fns->psa_connect(sid, version);
}

void psa_close(psa_handle_t handle)
{
    return PART_METADATA()->psa_fns->psa_close(handle);
}
#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API */

uint32_t psa_version(uint32_t sid)
{
    return PART_METADATA()->psa_fns->psa_version(sid);
}

uint32_t psa_framework_version(void)
{
    return PART_METADATA()->psa_fns->psa_framework_version();
}
