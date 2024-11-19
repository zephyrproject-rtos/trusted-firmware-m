/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "fwu_metadata.h"

#include <stdbool.h>
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
#define PRIVATE_METADATA_SIZE   FLASH_LBA_SIZE

#define METADATA_REGION_SIZE            0x2000
#define PRIVATE_METADATA_REGION_SIZE    0x2000

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

struct fwu_private_metadata_t {
    uint32_t failed_boot_count; /* Counter tracking number of failed boots */
    uint32_t nv_counter[FWU_BANK_COUNT]; /* Staged nv counter; before written to OTP */
} __PACKED;

/* Valid entries for data item width */
static const uint32_t data_width_byte[] = {
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
};

static int read_entire_region_from_host_flash(uint64_t offset,
                                              uint32_t region_size,
                                              uint8_t *data_buf)
{
    int rc;
    enum atu_error_t atu_err;
    uint32_t log_addr = HOST_FLASH0_TEMP_BASE_S;
    uint64_t physical_address = HOST_FLASH0_BASE + offset;

    ARM_FLASH_CAPABILITIES DriverCapabilities =
        FLASH_DEV_NAME.GetCapabilities();
    uint8_t data_width = data_width_byte[DriverCapabilities.data_width];

    if ((region_size / data_width) > (UINT32_MAX - offset)) {
        return -1;
    }

    atu_err = atu_initialize_region(&ATU_DEV_S,
                                    RSE_ATU_REGION_TEMP_SLOT,
                                    log_addr,
                                    physical_address,
                                    region_size);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    /* Read entire metadata */
    rc = FLASH_DEV_NAME.ReadData(log_addr - FLASH_BASE_ADDRESS, (void *)data_buf,
                                 FLASH_LBA_SIZE / data_width);
    if (rc != (FLASH_LBA_SIZE / data_width)) {
        return -1;
    }

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSE_ATU_REGION_TEMP_SLOT);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    return 0;
}

static int read_fwu_metadadata(struct metadata_header_t *md_header,
                               uint8_t *metadata,
                               uint64_t offset)
{
    int rc;

    rc = read_entire_region_from_host_flash(offset,
                                            METADATA_REGION_SIZE,
                                            metadata);
    if (rc != 0) {
        return rc;
    }

    /* Header is at the beginning of the metadata */
    memcpy(md_header, metadata, sizeof(struct metadata_header_t));

    return 0;
}

static int read_fwu_private_metadadata(
                                struct fwu_private_metadata_t *private_metadata,
                                uint8_t *private_metadata_buf,
                                uint64_t offset)
{
    int rc;

    rc = read_entire_region_from_host_flash(offset,
                                            PRIVATE_METADATA_REGION_SIZE,
                                            private_metadata_buf);
    if (rc != 0) {
        return rc;
    }

    memcpy(private_metadata, private_metadata_buf,
           sizeof(struct fwu_private_metadata_t));

    return 0;
}

static int write_fwu_private_metadadata(
                                struct fwu_private_metadata_t *private_metadata,
                                uint64_t offset)
{

    int rc;
    enum atu_error_t atu_err;
    uint32_t log_addr = HOST_FLASH0_TEMP_BASE_S;
    uint64_t physical_address = HOST_FLASH0_BASE + offset;

    ARM_FLASH_CAPABILITIES DriverCapabilities =
        FLASH_DEV_NAME.GetCapabilities();
    uint8_t data_width = data_width_byte[DriverCapabilities.data_width];

    if ((offset + PRIVATE_METADATA_REGION_SIZE) > HOST_FLASH0_SIZE) {
        return -1;
    }

    atu_err = atu_initialize_region(&ATU_DEV_S,
                                    RSE_ATU_REGION_TEMP_SLOT,
                                    log_addr,
                                    physical_address,
                                    PRIVATE_METADATA_REGION_SIZE);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    rc = FLASH_DEV_NAME.EraseSector(log_addr - FLASH_BASE_ADDRESS);
    if (rc != ARM_DRIVER_OK) {
        return -1;
    }

    /* Write private metadata */
    rc = FLASH_DEV_NAME.ProgramData(log_addr - FLASH_BASE_ADDRESS,
                                    (void *)private_metadata,
                                    sizeof(*private_metadata));
    if (rc < 0 || rc != sizeof(*private_metadata)) {
        return -1;
    }

    atu_err = atu_uninitialize_region(&ATU_DEV_S,
                                      RSE_ATU_REGION_TEMP_SLOT);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    return 0;
}

int parse_fwu_metadata(uint64_t md_offset,
                       uint64_t private_md_offset,
                       bool increment_failed_boot,
                       uint8_t *boot_index)
{
    int rc;
    struct metadata_header_t metadata_header;
    struct fwu_private_metadata_t private_md;
    static uint8_t metadata[METADATA_SIZE];
    static uint8_t private_metadata[PRIVATE_METADATA_SIZE];
    uint32_t calc_crc, active_index, previous_active_index;

    rc = read_fwu_metadadata(&metadata_header,
                             metadata,
                             md_offset);
    if (rc != 0) {
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

    previous_active_index = metadata_header.previous_active_index;
    if (previous_active_index >= FWU_BANK_COUNT) {
        return -1;
    }

    /* Read private metadata */
    rc = read_fwu_private_metadadata(&private_md,
                                     private_metadata,
                                     private_md_offset);
    if (rc != 0) {
        return -1;
    }

    if (increment_failed_boot) {
        /* Increment and update failed boot count in private metadata */
        private_md.failed_boot_count++;
        rc = write_fwu_private_metadadata(&private_md, private_md_offset);
        if (rc != 0) {
            return -1;
        }
    }

    if ((private_md.failed_boot_count < FWU_MAX_FAILED_BOOT) &&
        ((metadata_header.bank_state[active_index] == BANK_VALID) ||
         (metadata_header.bank_state[active_index] == BANK_ACCEPTED))) {
        /* Images should load from current bank */
        *boot_index = active_index;

    } else if ((active_index != previous_active_index) &&
               ((metadata_header.bank_state[previous_active_index] == BANK_VALID) ||
                (metadata_header.bank_state[previous_active_index] == BANK_ACCEPTED))) {
        /* Boot from previous active bank */
        *boot_index = previous_active_index;

    } else {
        //TODO: One or more images corrupted or partially written
        return -1;
    }

    return 0;
}
