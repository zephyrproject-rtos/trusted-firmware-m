/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RUNTIME_SHARED_DATA_H__
#define __RUNTIME_SHARED_DATA_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Major numbers (4 bit) to identify
 * the consumer of shared data in boot SW
 */
#define TLV_RUNTIME_MAJOR_ADAC     0x1
#define TLV_RUNTIME_MAJOR_INVALID  0xF

/* General macros to handle TLV type */
#define MAJOR_MASK 0xF     /* 4  bit */
#define MAJOR_POS  12      /* 12 bit */
#define MINOR_MASK 0xFFF   /* 12 bit */
#define MINOR_POS  0
#define MASK_LEFT_SHIFT(data, mask, position)  \
                    (((data) & (mask)) << (position))
#define MASK_RIGHT_SHIFT(data, mask, position) \
                    ((uint16_t)((data) & (mask)) >> (position))

#define SET_TLV_TYPE(major, minor) \
                    (MASK_LEFT_SHIFT(major, MAJOR_MASK, MAJOR_POS) | \
                     MASK_LEFT_SHIFT(minor, MINOR_MASK, MINOR_POS))
#define GET_MAJOR(tlv_type) ((tlv_type) >> MAJOR_POS)
#define GET_MINOR(tlv_type) ((tlv_type) &  MINOR_MASK)


/* Magic value which marks the beginning of shared data area in memory */
#define RUNTIME_SHARED_DATA_TLV_INFO_MAGIC    0xAA55


/**
 * Shared data TLV header.  All fields in little endian.
 *
 *    -----------------------------------
 *    | tlv_magic(16) | tlv_tot_len(16) |
 *    -----------------------------------
 */
struct runtime_shared_data_tlv_header {
    uint16_t tlv_magic;
    uint16_t tlv_tot_len; /* size of whole TLV area (including this header) */
};

#define RUNTIME_SHARED_DATA_HEADER_SIZE \
        sizeof(struct runtime_shared_data_tlv_header)

/**
 * Shared data TLV entry header format. All fields in little endian.
 *
 *    -------------------------------
 *    | tlv_type(16) |  tlv_len(16) |
 *    -------------------------------
 *    |         Raw data            |
 *    -------------------------------
 */
struct runtime_shared_data_tlv_entry {
    uint16_t tlv_type;
    uint16_t tlv_len; /* size of single TLV entry (not including this header) */
    uint32_t calc_crc;
};
/**
 * \struct tfm_runtime_data
 *
 * \brief Store the data for the boot SW
 */
struct tfm_runtime_data {
    struct runtime_shared_data_tlv_header header;
    uint8_t data[];
};

#define RUNTIME_SHARED_DATA_ENTRY_HEADER_SIZE  \
        sizeof(struct runtime_shared_data_tlv_entry)
#define RUNTIME_SHARED_DATA_ENTRY_SIZE(size) \
        (size + RUNTIME_SHARED_DATA_ENTRY_HEADER_SIZE)

/**
 * \brief                     Adds data to shared area from runtime to boot
 *
 * \return                    0 on success, non-zero on error.
 */
int runtime_add_data_to_shared_area(uint8_t        major_type,
                                    uint16_t       minor_type,
                                    const uint8_t *data,
                                    size_t         size);

int get_runtime_shared_data(uint8_t  major_type,
                            uint16_t minor_type,
                            uint8_t  *data,
                            size_t   data_len);


#ifdef __cplusplus
}
#endif

#endif /* __RUNTIME_SHARED_DATA_H__ */
