/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_test_service_api.h"

#include "tfm_ns_svc.h"

__attribute__ ((naked))
enum psa_sst_err_t sst_test_service_setup(void)
{
    SVC(SVC_SST_TEST_SERVICE_SETUP);
    __ASM("BX LR");
}

__attribute__ ((naked))
enum psa_sst_err_t sst_test_service_dummy_encrypt(uint32_t key_uuid,
                                                  uint8_t *buf,
                                                  uint32_t buf_size)
{
    SVC(SVC_SST_TEST_SERVICE_DUMMY_ENCRYPT);
    __ASM("BX LR");
}

__attribute__ ((naked))
enum psa_sst_err_t sst_test_service_dummy_decrypt(uint32_t key_uuid,
                                                  uint8_t *buf,
                                                  uint32_t buf_size)
{
    SVC(SVC_SST_TEST_SERVICE_DUMMY_DECRYPT);
    __ASM("BX LR");
}

__attribute__ ((naked))
enum psa_sst_err_t sst_test_service_clean(void)
{
    SVC(SVC_SST_TEST_SERVICE_CLEAN);
    __ASM("BX LR");
}
