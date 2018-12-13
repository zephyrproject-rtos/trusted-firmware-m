/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PROTECTED_STORAGE_H__
#define __TFM_PROTECTED_STORAGE_H__

#include <stdint.h>

#include "psa_protected_storage.h"
#include "tfm_sst_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes the secure storage system.
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref tfm_sst_err_t
 *
 * \retval TFM_SST_ERR_SUCCESS            The operation completed successfully
 * \retval TFM_SST_ERROR_STORAGE_FAILURE  The operation failed because the
 *                                        storage system initialization has
 *                                        failed (fatal error)
 * \retval TFM_SST_ERR_OPERATION_FAILED   The operation failed because of an
 *                                        unspecified internal failure
 */
enum tfm_sst_err_t tfm_sst_init(void);

/**
 * \brief Creates a new or modifies an existing asset.
 *
 * \param[in] uid           Pointer to the unique identifier for the data
 * \param[in] data_length   The size in bytes of the data in `p_data`
 * \param[in] p_data        A buffer containing the data
 * \param[in] create_flags  The flags indicating the properties of the data
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref tfm_sst_err_t
 *
 * \retval TFM_SST_ERR_SUCCESS              The operation completed successfully
 * \retval TFM_SST_ERR_WRITE_ONCE           The operation failed because the
 *                                          provided uid value was already
 *                                          created with PSA_PS_FLAG_WRITE_ONCE
 * \retval TFM_SST_ERR_INVALID_ARGUMENT     The operation failed because one or
 *                                          more of the given arguments were
 *                                          invalid (null pointer, etc.)
 * \retval TFM_SST_ERR_FLAGS_NOT_SUPPORTED  The operation failed because one or
 *                                          more of the flags provided in
 *                                          `create_flags` is not supported or
 *                                          is not valid
 * \retval TFM_SST_ERR_INSUFFICIENT_SPACE   The operation failed because there
 *                                          was insufficient space on the
 *                                          storage medium
 * \retval TFM_SST_ERR_STORAGE_FAILURE      The operation failed because the
 *                                          physical storage has failed (fatal
 *                                          error)
 * \retval TFM_SST_ERR_OPERATION_FAILED     The operation failed because of an
 *                                          unspecified internal failure
 */
enum tfm_sst_err_t tfm_sst_set(const psa_ps_uid_t *uid,
                               uint32_t data_length,
                               const void *p_data,
                               psa_ps_create_flags_t create_flags);

/**
 * \brief Gets the asset data for the provided uid.
 *
 * \param[in]  uid          Pointer to the unique identifier for the data
 * \param[in]  data_offset  The offset within the data associated with the `uid`
 *                          to start retrieving data
 * \param[in]  data_length  The amount of data to read (and the minimum
 *                          allocated size of the `p_data` buffer)
 * \param[out] p_data       The buffer where the data will be placed upon
 *                          successful completion
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref tfm_sst_err_t
 *
 * \retval TFM_SST_ERR_SUCCESS           The operation completed successfully
 * \retval TFM_SST_ERR_INVALID_ARGUMENT  The operation failed because one or
 *                                       more of the given arguments were
 *                                       invalid (null pointer, etc.)
 * \retval TFM_SST_ERR_UID_NOT_FOUND     The operation failed because the
 *                                       provided uid value was not found in the
 *                                       storage
 * \retval TFM_SST_ERR_INCORRECT_SIZE    The operation failed because the data
 *                                       associated with provided uid is not the
 *                                       same size as `data_size`
 * \retval TFM_SST_ERR_STORAGE_FAILURE   The operation failed because the
 *                                       physical storage has failed (fatal
 *                                       error)
 * \retval TFM_SST_ERR_OPERATION_FAILED  The operation failed because of an
 *                                       unspecified internal failure
 * \retval TFM_SST_ERR_DATA_CORRUPT      The operation failed because the data
 *                                       associated with the UID was corrupt
 * \retval TFM_SST_ERR_AUTH_FAILED       The operation failed because the data
 *                                       associated with the UID failed
 *                                       authentication
 */
enum tfm_sst_err_t tfm_sst_get(const psa_ps_uid_t *uid,
                               uint32_t data_offset,
                               uint32_t data_length,
                               void *p_data);

/**
 * \brief Gets the metadata for the provided uid.
 *
 * \param[in]  uid     Pointer to the unique identifier for the data
 * \param[out] p_info  A pointer to the `psa_ps_info_t` struct that will be
 *                     populated with the metadata
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref tfm_sst_err_t
 *
 * \retval TFM_SST_ERR_SUCCESS           The operation completed successfully
 * \retval TFM_SST_ERR_INVALID_ARGUMENT  The operation failed because one or
 *                                       more of the given arguments were
 *                                       invalid (null pointer, etc.)
 * \retval TFM_SST_ERR_UID_NOT_FOUND     The operation failed because the
 *                                       provided uid value was not found in the
 *                                       storage
 * \retval TFM_SST_ERR_STORAGE_FAILURE   The operation failed because the
 *                                       physical storage has failed (fatal
 *                                       error)
 * \retval TFM_SST_ERR_OPERATION_FAILED  The operation failed because of an
 *                                       unspecified internal failure
 * \retval TFM_SST_ERR_DATA_CORRUPT      The operation failed because the data
 *                                       associated with the UID was corrupt
 * \retval TFM_SST_ERR_AUTH_FAILED       The operation failed because the data
 *                                       associated with the UID failed
 *                                       authentication
 */
enum tfm_sst_err_t tfm_sst_get_info(const psa_ps_uid_t *uid,
                                    struct psa_ps_info_t *p_info);

/**
 * \brief Removes the provided uid and its associated data from storage.
 *
 * \param[in] uid  Pointer to the unique identifier for the data to be removed
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref tfm_sst_err_t
 *
 * \retval TFM_SST_ERR_SUCCESS           The operation completed successfully
 * \retval TFM_SST_ERR_INVALID_ARGUMENT  The operation failed because one or
 *                                       more of the given arguments were
 *                                       invalid (null pointer, etc.)
 * \retval TFM_SST_ERR_UID_NOT_FOUND     The operation failed because the
 *                                       provided uid value was not found in the
 *                                       storage
 * \retval TFM_SST_ERR_WRITE_ONCE        The operation failed because the
 *                                       provided uid value was created with
 *                                       PSA_PS_WRITE_ONCE_FLAG
 * \retval TFM_SST_ERR_STORAGE_FAILURE   The operation failed because the
 *                                       physical storage has failed (fatal
 *                                       error)
 * \retval TFM_SST_ERR_OPERATION_FAILED  The operation failed because of an
 *                                       unspecified internal failure
 */
enum tfm_sst_err_t tfm_sst_remove(const psa_ps_uid_t *uid);

/**
 * \brief Gets a bitmask with flags set for all of the optional features
 *        supported by the implementation.
 *
 * \param[out] support_flags  A pointer to a variable that will be populated
 *                            with a uint32_t bitmask value which contains all
 *                            the bits set for all the optional features
 *                            supported by the implementation
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref tfm_sst_err_t
 *
 * \retval TFM_SST_ERR_SUCCESS           The operation completed successfully
 * \retval TFM_SST_ERR_OPERATION_FAILED  The operation failed because of an
 *                                       unspecified internal failure
 */
enum tfm_sst_err_t tfm_sst_get_support(uint32_t *support_flags);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PROTECTED_STORAGE_H__ */
