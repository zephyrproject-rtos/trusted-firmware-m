/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __AUDIT_CORE_H__
#define __AUDIT_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "psa_audit_defs.h"

/*!
 * \struct log_entry
 *
 * \brief Structure of a single log entry
 *        in the log
 * \details This can't be represented as a
 *          structure because the payload
 *          is of variable size, i.e.
 * |Offset |Name        |
 * |-------|------------|
 * | 0     |TIMESTAMP   |
 * | 8     |IV_COUNTER  |
 * |12     |PARTITION ID|
 * |16     |SIZE        |
 * |20     |RECORD ID   |
 * |24     |PAYLOAD     |
 * |20+SIZE|MAC         |
 *
 * SIZE: at least LOG_MIN_SIZE bytes, known only at runtime. It's the size of
 *       the (RECORD_ID, PAYLOAD) fields
 *
 * MAC_SIZE: known at build time (currently, 4 bytes)
 *
 * At runtime, when adding a record, the value of SIZE has to be checked and
 * must be less than LOG_SIZE - MAC_SIZE - 12 and equal or greater than
 * LOG_MIN_SIZE
 *
 */

/*!
 * \def LOG_MIN_SIZE
 *
 * \brief Minimum size of the encrypted part
 */
#define LOG_MIN_SIZE (4)

/*!
 * \enum audit_tlv_type
 *
 * \brief Possible types for a TLV entry
 *        in payload
 */
enum audit_tlv_type {
    TLV_TYPE_ID = 0,
    TLV_TYPE_AUTH = 1,

    /* This is used to force the maximum size */
    TLV_TYPE_MAX = INT_MAX
};

/*!
 * \struct audit_tlv_entry
 *
 * \brief TLV entry structure with a flexible array member
 */
struct audit_tlv_entry {
    enum audit_tlv_type type;
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
 * \brief Fixed size header for a log record
 */
struct log_hdr {
    uint64_t timestamp;
    uint32_t iv_counter;
    int32_t partition_id;
    uint32_t size;
    uint32_t id;
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
 * \brief Size in bytes of the (fixed) header for each entry
 */
#define LOG_HDR_SIZE (sizeof(struct log_hdr))

/*!
 * \def LOG_TLR_SIZE
 *
 * \brief Size in bytes of the (fixed) trailer for each entry
 */
#define LOG_TLR_SIZE (sizeof(struct log_tlr))

/*!
 * \brief Initializes the Audit logging service
 *        during the TFM boot up process
 *
 * \return Returns PSA_AUDIT_ERR_SUCCESS if init has been completed,
 *         otherwise error as specified in \ref psa_audit_err
 */
enum psa_audit_err audit_core_init(void);

/*!
 * \brief Retrieves a record at the specified index
 *
 * \details The function retrieves an item specified by index and returns
 *          it on the buffer provided. The token is passed as a challenge
 *          value for the encryption scheme
 *
 * \note Currently the cryptography support is not yet enabled, so the
 *       token value is not used and must be passed as NULL, with 0 size
 *
 * \param[in]  record_index Index of the record to retrieve
 * \param[in]  buffer_size  Size in bytes of the provided buffer
 * \param[in]  token        Must be set to NULL. Token used as a challenge
 *                          for encryption, to protect against rollback
 *                          attacks
 * \param[in]  token_size   Must be set to 0. Size in bytes of the token
 *                          used as challenge
 * \param[out] buffer       Buffer used to store the retrieved record
 * \param[out] record_size  Size in bytes of the retrieved record
 *
 * \return Returns values as specified by the \ref psa_audit_err
 *
 */
enum psa_audit_err audit_core_retrieve_record(const uint32_t record_index,
                                              const uint32_t buffer_size,
                                              const uint8_t *token,
                                              const uint32_t token_size,
                                              uint8_t *buffer,
                                              uint32_t *record_size);
/*!
 * \brief Adds a record
 *
 * \details This function adds a record in the Audit log
 *
 * \param[in] record Pointer to the memory buffer containing the record
 *                   to be added
 *
 * \return Returns values as specified by the \ref psa_audit_err
 *
 */
enum psa_audit_err audit_core_add_record(const struct psa_audit_record *record);

/*!
 * \brief Returns the total number and size of the records stored
 *
 * \details The function returns the total size in bytes and the
 *          total number of records stored
 *
 * \param[out] num_records Total number of records stored
 * \param[out] size        Total size of the records stored, in bytes
 *
 * \return Returns values as specified by the \ref psa_audit_err
 *
 */
enum psa_audit_err audit_core_get_info(uint32_t *num_records,
                                       uint32_t *size);

/*!
 * \brief Returns the size of the record at the specified index
 *
 * \details The function returns the size of the record at the given index
 *          provided as input
 *
 * \param[in]  record_index Index of the record to return the size
 * \param[out] size         Size of the specified record, in bytes
 *
 * \return Returns values as specified by the \ref psa_audit_err
 *
 */
enum psa_audit_err audit_core_get_record_info(const uint32_t record_index,
                                              uint32_t *size);
/*!
 * \brief Deletes a record at the specified index
 *
 * \details The function removes a record at the specified index. It passes
 *          an authorisation token for removal which is a MAC of the plain text
 *
 * \note Currently the cryptography support is not yet enabled, so the
 *       token value is not used and must be passed as NULL, with 0 size
 *
 * \param[in] record_index Index of the record to be removed. Currently, only
 *                         the removal of the oldest entry, i.e. record_index 0
 *                         is supported
 * \param[in] token        Must be set to NULL. Token used as authorisation for
 *                         removal of the specified record_index
 * \param[in] token_size   Must be set to 0. Size in bytes of the token used as
 *                         authorisation for removal
 *
 * \return Returns values as specified by the \ref psa_audit_err
 *
 */
enum psa_audit_err audit_core_delete_record(const uint32_t record_index,
                                            const uint8_t *token,
                                            const uint32_t token_size);

#ifdef __cplusplus
}
#endif

#endif /* __AUDIT_CORE_H__ */
