/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "s_test_helpers.h"

#include <stdio.h>
#include <string.h>

#include "test/framework/test_framework.h"
#include "secure_fw/services/secure_storage/sst_object_system.h"

uint32_t prepare_test_ctx(struct test_result_t *ret)
{
    /* Wipes secure storage area */
    sst_system_wipe_all();

    /* Prepares secure storage area before write */
    if (sst_system_prepare() != PSA_SST_ERR_SUCCESS) {
        TEST_FAIL("Wiped system should be preparable");
        return 1;
    }

    return 0;
}
