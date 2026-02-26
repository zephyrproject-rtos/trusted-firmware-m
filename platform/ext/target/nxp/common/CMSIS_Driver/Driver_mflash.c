/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 * Copyright 2026 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Driver_Flash.h"
#include "platform_base_address.h"
#include "flash_layout.h"
#include "fsl_common.h"
#include "mflash_drv.h"

#define TARGET_DEBUG_LOG 0
#if TARGET_DEBUG_LOG
#include "tfm_spm_log.h"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

/**
 * \brief Flash driver capability macro definitions \ref ARM_FLASH_CAPABILITIES
 */
/* Flash Ready event generation capability values */
#define EVENT_READY_NOT_AVAILABLE   (0u)
#define EVENT_READY_AVAILABLE       (1u)

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Structures
 ******************************************************************************/

/* Data access size values */
 enum {
    DATA_WIDTH_8BIT   = 0u,
    DATA_WIDTH_16BIT,
    DATA_WIDTH_32BIT,
    DATA_WIDTH_ENUM_SIZE
};

/* Chip erase capability values */
#define CHIP_ERASE_NOT_SUPPORTED    (0u)
#define CHIP_ERASE_SUPPORTED        (1u)

/* ARM FLASH device structure */
struct arm_flash_dev_t
{
    ARM_FLASH_INFO *data;         /*!< FLASH data */
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus = {0, 0, 0};

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion =
{
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities =
{
    EVENT_READY_NOT_AVAILABLE,
    DATA_WIDTH_8BIT,
    CHIP_ERASE_SUPPORTED
};

static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA =
{
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH0_SIZE / FLASH0_SECTOR_SIZE,
    .sector_size  = FLASH0_SECTOR_SIZE,
    .page_size    = FLASH0_PAGE_SIZE,
    .program_unit = FLASH0_PROGRAM_UNIT,
    .erased_value = 0xFF};

static struct arm_flash_dev_t ARM_FLASH0_DEV =
{
    .data        = &(ARM_FLASH0_DEV_DATA)
};

static struct arm_flash_dev_t *FLASH0_DEV = &ARM_FLASH0_DEV;

/* Prototypes */
static bool is_range_valid(struct arm_flash_dev_t *flash_dev,
                           uint32_t offset);
static bool is_write_aligned(struct arm_flash_dev_t *flash_dev,
                             uint32_t param);

/* Functions */
static ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static bool flash_init_is_done = false;
static int32_t ARM_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);

    status_t status;

