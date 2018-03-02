/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_test_service_svc.h"

#include "sst_test_service_veneers.h"
#include "tfm_id_mngr.h"

enum tfm_sst_err_t sst_test_service_svc_setup(void)
{
    return sst_test_service_veneer_setup();
}

enum tfm_sst_err_t sst_test_service_svc_dummy_encrypt(uint16_t key_uuid,
                                                      uint8_t *buf,
                                                      uint32_t buf_size)
{
    uint32_t app_id;

    app_id = tfm_sst_get_cur_id();

    return sst_test_service_veneer_dummy_encrypt(app_id, key_uuid, buf,
                                                 buf_size);
}

enum tfm_sst_err_t sst_test_service_svc_dummy_decrypt(uint16_t key_uuid,
                                                      uint8_t *buf,
                                                      uint32_t buf_size)
{
    uint32_t app_id;

    app_id = tfm_sst_get_cur_id();

    return sst_test_service_veneer_dummy_decrypt(app_id, key_uuid, buf,
                                                 buf_size);
}

enum tfm_sst_err_t sst_test_service_svc_clean(void)
{
    return sst_test_service_veneer_clean();
}
