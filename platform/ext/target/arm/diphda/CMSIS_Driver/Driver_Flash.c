/*
 * Copyright (c) 2013-2021 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <stdint.h>
#include "Driver_Flash.h"
#include "RTE_Device.h"
#include "platform_base_address.h"
#include "flash_layout.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

#define FLASH0_BASE           FLASH_BASE_ADDRESS
#define FLASH0_SIZE           0x02000000 /* 32 MB */
#define FLASH0_SECTOR_SIZE    0x00001000 /* 4 kB */
#define FLASH0_PAGE_SIZE      256
#define FLASH0_PROGRAM_UNIT   1          /* Minimum write size */

#define FLASH1_BASE           (SRAM_BASE + SRAM_SIZE)
#define FLASH1_SIZE           0x00009000 /* 36 KiB */
#define FLASH1_SECTOR_SIZE    0x00000400 /* 1 kB */
#define FLASH1_PAGE_SIZE      256
#define FLASH1_PROGRAM_UNIT   1          /* Minimum write size */

/* Driver version */
#define ARM_FLASH_DRV_VERSION      ARM_DRIVER_VERSION_MAJOR_MINOR(1, 1)
#define ARM_FLASH_DRV_ERASE_VALUE  0xFF

struct arm_flash_dev_t {
    const uint32_t memory_base;   /*!< FLASH memory base address */
    ARM_FLASH_INFO *data;         /*!< FLASH data */
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus = {0, 0, 0};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    0, /* event_ready */
    2, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};

static int32_t is_range_valid(struct arm_flash_dev_t *flash_dev,
                              uint32_t offset)
{
    uint32_t flash_limit = 0;
    int32_t rc = 0;

    flash_limit = (flash_dev->data->sector_count * flash_dev->data->sector_size)
                   - 1;

    if (offset > flash_limit) {
        rc = -1;
    }
    return rc;
}

static int32_t is_write_aligned(struct arm_flash_dev_t *flash_dev,
                                uint32_t param)
{
    int32_t rc = 0;

    if ((param % flash_dev->data->program_unit) != 0) {
        rc = -1;
    }
    return rc;
}

static int32_t is_sector_aligned(struct arm_flash_dev_t *flash_dev,
                                 uint32_t offset)
{
    int32_t rc = 0;

    if ((offset % flash_dev->data->sector_size) != 0) {
        rc = -1;
    }
    return rc;
}

static int32_t is_flash_ready_to_write(const uint8_t *start_addr, uint32_t cnt)
{
    int32_t rc = 0;
    uint32_t i;

    for (i = 0; i < cnt; i++) {
        if(start_addr[i] != ARM_FLASH_DRV_ERASE_VALUE) {
            rc = -1;
            break;
        }
    }

    return rc;
}

/*
 * Common interface functions
 */

static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch (state) {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
        break;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

#if (RTE_FLASH0)
static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH0_SIZE / FLASH0_SECTOR_SIZE,
    .sector_size  = FLASH0_SECTOR_SIZE,
    .page_size    = FLASH0_PAGE_SIZE,
    .program_unit = FLASH0_PROGRAM_UNIT,
    .erased_value = ARM_FLASH_DRV_ERASE_VALUE};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
    .memory_base = FLASH0_BASE,
    .data        = &(ARM_FLASH0_DEV_DATA)};

struct arm_flash_dev_t *FLASH0_DEV = &ARM_FLASH0_DEV;

/*
 * Functions
 */

static int32_t FLASH0_ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t start_addr = FLASH0_DEV->memory_base + addr;
    int32_t rc = 0;

    /* Check flash memory boundaries */
    rc = is_range_valid(FLASH0_DEV, addr + cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    memcpy(data, (void *)start_addr, cnt);
    return ARM_DRIVER_OK;
}

