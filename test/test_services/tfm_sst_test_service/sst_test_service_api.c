/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_test_service_api.h"
#include "sst_test_service_veneers.h"
#include "tfm_id_mngr.h"

enum psa_sst_err_t sst_test_service_setup(void)
{
    return sst_test_service_veneer_setup();
}

enum psa_sst_err_t sst_test_service_dummy_encrypt(uint32_t key_uuid,
                                                  uint8_t *buf,
                                                  uint32_t buf_size)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return sst_test_service_veneer_dummy_encrypt(client_id, key_uuid, buf,
                                                 buf_size);
}

enum psa_sst_err_t sst_test_service_dummy_decrypt(uint32_t key_uuid,
                                                  uint8_t *buf,
                                                  uint32_t buf_size)
{
    int32_t client_id;

    client_id = tfm_sst_get_cur_id();

    return sst_test_service_veneer_dummy_decrypt(client_id, key_uuid, buf,
                                                 buf_size);
}

enum psa_sst_err_t sst_test_service_clean(void)
{
    return sst_test_service_veneer_clean();
}
