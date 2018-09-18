/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_record.h"
#include "tfm_boot_status.h"
#include "target.h"
#include <stdint.h>
#include <string.h>

/*!
 * \var shared_memory_init_done
 *
 * \brief Indicates whether shared memory area was already initialized.
 *
 */
static uint32_t shared_memory_init_done;

/*!
 * \def SHARED_MEMORY_UNINITIALZED
 *
 * \brief Indicates that shared memory is uninitialized.
 */
#define SHARED_MEMORY_UNINITIALZED (0u)

/*!
 * \def SHARED_MEMORY_INITIALZED
 *
 * \brief Indicates that shared memory was already initialized.
 */
#define SHARED_MEMORY_INITIALZED   (1u)

enum shared_memory_err_t
boot_add_data_to_shared_area(uint8_t        major_type,
                             uint8_t        minor_type,
                             size_t         size,
                             const uint8_t *data)
{
    struct shared_data_tlv_entry tlv_entry = {0};
    struct shared_data_tlv_header *tlv_header;
    uint8_t *next_tlv;
    uintptr_t tlv_end, offset;

    tlv_header = (struct shared_data_tlv_header *)BOOT_TFM_SHARED_DATA_BASE;

    /* Check whether first time to call this function. If does then initialise
     * shared data area.
     */
    if (shared_memory_init_done == SHARED_MEMORY_UNINITIALZED) {
        memset((void *)BOOT_TFM_SHARED_DATA_BASE, 0, BOOT_TFM_SHARED_DATA_SIZE);
        tlv_header->tlv_magic   = SHARED_DATA_TLV_INFO_MAGIC;
        tlv_header->tlv_tot_len = SHARED_DATA_HEADER_SIZE;
        shared_memory_init_done = SHARED_MEMORY_INITIALZED;
    }

    /* Check whether TLV entry is already added.
     * Get the boundaries of TLV section
     */
    tlv_end = BOOT_TFM_SHARED_DATA_BASE + tlv_header->tlv_tot_len;
    offset  = BOOT_TFM_SHARED_DATA_BASE + SHARED_DATA_HEADER_SIZE;

    /* Iterates over the TLV section looks for the same entry if found then
     * returns with error: SHARED_MEMORY_OVERWRITE
     */
    for(; offset < tlv_end; offset += tlv_entry.tlv_len) {
        tlv_entry = *((struct shared_data_tlv_entry *)offset);
        if (tlv_entry.tlv_major_type == major_type &&
            tlv_entry.tlv_minor_type == minor_type ) {
            return SHARED_MEMORY_OVERWRITE;
        }
    }

    /* Add TLV entry */
    tlv_entry.tlv_major_type = major_type;
    tlv_entry.tlv_minor_type = minor_type;
    tlv_entry.tlv_len        = SHARED_DATA_ENTRY_SIZE(size);

    /* Verify overflow of shared area */
    if ((tlv_header->tlv_tot_len + tlv_entry.tlv_len) >
         BOOT_TFM_SHARED_DATA_SIZE){
        return SHARED_MEMORY_OVERFLOW;
    }

    next_tlv = (uint8_t *)tlv_header + tlv_header->tlv_tot_len;
    memcpy(next_tlv, &tlv_entry, SHARED_DATA_ENTRY_HEADER_SIZE);

    next_tlv += SHARED_DATA_ENTRY_HEADER_SIZE;
    memcpy(next_tlv, data, size);

    tlv_header->tlv_tot_len = tlv_header->tlv_tot_len + tlv_entry.tlv_len;

    return SHARED_MEMORY_OK;
}
