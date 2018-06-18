/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_UTILS_H__
#define __SST_UTILS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SST_INVALID_UUID  0

/*
 * \brief Macro to check, at compilation time, if data fits in data buffer
 *
 * \param[in] err_msg        Error message which will be displayed in first
 *                           instance if the error is tiggered
 * \param[in] data_size      Data size to check if it fits
 * \param[in] data_buf_size  Size of the data buffer
 *
 * \return  Triggers a compilation error if data_size is bigger than
 *          data_buf_size. The compilation error should be
 *          "... error: 'err_msg' declared as an array with a negative size"
 */
#define SST_UTILS_BOUND_CHECK(err_msg, data_size, data_buf_size) \
typedef char err_msg[(data_size <= data_buf_size) - 1]

/**
 * \brief Acquires sst system lock. This lock is used for serializing accesses
 */
void sst_global_lock(void);

/**
 * \brief Releases sst system lock
 */
void sst_global_unlock(void);

/**
 * \brief Checks if the provided memory region is a valid address for
 *        the given application ID to read/write
 *
 * \param[in] addr    Address to the memory region to validate
 * \param[in] size    Memory region size in bytes
 * \param[in] app_id  Application ID
 * \param[in] access  Access type to be permormed on the given address
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_utils_memory_bound_check(void *addr,
                                                uint32_t size,
                                                uint32_t app_id,
                                                uint32_t access);

/**
 * \brief Checks bound, authenticate and copy
 *
 * \param[in] src     Source buffer
 * \param[in] dest    Destination buffer
 * \param[in] size    Size of the incoming buffer
 * \param[in] app_id  App_id for the incoming buffer
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_utils_bound_check_and_copy(uint8_t *src,
                                                  uint8_t *dest,
                                                  uint32_t size,
                                                  uint32_t app_id);

/**
 * \brief Checks if a given memory region is contained within another region
 *
 * \param[in] superset_start  Start of superset region
 * \param[in] superset_size   Size of superset region
 * \param[in] subset_start    Start of subset region
 * \param[in] subset_size     Size of the subset region
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_utils_check_contained_in(uint32_t superset_start,
                                                uint32_t superset_size,
                                                uint32_t subset_start,
                                                uint32_t subset_size);

/* FIXME: following functions(memcpy and memset) will be provided
 * by core. This is only an interim abstraction. In the current
 * implementation the stdlib is linked and protected as part of the TFM core
 * and not available for services.
 */
/**
 * \brief Memory copy function
 *
 * \param[out] dest  Destination position
 * \param[in]  src   Source position
 * \param[in]  size  Size of data to copy
 */
void sst_utils_memcpy(void *dest, const void *src, uint32_t size);

/**
 * \brief Memory set function
 *
 * \param[out] dest     Destination position
 * \param[in]  pattern  Pattern value to set
 * \param[in]  size     Size of data to set
 */
void sst_utils_memset(void *dest, const uint8_t pattern, uint32_t size);

/**
 * \brief Checks if the sst function caller is located in the secure or
 *        non-secure space.
 *
 * \return Returns 0 if the caller is located in the secure area, otherwise
 *         returns unspecified non-zero value if it is in the non-secure area
 */
uint32_t sst_utils_validate_secure_caller(void);

/**
 * \brief Validates asset's ID
 *
 * \param[in] unique_id  Asset's ID
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_utils_validate_uuid(uint32_t unique_id);

#ifdef __cplusplus
}
#endif

#endif /* __SST_UTILS_H__ */
