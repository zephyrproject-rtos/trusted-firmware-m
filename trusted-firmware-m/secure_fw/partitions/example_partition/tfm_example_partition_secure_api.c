/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_example_partition_api.h"

#include "psa/client.h"
#include "psa_manifest/sid.h"

__attribute__((section("SFN")))
psa_status_t psa_example_service(uint32_t arg)
{
    psa_status_t status;
    psa_handle_t handle;
    psa_invec in_vec[] = {
        { .base = &arg, .len = sizeof(arg) },
    };

    handle = psa_connect(TFM_EXAMPLE_SERVICE_SID, TFM_EXAMPLE_SERVICE_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        return PSA_HANDLE_TO_ERROR(handle);
    }

    status = psa_call(handle, PSA_IPC_CALL, in_vec, 1, NULL, 0);

    psa_close(handle);

    return status;
}
