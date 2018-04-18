/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_secure_client_service_svc.h"
#include "tfm_secure_client_service_veneers.h"

int32_t tfm_secure_client_service_svc_run_tests(void)
{
    return tfm_secure_client_service_veneer_run_tests();
}
