/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <string.h>
#include "fwu_agent.h"
#include "Driver_Flash.h"
#include "flash_layout.h"
#include "fip_parser/external/uuid.h"

/* Properties of image in a bank */
struct fwu_image_properties {

        /* UUID of the image in this bank */
        uuid_t img_uuid;

        /* [0]: bit describing the image acceptance status –
         *      1 means the image is accepted
         * [31:1]: MBZ
         */
        uint32_t accepted;

        /* NOTE: using the reserved field */
        /* image version */
        uint32_t version;

} __packed;

/* Image entry information */
struct fwu_image_entry {

        /* UUID identifying the image type */
        uuid_t img_type_uuid;

        /* UUID of the storage volume where the image is located */
        uuid_t location_uuid;

        /* Properties of images with img_type_uuid in the different FW banks */
        struct fwu_image_properties img_props[NR_OF_FW_BANKS];

} __packed;

struct fwu_metadata {

        /* Metadata CRC value */
        uint32_t crc_32;

        /* Metadata version */
        uint32_t version;

        /* Bank index with which device boots */
        uint32_t active_index;

        /* Previous bank index with which device booted successfully */
        uint32_t previous_active_index;

        /* Image entry information */
        struct fwu_image_entry img_entry[NR_OF_IMAGES_IN_FW_BANK];

} __packed;

/* This is Corstone1000 speific metadata for OTA.
 * Private metadata is written at next sector following
 * FWU METADATA location */
struct fwu_private_metadata {

       /* boot_index: the bank from which system is booted from */
       uint32_t boot_index;

} __packed;

struct fwu_metadata _metadata;
int is_initialized = 0;

#define IMAGE_ACCEPTED          (1)
#define IMAGE_NOT_ACCEPTED      (0)
#define BANK_0                  (0)
#define BANK_1                  (1)
#define IMAGE_0                 (0)
#define IMAGE_1                 (1)
#define IMAGE_2                 (2)
#define IMAGE_3                 (3)
#define INVALID_VERSION         (0xffffffff)

#ifndef FWU_METADATA_FLASH_DEV
    #ifndef FLASH_DEV_NAME
    #error "FWU_METADATA_FLASH_DEV or FLASH_DEV_NAME must be defined in flash_layout.h"
    #else
    #define FWU_METADATA_FLASH_DEV FLASH_DEV_NAME
    #endif
#endif

/* Import the CMSIS flash device driver */
extern ARM_DRIVER_FLASH FWU_METADATA_FLASH_DEV;

static enum fwu_agent_error_t private_metadata_read(
        struct fwu_private_metadata* p_metadata)
{
    int ret;

    FWU_LOG_MSG("%s: enter\n\r", __func__);

    if (!p_metadata) {
        return FWU_AGENT_ERROR;
    }

    ret = FWU_METADATA_FLASH_DEV.ReadData(FWU_PRIVATE_AREA_OFFSET, p_metadata,
                                          sizeof(struct fwu_private_metadata));
    if (ret != ARM_DRIVER_OK) {
        return FWU_AGENT_ERROR;
    }

    FWU_LOG_MSG("%s: success: boot_index = %u\n\r", __func__,
                        p_metadata->boot_index);

    return FWU_AGENT_SUCCESS;
}

static enum fwu_agent_error_t private_metadata_write(
        struct fwu_private_metadata* p_metadata)
{
    int ret;

    FWU_LOG_MSG("%s: enter: boot_index = %u\n\r", __func__,
                        p_metadata->boot_index);

    if (!p_metadata) {
        return FWU_AGENT_ERROR;
    }

    ret = FWU_METADATA_FLASH_DEV.EraseSector(FWU_PRIVATE_AREA_OFFSET);
    if (ret != ARM_DRIVER_OK) {
        return FWU_AGENT_ERROR;
    }

    ret = FWU_METADATA_FLASH_DEV.ProgramData(FWU_PRIVATE_AREA_OFFSET,
                                p_metadata, sizeof(struct fwu_private_metadata));
    if (ret != ARM_DRIVER_OK) {
        return FWU_AGENT_ERROR;
    }

    FWU_LOG_MSG("%s: success\n\r", __func__);
    return FWU_AGENT_SUCCESS;
}

static enum fwu_agent_error_t metadata_read(struct fwu_metadata *p_metadata)
{
    int ret;

    FWU_LOG_MSG("%s: enter: flash addr = %u, size = %d\n\r", __func__,
                  FWU_METADATA_REPLICA_1_OFFSET, sizeof(struct fwu_metadata));

    if (!p_metadata) {
        return FWU_AGENT_ERROR;
    }

    ret = FWU_METADATA_FLASH_DEV.ReadData(FWU_METADATA_REPLICA_1_OFFSET,
                                p_metadata, sizeof(struct fwu_metadata));
    if (ret != ARM_DRIVER_OK) {
        return FWU_AGENT_ERROR;
    }

