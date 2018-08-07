/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_test_service.h"

#include "secure_fw/core/tfm_secure_api.h"
#include "secure_fw/services/secure_storage/assets/sst_asset_defs.h"
#include "secure_fw/services/secure_storage/sst_asset_management.h"
#include "secure_fw/services/secure_storage/sst_utils.h"
#include "psa_sst_api.h"

#define SST_TEST_SERVICE_KEY { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, \
                               0xDE, 0xAD, 0xBE, 0xEF, 0xBA, 0xAD, 0xF0, 0x0D, }
#define SST_TEST_SERVICE_KEY_SIZE SST_ASSET_MAX_SIZE_AES_KEY_128

/* Define default asset's token */
#define ASSET_TOKEN      NULL
#define ASSET_TOKEN_SIZE 0

/**
 * \brief Service initialisation function. No special initialisation is
 *        required.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_test_service_init(void)
{
    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_test_service_sfn_setup(void)
{
    enum psa_sst_err_t err;
    const uint32_t key_uuid = SST_ASSET_ID_AES_KEY_128;
    static uint8_t key_data[SST_TEST_SERVICE_KEY_SIZE] = SST_TEST_SERVICE_KEY;


    /* Create the key asset using our secure app ID */
    err = psa_sst_create(key_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Write the key to the asset using our secure app ID */
    err = psa_sst_write(key_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE,
                        SST_TEST_SERVICE_KEY_SIZE, 0, key_data);

    return err;
}

enum psa_sst_err_t sst_test_service_sfn_dummy_encrypt(uint32_t key_uuid,
                                                      uint8_t *buf,
                                                      uint32_t buf_size)
{
    enum psa_sst_err_t err;
    uint32_t i;
    uint8_t key_data[SST_TEST_SERVICE_KEY_SIZE];
    int32_t client_id;

    if (tfm_core_get_caller_client_id(&client_id) != TFM_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Read the key from the asset using the non-secure caller's client ID */
    err = psa_sst_reference_read(client_id, key_uuid, ASSET_TOKEN,
                                 ASSET_TOKEN_SIZE, SST_TEST_SERVICE_KEY_SIZE,
                                 0, key_data);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Check the buffer is valid memory for the client that supplied it */
    err = sst_utils_memory_bound_check(buf, buf_size, client_id,
                                       TFM_MEMORY_ACCESS_RW);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    /* Encrypt the data (very badly) using the key from secure storage */
    for (i = 0; i < buf_size; i++) {
        buf[i] ^= key_data[i % SST_TEST_SERVICE_KEY_SIZE];
    }

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_test_service_sfn_dummy_decrypt(uint32_t key_uuid,
                                                      uint8_t *buf,
                                                      uint32_t buf_size)
{
    /* In the current implementation encrypt and decrypt are the same
     * operation.
     */
    return sst_test_service_sfn_dummy_encrypt(key_uuid, buf, buf_size);
}

enum psa_sst_err_t sst_test_service_sfn_clean(void)
{
    enum psa_sst_err_t err;
    const uint32_t key_uuid = SST_ASSET_ID_AES_KEY_128;

    /* Delete the key asset using our secure app ID */
    err = psa_sst_delete(key_uuid, ASSET_TOKEN, ASSET_TOKEN_SIZE);

    return err;
}
