/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_secure_client_service.h"
#include "test/framework/integ_test.h"

/**
 * \brief Service initialisation function. No special initialisation is
 *        required.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t tfm_secure_client_service_init(void)
{
    return PSA_SST_ERR_SUCCESS;
}

int32_t tfm_secure_client_service_sfn_run_tests(void)
{
    start_integ_test();
    return 0;
}