    FWU_LOG_MSG("%s: success: active = %u, previous = %d\n\r", __func__,
                  p_metadata->active_index, p_metadata->previous_active_index);

    return FWU_AGENT_SUCCESS;
}

static enum fwu_agent_error_t metadata_write(
                        struct fwu_metadata *p_metadata)
{
    int ret;

    FWU_LOG_MSG("%s: enter: flash addr = %u, size = %d\n\r", __func__,
                  FWU_METADATA_REPLICA_1_OFFSET, sizeof(struct fwu_metadata));

    if (!p_metadata) {
        return FWU_AGENT_ERROR;
    }

    ret = FWU_METADATA_FLASH_DEV.EraseSector(FWU_METADATA_REPLICA_1_OFFSET);
    if (ret != ARM_DRIVER_OK) {
        return FWU_AGENT_ERROR;
    }

    ret = FWU_METADATA_FLASH_DEV.ProgramData(FWU_METADATA_REPLICA_1_OFFSET,
                                p_metadata, sizeof(struct fwu_metadata));
    if (ret != ARM_DRIVER_OK) {
        return FWU_AGENT_ERROR;
    }

    FWU_LOG_MSG("%s: success: active = %u, previous = %d\n\r", __func__,
                  p_metadata->active_index, p_metadata->previous_active_index);
    return FWU_AGENT_SUCCESS;
}

enum fwu_agent_error_t fwu_metadata_init(void)
{
    enum fwu_agent_error_t ret;
    ARM_FLASH_INFO* flash_info;

    FWU_LOG_MSG("%s: enter\n\r", __func__);

    if (is_initialized) {
        return FWU_AGENT_SUCCESS;
    }

    /* Code assumes everything fits into a sector */
    if (sizeof(struct fwu_metadata) > FWU_METADATA_FLASH_SECTOR_SIZE) {
        return FWU_AGENT_ERROR;
    }

    if (sizeof(struct fwu_private_metadata) > FWU_METADATA_FLASH_SECTOR_SIZE) {
        return FWU_AGENT_ERROR;
    }

    ret = FWU_METADATA_FLASH_DEV.Initialize(NULL);
    if (ret != ARM_DRIVER_OK) {
        return FWU_AGENT_ERROR;
    }

    flash_info = FWU_METADATA_FLASH_DEV.GetInfo();
    if (flash_info->program_unit != 1) {
        FWU_METADATA_FLASH_DEV.Uninitialize();
        return FWU_AGENT_ERROR;
    }

    is_initialized = 1;

    FWU_LOG_MSG("%s: is_initialized = %d\n\r", __func__, is_initialized);

    return FWU_AGENT_SUCCESS;
}

enum fwu_agent_error_t fwu_metadata_provision(void)
{
    enum fwu_agent_error_t ret;
    struct fwu_private_metadata priv_metadata;
    uint32_t image_version = 0;

    FWU_LOG_MSG("%s: enter\n\r", __func__);

    ret = fwu_metadata_init();
    if (ret) {
        return ret;
    }

    /* Provision FWU Agent Metadata */

    memset(&_metadata, 0, sizeof(struct fwu_metadata));

    _metadata.version = 1;
    _metadata.active_index = 0;
    _metadata.previous_active_index = 1;

    /* bank 0 is the place where images are located at the
     * start of device lifecycle */

    for (int i = 0; i < NR_OF_IMAGES_IN_FW_BANK; i++) {

        _metadata.img_entry[i].img_props[BANK_0].accepted = IMAGE_ACCEPTED;
        _metadata.img_entry[i].img_props[BANK_0].version = image_version;

        _metadata.img_entry[i].img_props[BANK_1].accepted = IMAGE_NOT_ACCEPTED;
        _metadata.img_entry[i].img_props[BANK_1].version = INVALID_VERSION;
    }

    ret = metadata_write(&_metadata);
    if (ret) {
        return ret;
    }

    memset(&_metadata, 0, sizeof(struct fwu_metadata));
    ret = metadata_read(&_metadata);
    if (ret) {
        return ret;
    }
    FWU_LOG_MSG("%s: provisioned values: active = %u, previous = %d\n\r",
             __func__, _metadata.active_index, _metadata.previous_active_index);


    /* Provision Private metadata for update agent which is shared
       beween BL1 and tf-m of secure enclave */

    memset(&priv_metadata, 0, sizeof(struct fwu_private_metadata));

    priv_metadata.boot_index = BANK_0;

    ret = private_metadata_write(&priv_metadata);
    if (ret) {
        return ret;
    }

    memset(&priv_metadata, 0, sizeof(struct fwu_private_metadata));
    ret = private_metadata_read(&priv_metadata);
    if (ret) {
        return ret;
    }
    FWU_LOG_MSG("%s: provisioned values: boot_index = %u\n\r", __func__,
                        priv_metadata.boot_index);

    FWU_LOG_MSG("%s: FWU METADATA PROVISIONED.\n\r", __func__);
    return FWU_AGENT_SUCCESS;
}

