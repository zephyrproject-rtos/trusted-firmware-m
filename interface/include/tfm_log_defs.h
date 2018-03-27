/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LOG_DEFS_H__
#define __TFM_LOG_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_api.h"
#include "limits.h"

/*!
 * \def TFM_LOG_READ_RECENT
 *
 * \brief Special value used in the log retrieval API to indicate
 *        that data should be read up to the most recent entry
 */
#define TFM_LOG_READ_RECENT (-1)

/*!
 * \struct tfm_log_info
 *
 * \brief Structure containing information related to the size in bytes
 *        and number of items retrieved or available in the audit log
 */
struct tfm_log_info {
    uint32_t size; /*!< Size in bytes of items retrieved or available */
    uint32_t num_items; /*!< Number of items retrieved or available */
};

/* The return value is shared with the TFM partition status value. The LOG
 * return codes shouldn't overlap with predefined TFM status values.
 */
#define TFM_LOG_ERR_OFFSET (TFM_PARTITION_SPECIFIC_ERROR_MIN)

enum tfm_log_err {
    TFM_LOG_ERR_SUCCESS = 0,
    TFM_LOG_ERR_FAILURE = TFM_LOG_ERR_OFFSET,

    /* Following entry is only to ensure the error code of int size */
    TFM_LOG_ERR_FORCE_INT_SIZE = INT_MAX
};

/*!
 * \struct tfm_log_line
 *
 * \brief The part of the log line which has to be
 *        provided by the secure service that wants
 *        to add information to the log
 */
struct tfm_log_line {
    uint32_t size;        /*!< Size in bytes of the three following fields */
    uint32_t function_id; /*!< ID of the function requested */
    uint32_t arg[4];      /*!< [r0,r1,r2,r3] arguments to the function */
    uint8_t  payload[];   /*!< Flexible array member for payload */
};

#ifdef __cplusplus
}
#endif

#endif /* __TFM_LOG_DEFS_H__ */
