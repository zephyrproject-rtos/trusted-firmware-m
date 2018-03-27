/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_test_service_veneers.h"

#include "sst_test_service.h"
#include "secure_fw/core/tfm_secure_api.h"
#include "secure_fw/spm/service_defs.h"

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t sst_test_service_veneer_setup(void)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_SST_TEST_PARTITION_ID,
                         sst_test_service_sfn_setup,
                         0, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t sst_test_service_veneer_dummy_encrypt(uint32_t app_id,
                                                         uint16_t key_uuid,
                                                         uint8_t *buf,
                                                         uint32_t buf_size)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_SST_TEST_PARTITION_ID,
                         sst_test_service_sfn_dummy_encrypt,
                         app_id, key_uuid, buf, buf_size);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t sst_test_service_veneer_dummy_decrypt(uint32_t app_id,
                                                         uint16_t key_uuid,
                                                         uint8_t *buf,
                                                         uint32_t buf_size)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_SST_TEST_PARTITION_ID,
                         sst_test_service_sfn_dummy_decrypt,
                         app_id, key_uuid, buf, buf_size);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t sst_test_service_veneer_clean(void)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_SST_TEST_PARTITION_ID,
                         sst_test_service_sfn_clean,
                         0, 0, 0, 0);
}
