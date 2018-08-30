/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "core_test_api.h"
#include "test/test_services/tfm_core_test/core_test_defs.h"

int32_t tfm_core_test_call(void *fn_ptr, int32_t args[])
{
    int32_t (*fn_ptr_to_call)(int32_t, int32_t, int32_t, int32_t) = fn_ptr;
    return fn_ptr_to_call(args[0], args[1], args[2], args[3]);
}
