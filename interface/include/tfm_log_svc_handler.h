/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LOG_SVC_HANDLER_H__
#define __TFM_LOG_SVC_HANDLER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tfm_log_defs.h"

/**
 * \brief Retrieves the audit log (SVC function)
 *
 * \details The function reads the audit log into the buffer provided.
 *          If provided buffer size is too small to fit the full log,
 *          the function will read the maximum number of items in the
 *          log that fit the available space in the buffer
 *
 * \param[in]  size     Maximum number of bytes to retrieve from the log
 * \param[in]  start    Index of element from where to start retrieval
 * \param[out] buffer   Pointer to the buffer that will hold the log
 * \param[out] info     Pointer to the \ref tfm_log_info structure
 *                      contained information related to the retrieved
 *                      portion of the log (size and number of items)
 *
 * \return Returns TFM_LOG_ERR_SUCCESS if retrieval has been completed,
 *         otherwise error as specified in \ref tfm_log_err
 *
 * \note If start is equal to TFM_ALG_READ_RECENT, the function will
 *       retrieve the most recent elements that fit the provided size
 */
enum tfm_log_err tfm_log_svc_retrieve(uint32_t size,
                                      int32_t start,
                                      uint8_t *buffer,
                                      struct tfm_log_info *info);

/**
 * \brief Gets the log information (SVC function)
 *
 * \param[out] info Pointer to the \ref tfm_log_info structure that
 *                  holds the current log size (both in bytes and items)
 *
 * \return Returns TFM_LOG_ERR_SUCCESS if reading has been completed,
 *         otherwise error as specified in \ref tfm_log_err
 */
enum tfm_log_err tfm_log_svc_get_info(struct tfm_log_info *info);

/**
 * \brief Deletes one or more elements from the head of the log (SVC function)
 *
 * \param[in]  num_items Number of elements to be deleted
 * \param[out] rem_items Pointer to the number of elements removed. This
 *                       value indicates the number of elements actually
 *                       removed from the log. In case the number of items
 *                       stored is less than the number of items requested
 *                       to remove, this value will reflect the number of
 *                       items effectively removed.
 *
 * \return Returns TFM_LOG_ERR_SUCCESS if removal has been completed,
 *         otherwise error as specified in \ref tfm_log_err
 */
enum tfm_log_err tfm_log_svc_delete_items(uint32_t num_items,
                                          uint32_t *rem_items);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_LOG_SVC_HANDLER_H__ */
