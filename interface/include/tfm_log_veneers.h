/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LOG_VENEERS_H__
#define __TFM_LOG_VENEERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
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
enum tfm_log_err tfm_log_veneer_retrieve(uint32_t size,
                                         uint8_t *buffer,
                                         uint32_t *log_size);
/**
 * \brief Adds a log entry
 *
 * \param[in] line Pointer to the line to be added. This memory must
 *                 be accessible by the audit logging service.
 *
 * \return Returns TFM_LOG_ERR_SUCCESS if addition has been completed
 *         otherwise error as specified in \ref tfm_log_err
 *
 * \note This is a secure only callable API, NS calls will always
 *       return error
 */
enum tfm_log_err tfm_log_veneer_add_line(struct tfm_log_line *line);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_LOG_VENEERS_H__ */
