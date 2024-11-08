/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "fwu_metadata.h"

#include <stdint.h>
#include <string.h>
#include "atu_rse_drv.h"
#include "cmsis_compiler.h"
#include "device_definition.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "fwu_config.h"
#include "gpt.h"
#include "host_base_address.h"
#include "platform_base_address.h"
#include "soft_crc.h"

#define RSE_ATU_REGION_TEMP_SLOT           2
#define BANK_INVALID    0xffu  /* One or more images corrupted or partially written */
#define BANK_VALID      0xfeu  /* Valid images, but some are in unaccepted state */
#define BANK_ACCEPTED   0xfcu  /* All images valid and accepted */
#define METADATA_SIZE   FLASH_LBA_SIZE

extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

/* DEN0118 Metadata version 2 */
struct metadata_header_t {
    uint32_t crc_32;        /* Metadata crc value */
    uint32_t version;       /* Metadata version, must be 2 for this type */
    uint32_t active_index;  /* Bank index from which device boots   */
    uint32_t previous_active_index; /* Previous successfully booted bank index */
    uint32_t metadata_size; /* Size in bytes of complete metadata structure */
    uint16_t descriptor_offset; /* The offset, from the start of this data structure,
                                 * where the fw_store_desc starts (if one exists).
                                 * If the fw_store_desc does not exist, then this
                                 * field must be set to 0.
                                 */
    uint16_t reserved1;
    uint8_t bank_state[4];  /* The state of each bank */
    uint32_t reserved2;
    /* OPTIONAL fw_store_desc: not used */
} __PACKED;

/* Valid entries for data item width */
static const uint32_t data_width_byte[] = {
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
};

static int read_fwu_metadadata(struct metadata_header_t *md_header,
                               uint8_t *metadata,
                               uint64_t offset)
{
    int rc;
    enum atu_error_t atu_err;
    uint32_t log_addr = HOST_FLASH0_TEMP_BASE_S;
    uint64_t physical_address = HOST_FLASH0_BASE + offset;

    ARM_FLASH_CAPABILITIES DriverCapabilities =
        FLASH_DEV_NAME.GetCapabilities();
    uint8_t data_width = data_width_byte[DriverCapabilities.data_width];

    if ((sizeof(struct metadata_header_t) / data_width) > (UINT32_MAX - offset)) {
        return -1;
    }

    atu_err = atu_initialize_region(&ATU_DEV_S,
                                    RSE_ATU_REGION_TEMP_SLOT,
                                    log_addr,
                                    physical_address,
                                    0x2000);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    /* Read entire metdata */
    rc = FLASH_DEV_NAME.ReadData(log_addr - FLASH_BASE_ADDRESS, (void *)metadata,
                                 FLASH_LBA_SIZE / data_width);
    if (rc != (FLASH_LBA_SIZE / data_width)) {
        return -1;
    }

    /* Header is at the beginning of the metadata */
    memcpy(md_header, metadata, sizeof(struct metadata_header_t));

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSE_ATU_REGION_TEMP_SLOT);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    return 0;
}

int parse_fwu_metadata(uint64_t md_offset)
{
    int rc;
    struct metadata_header_t metadata_header;
    static uint8_t metadata[METADATA_SIZE];
    uint32_t calc_crc, active_index;

    rc = read_fwu_metadadata(&metadata_header,
                             metadata,
                             md_offset);
    if (rc) {
        return -1;
    }

    /* DEN0118 A3.2.4 Metadata Integrity check for version 2 */
    calc_crc = crc32((const uint32_t *)&metadata[sizeof(metadata_header.crc_32)],
                     metadata_header.metadata_size - sizeof(metadata_header.crc_32));

    if (metadata_header.crc_32 != calc_crc) {
        return -1;
    }

    active_index = metadata_header.active_index;
    if (active_index >= FWU_BANK_COUNT) {
        return -1;
    }

    if ((metadata_header.bank_state[active_index] == BANK_VALID) ||
        (metadata_header.bank_state[active_index] == BANK_ACCEPTED)) {
        // Images should load from current bank
        return 0;
    } else {
        //TODO: One or more images corrupted or partially written
    }

    return -1;
}
