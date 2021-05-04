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
#include "cmsis_driver_config.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION      ARM_DRIVER_VERSION_MAJOR_MINOR(1, 1)
#define ARM_FLASH_DRV_ERASE_VALUE  0xFF

/*
 * ARM FLASH device structure
 */
struct arm_flash_dev_t {
    struct spi_n25q256a_dev_t *dev;         /*!< FLASH memory device structure */
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
static const ARM_FLASH_CAPABILITIES N25Q256ADriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    1  /* erase_chip */
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
    .sector_count   = PMOD_SF3_FLASH_TOTAL_SIZE / PMOD_SF3_FLASH_SECTOR_SIZE,
    .sector_size    = PMOD_SF3_FLASH_SECTOR_SIZE,
    .page_size      = PMOD_SF3_FLASH_PAGE_SIZE,
    .program_unit   = PMOD_SF3_FLASH_PROGRAM_UNIT,
    .erased_value   = ARM_FLASH_DRV_ERASE_VALUE
};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
    .dev    = &FLASH0_DEV,
    .data   = &(ARM_FLASH0_DEV_DATA)
};

/*
 * Functions
 */

static ARM_FLASH_CAPABILITIES N25Q256A_Driver_GetCapabilities(void)
{
    return N25Q256ADriverCapabilities;
}

static int32_t N25Q256A_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);
    enum n25q256a_error_t ret;
    struct spi_n25q256a_dev_t* dev = ARM_FLASH0_DEV.dev;
    ARM_FLASH_INFO* data = ARM_FLASH0_DEV.data;

    dev->total_sector_cnt = data->sector_count;
    dev->page_size = data->page_size;
    dev->sector_size = data->sector_size;
    dev->program_unit = data->program_unit;

    ret = spi_n25q256a_initialize(ARM_FLASH0_DEV.dev);
    if (ret != N25Q256A_ERR_NONE) {
        SPI_FLASH_LOG_MSG("%s: Initialization failed.\n\r", __func__);
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t N25Q256A_Flash_Uninitialize(void)
{
    /* Nothing to be done */
    spi_n25q256a_uninitialize(ARM_FLASH0_DEV.dev);
    return ARM_DRIVER_OK;
}

static int32_t N25Q256A_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    enum n25q256a_error_t ret;

    ret = spi_n25q256a_read(ARM_FLASH0_DEV.dev, addr, data, cnt);
    if (ret != N25Q256A_ERR_NONE) {
        SPI_FLASH_LOG_MSG("%s: read failed: addr=0x%x, cnt=%u\n\r", __func__, addr, cnt);
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t N25Q256A_Flash_ProgramData(uint32_t addr, const void *data,
                                     uint32_t cnt)
{
    enum n25q256a_error_t ret;

    ret = spi_n25q256a_program(ARM_FLASH0_DEV.dev, addr, data, cnt);
    if (ret != N25Q256A_ERR_NONE) {
        SPI_FLASH_LOG_MSG("%s: program failed: addr=0x%x, cnt=%u\n\r", __func__, addr, cnt);
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t N25Q256A_Flash_EraseSector(uint32_t addr)
{
    enum n25q256a_error_t ret;

    ret = spi_n25q256a_erase(ARM_FLASH0_DEV.dev, addr);
    if (ret != N25Q256A_ERR_NONE) {
        SPI_FLASH_LOG_MSG("%s: erase failed: addr=0x%x\n\r", __func__, addr);
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t N25Q256A_Flash_EraseChip(void)
{
    enum n25q256a_error_t ret;

    ret = spi_n25q256a_erase_chip(ARM_FLASH0_DEV.dev);
    if (ret != N25Q256A_ERR_NONE) {
        SPI_FLASH_LOG_MSG("%s: erase chip failed\n\r", __func__);
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static ARM_FLASH_INFO * N25Q256A_Flash_GetInfo(void)
{
    return ARM_FLASH0_DEV.data;
}

ARM_DRIVER_FLASH Driver_FLASH0 = {
    ARM_Flash_GetVersion,
    N25Q256A_Driver_GetCapabilities,
    N25Q256A_Flash_Initialize,
    N25Q256A_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    N25Q256A_Flash_ReadData,
    N25Q256A_Flash_ProgramData,
    N25Q256A_Flash_EraseSector,
    N25Q256A_Flash_EraseChip,
    ARM_Flash_GetStatus,
    N25Q256A_Flash_GetInfo
};

#endif /* RTE_FLASH0 */
