/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "test/test_services/tfm_core_test/core_test_defs.h"

int32_t svc_tfm_core_test(int32_t (*fn_ptr)(int32_t, int32_t, int32_t, int32_t),
                          int32_t args[])
{
    return fn_ptr(args[0], args[1], args[2], args[3]);
}

int32_t svc_tfm_core_test_multiple_calls(
            int32_t (*fn_ptr)(int32_t, int32_t, int32_t, int32_t),
            int32_t args[])
{
    int32_t res = fn_ptr(args[0], args[1], args[2], args[3]);

    if (res != CORE_TEST_ERRNO_SUCCESS) {
        return CORE_TEST_ERRNO_FIRST_CALL_FAILED;
    }
    res = fn_ptr(args[0], args[1], args[2], args[3]);
    if (res != CORE_TEST_ERRNO_SUCCESS) {
        return CORE_TEST_ERRNO_SECOND_CALL_FAILED;
    }
    return CORE_TEST_ERRNO_SUCCESS;
}
