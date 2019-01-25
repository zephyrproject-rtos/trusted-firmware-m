/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_UTILS_H__
#define __SST_UTILS_H__

#include <stdint.h>

#include "flash_layout.h"
#include "psa_protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SST_INVALID_FID  0
#define SST_DEFAULT_EMPTY_BUFF_VAL 0

/* FIXME: Support other flash program units.*/
#if ((SST_FLASH_PROGRAM_UNIT != 1) && (SST_FLASH_PROGRAM_UNIT != 2) \
      && (SST_FLASH_PROGRAM_UNIT != 4))
#error "The supported SST_FLASH_PROGRAM_UNIT values are 1, 2 or 4 bytes"
#endif

/**
 * \brief Macro to check, at compilation time, if data fits in data buffer
 *
 * \param[in] err_msg        Error message which will be displayed in first
 *                           instance if the error is triggered
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
 * \brief Macro to get the number of bytes aligned with the
 *        SST_FLASH_PROGRAM_UNIT.
 *
 * \param[in] nbr_bytes  Number of bytes
 *
 * \return Return number of bytes aligned with SST_FLASH_PROGRAM_UNIT
 */
#define GET_ALIGNED_FLASH_BYTES(nbr_bytes) \
 ((nbr_bytes + (SST_FLASH_PROGRAM_UNIT - 1)) & ~((SST_FLASH_PROGRAM_UNIT - 1)))

/**
 * \brief Acquires sst system lock. This lock is used for serializing accesses
 */
void sst_global_lock(void);

/**
 * \brief Releases sst system lock
 */
void sst_global_unlock(void);

/**
 * \brief Checks if a subset region is fully contained within a superset region.
 *
 * \param[in] superset_size  Size of superset region
 * \param[in] subset_offset  Offset of start of subset region from start of
 *                           superset region
 * \param[in] subset_size    Size of subset region
 *
 * \return Returns error code as specified in \ref psa_ps_status_t
 *
 * \retval PSA_PS_SUCCESS               The subset is contained within the
 *                                      superset
 * \retval PSA_PS_ERROR_OFFSET_INVALID  The subset offset is greater than the
 *                                      size of the superset
 * \retval PSA_PS_ERROR_INCORRECT_SIZE  The subset offset is valid, but the
 *                                      subset offset + size is greater than the
 *                                      size of the superset
 */
psa_ps_status_t sst_utils_check_contained_in(uint32_t superset_size,
                                             uint32_t subset_offset,
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
 * \brief Validates file ID
 *
 * \param[in] fid  File ID
 *
 * \return Returns error code as specified in \ref psa_ps_status_t
 */
psa_ps_status_t sst_utils_validate_fid(uint32_t fid);

#ifdef __cplusplus
}
#endif

#endif /* __SST_UTILS_H__ */
