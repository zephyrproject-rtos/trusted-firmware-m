/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTEST_H__
#define __ATTEST_H__

#include "psa/initial_attestation.h"
#include "psa/client.h"
#include "tfm_boot_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum psa_attest_err_t
 *
 * \brief Initial attestation service error types
 *
 */
enum psa_attest_err_t {
    /** Action was performed successfully */
    PSA_ATTEST_ERR_SUCCESS = 0,
    /** Boot status data is unavailable or malformed */
    PSA_ATTEST_ERR_INIT_FAILED,
    /** Buffer is too small to store required data */
    PSA_ATTEST_ERR_BUFFER_OVERFLOW,
    /** Some of the mandatory claims are unavailable*/
    PSA_ATTEST_ERR_CLAIM_UNAVAILABLE,
    /** Some parameter or combination of parameters are recognised as invalid:
     * - challenge size is not allowed
     * - challenge object is unavailable
     * - token buffer is unavailable
     */
    PSA_ATTEST_ERR_INVALID_INPUT,
    /** Unexpected error happened during operation */
    PSA_ATTEST_ERR_GENERAL,
    /** Following entry is only to ensure the error code of integer size */
    PSA_ATTEST_ERR_FORCE_INT_SIZE = INT_MAX
};

/*!
 * \brief Copy the boot data (coming from boot loader) from shared memory area
 *        to service memory area
 *
 * \param[in]   major_type  Major type of TLV entries to copy
 * \param[out]  boot_data   Pointer to the buffer to store the boot data
 * \param[in]   len         Size of the buffer to store the boot data
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
enum psa_attest_err_t
attest_get_boot_data(uint8_t major_type,
                     struct tfm_boot_data *boot_data,
                     uint32_t len);

/*!
 * \brief Get the ID of the caller thread.
 *
 * \param[out]  caller_id  Pointer where to store caller ID
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
enum psa_attest_err_t
attest_get_caller_client_id(int32_t *caller_id);

/*!
 * \brief Initialise the initial attestation service during the TF-M boot up
 *        process.
 *
 * \return Returns PSA_SUCCESS if init has been completed,
 *         otherwise error as specified in \ref psa_status_t
 */
psa_status_t attest_init(void);

/*!
 * \brief Get initial attestation token
 *
 * \param[in]     challenge_buf   Pointer to buffer where challenge input is
 *                                stored.
 * \param[in]     challenge_size  Size of challenge object in bytes.
 * \param[out]    token_buf       Pointer to the buffer where attestation token
 *                                will be stored.
 * \param[in]     token_buf_size  Size of allocated buffer for token, in bytes.
 * \param[out]    token_size      Size of the token that has been returned, in
 *                                bytes.
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t
initial_attest_get_token(const void *challenge_buf, size_t challenge_size,
                         void *token_buf, size_t token_buf_size,
                         size_t *token_size);

/**
 * \brief Get the size of the initial attestation token
 *
 * \param[in]   challenge_size  Size of challenge object in bytes. This must be
 *                              a supported challenge size.
 * \param[out]  token_size      Size of the token in bytes, which is created by
 *                              initial attestation service.
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t
initial_attest_get_token_size(size_t challenge_size, size_t *token_size);

#ifdef __cplusplus
}
#endif

#endif /* __ATTEST_H__ */
