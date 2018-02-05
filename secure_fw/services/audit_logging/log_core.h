/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __LOG_CORE_H__
#define __LOG_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "tfm_log_defs.h"

/*!
 * \struct log_entry
 *
 * \brief Structure of a single log entry
 *        in the log
 * \details This can't be represented as a
 *          structure because the payload
 *          is of variable size, i.e.
 *
 * +-------------+0
 * | TIMESTAMP   |
 * |             |
 * +-------------+4
 * | PARTITION ID|
 * |             |
 * +-------------+8
 * | SIZE        |
 * |             |
 * +-------------+12
 * | FUNCTION ID |
 * |             |
 * +-------------+16
 * | ARG0-3      |
 * |             |
 * |             |
 * +-------------+32
 * | PAYLOAD     |
 * |             |
 * |             |
 * |             |
 * +-------------+12 + SIZE
 * | MAC         |
 * |             |
 * |             |
 * +-------------+12 + SIZE + MAC_SIZE - 1
 *
 * SIZE: at least 20 bytes
 *       known only at runtime. It's the
 *       size of the three fields that
 *       follow it.
 *
 * MAC_SIZE: known at build time (currently,
 *           4 bytes)
 *
 * At runtime SIZE has to be checked and
 * must be less than LOG_SIZE - MAC_SIZE - 12
 * and equal or greater than 20
 */

/*!
 * \def LOG_MIN_SIZE
 *
 * \brief Minimum size of the encrypted
 *        part
 */
#define LOG_MIN_SIZE (20)

/*!
 * \enum log_tlv_type
 *
 * \brief Possible types for a TLV entry
 *        in payload
 */
enum log_tlv_type {
    TLV_TYPE_ID = 0,
    TLV_TYPE_AUTH = 1,

    /* This is used to force the maximum size */
    TLV_TYPE_MAX = INT_MAX
};

/*!
 * \struct log_tlv_entry
 *
 * \brief TLV entry structure with a flexible
 *        array member
 */
struct log_tlv_entry {
    enum log_tlv_type type;
    uint32_t length;
    uint8_t value[];
};

/*!
 * \def LOG_MAC_SIZE
 *
 * \brief Size in bytes of the MAC for each entry
 */
#define LOG_MAC_SIZE (4)

/*!
 * \struct log_hdr
 *
 * \brief Fixed size logging entry header
 */
struct log_hdr {
    uint32_t timestamp;
    uint32_t partition_id;
    uint32_t size;
    uint32_t function_id;
    uint32_t arg[4];
};

/*!
 * \struct log_tlr
 *
 * \brief Fixed size logging entry trailer
 */
struct log_tlr {
    uint8_t mac[LOG_MAC_SIZE];
};

/*!
 * \def LOG_HDR_SIZE
 *
 * \brief Size in bytes of the (fixed) header for
 *        each entry
 */
#define LOG_HDR_SIZE (sizeof(struct log_hdr))

/*!
 * \def LOG_TLR_SIZE
 *
 * \brief Size in bytes of the (fixed) trailer for
 *        each entry
 */
#define LOG_TLR_SIZE (sizeof(struct log_tlr))

/*!
 * \brief Retrieves at most size bytes from the log
 *
 * \param[in]  size     Maximum number of bytes to retrieve from the log
 * \param[out] buffer   Pointer to the buffer that will hold the log
 * \param[out] log_size Pointer to the actual size of the log retrieved
 *
 * \return Returns TFM_LOG_ERR_SUCCESS if retrieval has been completed,
 *         otherwise error as specified in \ref tfm_log_err
 *
 */
enum tfm_log_err log_core_retrieve(uint32_t size,
                                   uint8_t *buffer,
                                   uint32_t *log_size);
/*!
 * \brief Initializes the Audit logging service
 *        during the TFM boot up process
 *
 * \return Returns TFM_LOG_ERR_SUCCESS if init has been completed,
 *         otherwise error as specified in \ref tfm_log_err
 */
enum tfm_log_err log_core_init(void);

/*!
 * \brief Adds a log line to the log. It's meant to be called
 *        only by other services that want to log information
 *
 * \param[in] line Pointer to the line to be added. This memory must
 *                 be accessible by the Audit logging service
 *
 * \return Returns TFM_LOG_ERR_SUCCESS if addition has been completed
 *         otherwise error as specified in \ref tfm_log_err
 *
 */
enum tfm_log_err log_core_add_line(struct tfm_log_line *line);

#ifdef __cplusplus
}
#endif

#endif /* LOG_CORE_H_ */