    if (flash_init_is_done == false)
    {
        if (DriverCapabilities.data_width >= DATA_WIDTH_ENUM_SIZE)
        {
            return ARM_DRIVER_ERROR;
        }

        /* Driver init call*/
        status = mflash_drv_init();
        if(status != kStatus_Success)
        {
            return ARM_DRIVER_ERROR;
        }

        /* Disable Error Detection functionality*/
        flash_init_is_done = true;
    }
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_Uninitialize(void)
{
    flash_init_is_done = false;
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch (state)
    {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    static uint32_t status;

    /* Check Flash memory boundaries */
    status = is_range_valid(FLASH0_DEV, addr + cnt);
    if(status != kStatus_Success)
    {
#if TARGET_DEBUG_LOG
        SPMLOG_INFMSGVAL("ARM_Flash_ReadData addr:",addr);
        SPMLOG_INFMSGVAL("ARM_Flash_ReadData cnt:",cnt);
        SPMLOG_INFMSGVAL("ARM_Flash_ReadData status:",status);
        SPMLOG_DBGMSG("\r\n***NOR Flash Read error parameters!***\r\n");
#endif
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Read Data */
    (void)memcpy(data, (void *)(MFLASH_BASE_ADDRESS + addr), cnt);

    return ARM_DRIVER_OK;;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data, uint32_t cnt)
{
    static uint32_t status;
    uint32_t        pages;
    uint32_t        i;
    const uint32_t  *input_data;
    bool            data_is_aligned;
    uint32_t        src_addr = addr;
    const void      *data_p = data;
    /* Aligned temporary buffer of 4 bytes.*/
    static uint32_t write_buffer[FLASH0_PAGE_SIZE/sizeof(uint32_t)];
    static uint32_t write_buffer_pharase[FLASH0_PROGRAM_UNIT/sizeof(uint32_t)];

    /* Check Flash memory boundaries */
    status = is_range_valid(FLASH0_DEV, addr);
    status |= is_write_aligned(FLASH0_DEV, addr);
    if(status != kStatus_Success)
    {
#if TARGET_DEBUG_LOG
        SPMLOG_INFMSGVAL("\r\nARM_Flash_ProgramData unaligned data addr:\r\n", addr);
#endif
    }

    /* Use program pharase in following case, otherwsie use program page*/
    if (TFM_HAL_FLASH_PROGRAM_UNIT <= 16)
    {
        uint32_t num_pharases = cnt / FLASH0_DEV->data->program_unit;
#if TARGET_DEBUG_LOG
        SPMLOG_INFMSGVAL("flash_program_page num_pharases:",num_pharases);
#endif
        /* evaluate align status*/
        data_is_aligned = ((uint32_t)data_p % (sizeof(uint32_t)) != 0) ? (false) : (true);

        for (i = 0; i<num_pharases ; i++, data_p = (uint8_t*)data_p + FLASH0_PROGRAM_UNIT, src_addr += FLASH0_PROGRAM_UNIT)
        {
            /* The src address must be 32-bit aligned */
            if(data_is_aligned)
            {
                input_data = data_p;
            }
            else
            {
#if TARGET_DEBUG_LOG
                SPMLOG_INFMSG("Got unaligned data:\n");
#endif
                input_data = memcpy(write_buffer_pharase, data_p, FLASH0_PROGRAM_UNIT);
            }
            /* write to flash */
            status = mflash_drv_phrase_program(src_addr, (uint32_t *)input_data);
        }
    }
    else
    {

        pages = cnt / FLASH0_DEV->data->page_size;
#if TARGET_DEBUG_LOG
        SPMLOG_INFMSGVAL("flash_program_page pages:",pages);
#endif
        /* evaluate align status*/
        data_is_aligned = ((uint32_t)data_p % (sizeof(uint32_t)) != 0) ? (false) : (true);

        /*Iterate over pages to do write*/
        for (i = 0; i < pages; i++, data_p = (uint8_t*)data_p + FLASH0_PAGE_SIZE, src_addr += FLASH0_PAGE_SIZE)
        {
            /* The src address must be 32-bit aligned */
            if(data_is_aligned)
            {
                input_data = data_p;
            }
            else
            {
#if TARGET_DEBUG_LOG
                SPMLOG_INFMSG("Got unaligned data:\n");
#endif
                input_data = memcpy(write_buffer, data_p, FLASH0_PAGE_SIZE);
            }
            /* write to flash */
            status = mflash_drv_page_program(src_addr, (uint32_t *)input_data);
        }
    }

    /* check flash write status*/
    if (status != kStatus_Success)
    {
#if TARGET_DEBUG_LOG
        SPMLOG_INFMSGVAL("flash_program_page addr:",addr);
        SPMLOG_INFMSGVAL("flash_program_page cnt:",cnt);
        SPMLOG_INFMSGVAL("ARM_Flash_ProgramData status: ",status);
#endif
        return ARM_DRIVER_ERROR;
    }

    return cnt;
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    static uint32_t status;
    status = is_range_valid(FLASH0_DEV, addr);
    status |= is_write_aligned(FLASH0_DEV, addr);
    if(status != kStatus_Success)
    {
#if TARGET_DEBUG_LOG
        SPMLOG_INFMSG("\r\nARM_Flash_EraseSector return ERROR\r\n");
#endif
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* flash erase call*/
    status = mflash_drv_sector_erase(addr);

    if (status != kStatus_Success)
    {
#if TARGET_DEBUG_LOG
        SPMLOG_INFMSGVAL("flash_erase_sector addr:",addr);
        SPMLOG_INFMSGVAL("flash_erase_sector status:",status);
        SPMLOG_INFMSG("\r\n***NOR Flash erase error parameters!***\r\n");
#endif
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

static ARM_FLASH_INFO * ARM_Flash_GetInfo(void)
{
    return FLASH0_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH0 =
{
    .GetVersion = ARM_Flash_GetVersion,
    .GetCapabilities = ARM_Flash_GetCapabilities,
    .Initialize = ARM_Flash_Initialize,
    .Uninitialize = ARM_Flash_Uninitialize,
    .PowerControl = ARM_Flash_PowerControl,
    .ReadData = ARM_Flash_ReadData,
    .ProgramData = ARM_Flash_ProgramData,
    .EraseSector = ARM_Flash_EraseSector,
    .GetStatus = ARM_Flash_GetStatus,
    .GetInfo = ARM_Flash_GetInfo
};

/**
 * \brief      Check if the Flash memory boundaries are not violated.
 * \param[in]  flash_dev  Flash device structure \ref arm_flash_dev_t
 * \param[in]  offset     Highest Flash memory address which would be accessed.
 * \return     Returns true if Flash memory boundaries are not violated, false
 *             otherwise.
 */
static bool is_range_valid(struct arm_flash_dev_t *flash_dev,
                           uint32_t offset)
{
    uint32_t flash_limit = 0;

    /* Calculating the highest address of the Flash memory address range */
    flash_limit = FLASH0_SIZE - 1;

    return (offset > flash_limit) ? (kStatus_Fail) : (kStatus_Success) ;
}

/* Check if the parameter is aligned to program_unit. */
static bool is_write_aligned(struct arm_flash_dev_t *flash_dev,
                             uint32_t param)
{
    return ((param % flash_dev->data->program_unit) != 0) ? (kStatus_Fail) : (kStatus_Success);
}
