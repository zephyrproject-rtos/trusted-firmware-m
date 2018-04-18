/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_secure_client_service_api.h"
#include "tfm_ns_svc.h"

__attribute__ ((naked))
int32_t tfm_secure_client_run_tests(void)
{
    SVC(SVC_TFM_SECURE_CLIENT_RUN_TESTS);
    __ASM("BX LR");
}
