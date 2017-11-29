/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "app/tfm_integ_test.h"

#include "core_test_api.h"
#include "tfm_ns_svc.h"

__attribute__ ((naked)) int32_t tfm_core_test_svc(void *fn_ptr, int32_t args[])
{
    SVC(SVC_TFM_CORE_TEST);
    __ASM("BX LR");
}

__attribute__ ((naked)) int32_t tfm_core_test_multiple_calls_svc(void *fn_ptr,
                                                                 int32_t args[])
{
    SVC(SVC_TFM_CORE_TEST_MULTIPLE_CALLS);
    __ASM("BX LR");
}
