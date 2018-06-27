/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_TEST_SERVICE_H__
#define __SST_TEST_SERVICE_H__

#include <stdint.h>
#include "tfm_sst_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Sets-up the SST test service so that it is ready for test functions to
 *        be called.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_test_service_sfn_setup(void);

/**
 * \brief Performs a dummy encryption on the supplied buffer, using the key
 *        stored in the asset with the given UUID.
 *
 * \param[in]     app_id    Application ID
 * \param[in]     key_uuid  UUID of asset containing key
 * \param[in,out] buf       Plaintext buffer
 * \param[in]     buf_size  Size of buf
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_test_service_sfn_dummy_encrypt(uint32_t app_id,
                                                      uint32_t key_uuid,
                                                      uint8_t *buf,
                                                      uint32_t buf_size);

/**
 * \brief Performs a dummy decryption on the supplied buffer, using the key
 *        stored in the asset with the given UUID.
 *
 * \param[in]     app_id    Application ID
 * \param[in]     key_uuid  UUID of asset containing key
 * \param[in,out] buf       Ciphertext buffer
 * \param[in]     buf_size  Size of buf
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_test_service_sfn_dummy_decrypt(uint32_t app_id,
                                                      uint32_t key_uuid,
                                                      uint8_t *buf,
                                                      uint32_t buf_size);

/**
 * \brief Cleans the secure storage used by the SST test service.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_test_service_sfn_clean(void);

#ifdef __cplusplus
}
#endif

#endif /* __SST_TEST_SERVICE_H__ */