static int32_t FLASH0_ARM_Flash_ProgramData(uint32_t addr, const void *data,
                                            uint32_t cnt)
{
    ARG_UNUSED(addr);
    ARG_UNUSED(data);
    ARG_UNUSED(cnt);

    /* This flash device cannot be programmed over this interface. */
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static int32_t FLASH0_ARM_Flash_EraseSector(uint32_t addr)
{
    ARG_UNUSED(addr);

    /* This flash device cannot be erased over this interface, but this
     * function must return with success. Only the bootloader used this driver,
     * and it might try do delete a bad firmware image from memory. It is not a
     * problem if the image not deleted, but returning with error would crash
     * the bootloader. */
    return ARM_DRIVER_OK;
}

static int32_t FLASH0_ARM_Flash_EraseChip(void)
{
    /* This flash device cannot be erased over this interface, but this
     * function must return with success. Only the bootloader used this driver,
     * and it might try do delete a bad firmware image from memory. It is not a
     * problem if the image not deleted, but returning with error would crash
     * the bootloader. */
    return ARM_DRIVER_OK;
}

static ARM_FLASH_INFO * FLASH0_ARM_Flash_GetInfo(void)
{
    return FLASH0_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH0 = {
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    FLASH0_ARM_Flash_ReadData,
    FLASH0_ARM_Flash_ProgramData,
    FLASH0_ARM_Flash_EraseSector,
    FLASH0_ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    FLASH0_ARM_Flash_GetInfo
};
#endif /* RTE_FLASH0 */

#if (RTE_FLASH1)
static ARM_FLASH_INFO ARM_FLASH1_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH1_SIZE / FLASH1_SECTOR_SIZE,
    .sector_size  = FLASH1_SECTOR_SIZE,
    .page_size    = FLASH1_PAGE_SIZE,
    .program_unit = FLASH1_PROGRAM_UNIT,
    .erased_value = ARM_FLASH_DRV_ERASE_VALUE};

static struct arm_flash_dev_t ARM_FLASH1_DEV = {
    .memory_base = FLASH1_BASE,
    .data        = &(ARM_FLASH1_DEV_DATA)};

struct arm_flash_dev_t *FLASH1_DEV = &ARM_FLASH1_DEV;

/*
 * Functions
 */

static int32_t FLASH1_ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    uint32_t start_addr = FLASH1_DEV->memory_base + addr;
    int32_t rc = 0;

    /* Check flash memory boundaries */
    rc = is_range_valid(FLASH1_DEV, addr + cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    memcpy(data, (void *)start_addr, cnt);
    return ARM_DRIVER_OK;
}

static int32_t FLASH1_ARM_Flash_ProgramData(uint32_t addr, const void *data,
                                            uint32_t cnt)
{
    uint32_t start_addr = FLASH1_DEV->memory_base + addr;
    int32_t rc = 0;

    /* Check flash memory boundaries and alignment with minimal write size */
    rc  = is_range_valid(FLASH1_DEV, addr + cnt);
    rc |= is_write_aligned(FLASH1_DEV, addr);
    rc |= is_write_aligned(FLASH1_DEV, cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Check if the flash area to write the data was erased previously */
    rc = is_flash_ready_to_write((const uint8_t*)start_addr, cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR;
    }

    memcpy((void *)start_addr, data, cnt);
    return ARM_DRIVER_OK;
}

static int32_t FLASH1_ARM_Flash_EraseSector(uint32_t addr)
{
    uint32_t start_addr = FLASH1_DEV->memory_base + addr;
    uint32_t rc = 0;

    rc  = is_range_valid(FLASH1_DEV, addr);
    rc |= is_sector_aligned(FLASH1_DEV, addr);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    memset((void *)start_addr,
           FLASH1_DEV->data->erased_value,
           FLASH1_DEV->data->sector_size);
    return ARM_DRIVER_OK;
}

static int32_t FLASH1_ARM_Flash_EraseChip(void)
{
    uint32_t i;
    uint32_t addr = FLASH1_DEV->memory_base;
    int32_t rc = ARM_DRIVER_ERROR_UNSUPPORTED;

    /* Check driver capability erase_chip bit */
    if (DriverCapabilities.erase_chip == 1) {
        for (i = 0; i < FLASH1_DEV->data->sector_count; i++) {
            memset((void *)addr,
                   FLASH1_DEV->data->erased_value,
                   FLASH1_DEV->data->sector_size);

            addr += FLASH1_DEV->data->sector_size;
            rc = ARM_DRIVER_OK;
        }
    }
    return rc;
}

static ARM_FLASH_INFO * FLASH1_ARM_Flash_GetInfo(void)
{
    return FLASH1_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH1 = {
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    FLASH1_ARM_Flash_ReadData,
    FLASH1_ARM_Flash_ProgramData,
    FLASH1_ARM_Flash_EraseSector,
    FLASH1_ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    FLASH1_ARM_Flash_GetInfo
};
#endif /* RTE_FLASH1 */
