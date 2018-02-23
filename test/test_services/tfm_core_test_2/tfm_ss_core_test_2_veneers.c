/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_ss_core_test_2_veneers.h"
#include "tfm_secure_api.h"
#include "tfm_ss_core_test_2.h"
#include "secure_fw/spm/spm_api.h"

/* Add functions to the service*/
int32_t tfm_core_test_2_veneer_slave_service(void)
{
   TFM_CORE_SERVICE_REQUEST(TFM_SP_CORE_TEST_2_ID,
                            spm_core_test_2_slave_service,
                            0, 0, 0, 0);
}

int32_t tfm_core_test_2_sfn_invert(int32_t *res_ptr, uint32_t *in_ptr,
                                   uint32_t *out_ptr, int32_t len)
{
    TFM_CORE_SERVICE_REQUEST(TFM_SP_CORE_TEST_2_ID,
                             spm_core_test_2_sfn_invert,
                             res_ptr, in_ptr, out_ptr, len);
}
