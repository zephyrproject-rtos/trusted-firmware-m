/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SST_DEFS_H__
#define __TFM_SST_DEFS_H__

#include <limits.h>
#include "psa_protected_storage.h"
#include "tfm_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The return value is shared with the TF-M partition status value.
 * The SST return codes shouldn't overlap with predefined TF-M status values.
 */
#define TFM_SST_ERR_OFFSET (TFM_PARTITION_SPECIFIC_ERROR_MIN)

/**
 * \enum tfm_sst_err_t
 *
 * \brief TF-M Secure Storage service error types
 *
 */
enum tfm_sst_err_t {
    TFM_SST_ERR_SUCCESS = 0,
    TFM_SST_ERR_WRITE_ONCE = TFM_SST_ERR_OFFSET,
    TFM_SST_ERR_FLAGS_NOT_SUPPORTED,
    TFM_SST_ERR_INSUFFICIENT_SPACE,
    TFM_SST_ERR_STORAGE_FAILURE,
    TFM_SST_ERR_UID_NOT_FOUND,
    TFM_SST_ERR_INCORRECT_SIZE,
    TFM_SST_ERR_OFFSET_INVALID,
    TFM_SST_ERR_INVALID_ARGUMENT,
    TFM_SST_ERR_DATA_CORRUPT,
    TFM_SST_ERR_AUTH_FAILED,
    TFM_SST_ERR_OPERATION_FAILED,
    TFM_SST_ERR_NOT_SUPPORTED,
    /* Add an invalid return code which forces the size of the type as well */
    TFM_SST_ERR_INVALID = INT_MAX
};

/**
 * \brief A macro to translate TF-M API return values including the offset
 *        needed by TF-M, to the corresponding PSA value.
 */
#define TFM_SST_PSA_RETURN(err) (                                              \
    (err) == TFM_SST_ERR_SUCCESS ? err :                                       \
    (err) >= TFM_SST_ERR_WRITE_ONCE ? ((err) - (TFM_SST_ERR_WRITE_ONCE - 1)) : \
    TFM_SST_ERR_INVALID                                                        \
)

/* Invalid UID */
#define TFM_SST_INVALID_UID 0

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SST_DEFS_H__ */
