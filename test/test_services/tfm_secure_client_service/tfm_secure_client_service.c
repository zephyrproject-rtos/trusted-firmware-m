/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_secure_client_service.h"
#include "test/framework/test_framework_integ_test.h"

/**
 * \brief Service initialisation function. No special initialisation is
 *        required.
 *
 * \return Returns 0 on success
 */
int32_t tfm_secure_client_service_init(void)
{
    return 0;
}

int32_t tfm_secure_client_service_sfn_run_tests(void)
{
    start_integ_test();
    return 0;
}
