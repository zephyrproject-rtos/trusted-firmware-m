/*
 * Copyright (c) 2013-2023 Arm Limited. All rights reserved.
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
#include "host_base_address.h"
#include "flash_layout.h"
#include "cmsis_driver_config.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION      ARM_DRIVER_VERSION_MAJOR_MINOR(1, 1)
#define ARM_FLASH_DRV_ERASE_VALUE  0xFF


/**
 * Data width values for ARM_FLASH_CAPABILITIES::data_width
 * \ref ARM_FLASH_CAPABILITIES
 */
 enum {
    DATA_WIDTH_8BIT   = 0u,
    DATA_WIDTH_16BIT,
    DATA_WIDTH_32BIT,
    DATA_WIDTH_ENUM_SIZE
};

static const uint32_t data_width_byte[DATA_WIDTH_ENUM_SIZE] = {
    sizeof(uint8_t),
    sizeof(uint16_t),
    sizeof(uint32_t),
};

/*
 * ARM FLASH device structure
 */
struct arm_flash_dev_t {
    struct cfi_strataflashj3_dev_t *dev;         /*!< FLASH memory device structure */
    ARM_FLASH_INFO *data;         /*!< FLASH data */
};
/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES STRATAFLASHJ3DriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus = {0, 0, 0};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};


/*
 * Common interface functions
 */

static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
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
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = HOST_ACCESS_SIZE / 0x1000,
    .sector_size    = 0x1000,
    .page_size      = 256U,
    .program_unit   = 1U,
    .erased_value   = ARM_FLASH_DRV_ERASE_VALUE
};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
    .dev    = &FLASH0_DEV,
    .data   = &(ARM_FLASH0_DEV_DATA)
};
#endif /* RTE_FLASH0 */

/*
 * Functions
 */
static int32_t ARM_Flash_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

int32_t Select_XIP_Mode_For_Shared_Flash(void)
{
    return ARM_DRIVER_OK;
}

int32_t Select_Write_Mode_For_Shared_Flash(void)
{
    return ARM_DRIVER_OK;
}

static int32_t STRATAFLASHJ3_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);
    enum strataflashj3_error_t ret;
    struct cfi_strataflashj3_dev_t* dev = ARM_FLASH0_DEV.dev;
    ARM_FLASH_INFO* data = ARM_FLASH0_DEV.data;

    dev->total_sector_cnt = data->sector_count;
    dev->page_size = data->page_size;
    dev->sector_size = data->sector_size;
    dev->program_unit = data->program_unit;


    ret = cfi_strataflashj3_initialize(ARM_FLASH0_DEV.dev);

    if (ret != STRATAFLASHJ3_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static ARM_FLASH_CAPABILITIES STRATAFLASHJ3_Driver_GetCapabilities(void)
{
    return STRATAFLASHJ3DriverCapabilities;
}

static int32_t STRATAFLASHJ3_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    enum strataflashj3_error_t ret;

    if (STRATAFLASHJ3DriverCapabilities.data_width > 2 || STRATAFLASHJ3DriverCapabilities.data_width < 0)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Conversion between data items and bytes */
    cnt *= data_width_byte[STRATAFLASHJ3DriverCapabilities.data_width];

    ret = cfi_strataflashj3_read(ARM_FLASH0_DEV.dev, addr, data, cnt);
    if (ret != STRATAFLASHJ3_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    /* Conversion between bytes and data items */
    cnt /= data_width_byte[STRATAFLASHJ3DriverCapabilities.data_width];

    return cnt;
}

static int32_t STRATAFLASHJ3_Flash_ProgramData(uint32_t addr, const void *data,
                                     uint32_t cnt)
{
    enum strataflashj3_error_t ret;
    
    if (STRATAFLASHJ3DriverCapabilities.data_width > 2 || STRATAFLASHJ3DriverCapabilities.data_width < 0)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Conversion between data items and bytes */
    cnt *= data_width_byte[STRATAFLASHJ3DriverCapabilities.data_width];

    ret = cfi_strataflashj3_program(ARM_FLASH0_DEV.dev, addr, data, cnt);
    if (ret != STRATAFLASHJ3_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    /* Conversion between bytes and data items */
    cnt /= data_width_byte[STRATAFLASHJ3DriverCapabilities.data_width];

    return cnt;
}

static int32_t STRATAFLASHJ3_Flash_EraseSector(uint32_t addr)
{
    enum strataflashj3_error_t ret;

    ret = cfi_strataflashj3_erase(ARM_FLASH0_DEV.dev, addr);
    if (ret != STRATAFLASHJ3_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t STRATAFLASHJ3_Flash_EraseChip(void)
{
    enum strataflashj3_error_t ret;

    ret = cfi_strataflashj3_erase_chip(ARM_FLASH0_DEV.dev);
    if (ret != STRATAFLASHJ3_ERR_NONE) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static ARM_FLASH_INFO * STRATAFLASHJ3_Flash_GetInfo(void)
{
    return &ARM_FLASH0_DEV_DATA;
}

ARM_DRIVER_FLASH Driver_FLASH0 = {
    ARM_Flash_GetVersion,
    STRATAFLASHJ3_Driver_GetCapabilities,
    STRATAFLASHJ3_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    STRATAFLASHJ3_Flash_ReadData,
    STRATAFLASHJ3_Flash_ProgramData,
    STRATAFLASHJ3_Flash_EraseSector,
    STRATAFLASHJ3_Flash_EraseChip,
    ARM_Flash_GetStatus,
    STRATAFLASHJ3_Flash_GetInfo
};
