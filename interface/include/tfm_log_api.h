/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LOG_API__
#define __TFM_LOG_API__

#ifdef __cplusplus
extern "C" {
#endif

#include "tfm_log_defs.h"

/**
 * \brief Retrieves the audit log
 *
 * \param[in]  size     Maximum number of bytes to retrieve from the log
 * \param[out] buffer   Pointer to the buffer that will hold the log
 * \param[out] log_size Pointer to the actual size of the log retrieved
 *
 * \return Returns TFM_LOG_ERR_SUCCESS if retrieval has been completed,
 *         otherwise error as specified in \ref tfm_log_err
 */
enum tfm_log_err tfm_log_retrieve(uint32_t size,
                                  uint8_t *buffer,
                                  uint32_t *log_size);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_LOG_API__ */
