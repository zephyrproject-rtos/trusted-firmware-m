/*
 * Copyright (c) 2013-2018 Arm Limited. All rights reserved.
 * Copyright (c) 2021 Laird Connectivity. All rights reserved.
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

#include <Driver_Flash.h>
#include <RTE_Device.h>
#include <flash_layout.h>
#include <string.h>
#include <nrfx_qspi.h>

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  (void)arg
#endif

#define ARM_FLASH_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)
#define QSPI_MIN_READ_SIZE       4
#define QSPI_READ_SIZE_ALIGNMENT 4

#if RTE_QSPI0

static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,
    ARM_FLASH_DRV_VERSION
};

static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    .event_ready = 0,
    .data_width  = 2, /* 32-bit */
    .erase_chip  = 0
};

static ARM_FLASH_INFO FlashInfo = {
    .sector_info  = NULL, /* Uniform sector layout */
    .sector_count = QSPI_FLASH_TOTAL_SIZE / QSPI_FLASH_AREA_IMAGE_SECTOR_SIZE,
    .sector_size  = QSPI_FLASH_AREA_IMAGE_SECTOR_SIZE,
    .page_size    = 256,
    .program_unit = 4, /* 32-bit word = 4 bytes */
    .erased_value = 0xFF
};

static const nrfx_qspi_config_t pQSPIConf = NRFX_QSPI_DEFAULT_CONFIG(RTE_QSPI0_SCL_PIN,
                                                                     RTE_QSPI0_CSN_PIN,
                                                                     RTE_QSPI0_IO0_PIN,
                                                                     RTE_QSPI0_IO1_PIN,
                                                                     RTE_QSPI0_IO2_PIN,
                                                                     RTE_QSPI0_IO3_PIN);

static bool bQSPIInit = false;

static bool is_range_valid(uint32_t addr, uint32_t cnt)
{
    if (addr > QSPI_FLASH_TOTAL_SIZE) {
        return false;
    }

    if (cnt > (QSPI_FLASH_TOTAL_SIZE - addr)) {
        return false;
    }

    return true;
}

static ARM_DRIVER_VERSION ARM_QSPI_Flash_GetVersion(void)
{
    return DriverVersion;
}

static ARM_FLASH_CAPABILITIES ARM_QSPI_Flash_GetCapabilities(void)
{
    return DriverCapabilities;
}

static int32_t ARM_QSPI_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    ARG_UNUSED(cb_event);

    if (bQSPIInit == false)
    {
        if (nrfx_qspi_init(&pQSPIConf, NULL, NULL) != NRFX_SUCCESS)
        {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        bQSPIInit = true;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_QSPI_Flash_Uninitialize(void)
{
    if (bQSPIInit == true)
    {
        nrfx_qspi_uninit();
        bQSPIInit = false;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_QSPI_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch (state) {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static int32_t ARM_QSPI_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    uint8_t nOrigCnt = 0;
    uint8_t nExtCnt = 0;
    __ALIGN(4) uint8_t baTmpRAMBuffer[QSPI_MIN_READ_SIZE];

    /* Workaround for TF-M bug T905 */
    if (cnt < QSPI_MIN_READ_SIZE)
    {
        nOrigCnt = cnt;
        cnt = QSPI_MIN_READ_SIZE;
    }
    else if ((cnt % QSPI_READ_SIZE_ALIGNMENT) != 0)
    {
        nExtCnt = cnt & (QSPI_READ_SIZE_ALIGNMENT - 1);
        cnt = cnt ^ (uint32_t)nExtCnt;
    }

    if (!is_range_valid(addr, cnt)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (nOrigCnt == 0)
    {
        if (nrfx_qspi_read(data, cnt, addr) != NRFX_SUCCESS)
        {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    }
    else
    {
        if (nrfx_qspi_read(baTmpRAMBuffer, QSPI_MIN_READ_SIZE, addr) != NRFX_SUCCESS)
        {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
        memcpy(data, baTmpRAMBuffer, nOrigCnt);
    }

    /* Workaround for TF-M bug T905 */
    if (nExtCnt != 0)
    {
        if (nrfx_qspi_read(baTmpRAMBuffer, QSPI_MIN_READ_SIZE, (addr + cnt)) != NRFX_SUCCESS)
        {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

        memcpy(data + cnt, baTmpRAMBuffer, nExtCnt);
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_QSPI_Flash_ProgramData(uint32_t addr, const void *data,
                                     uint32_t cnt)
{
    uint32_t err;

    /* Only aligned writes of full 32-bit words are allowed. */
    if ((addr % sizeof(uint32_t)) || (cnt % sizeof(uint32_t))) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (!is_range_valid(addr, cnt)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    if (!nrfx_is_in_ram(data))
    {
        /* Not in RAM, copy to RAM so it can be written to QSPI */
        __ALIGN(4) uint8_t baRAMDataBuffer[cnt];
        memcpy(baRAMDataBuffer, data, cnt);
        err = nrfx_qspi_write(baRAMDataBuffer, cnt, addr);
    }
    else
    {
        err = nrfx_qspi_write(data, cnt, addr);
    }

    if (err != NRFX_SUCCESS)
    {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_QSPI_Flash_EraseSector(uint32_t addr)
{
    nrfx_err_t err_code;

    if (!is_range_valid(addr, QSPI_FLASH_AREA_IMAGE_SECTOR_SIZE)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    err_code = nrfx_qspi_erase(NRF_QSPI_ERASE_LEN_4KB, addr);

    if (err_code != NRFX_SUCCESS) {
        return ARM_DRIVER_ERROR;
    }

    return ARM_DRIVER_OK;
}

static int32_t ARM_QSPI_Flash_EraseChip(void)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_FLASH_STATUS ARM_QSPI_Flash_GetStatus(void)
{
    ARM_FLASH_STATUS status = {
        .busy = (nrfx_qspi_mem_busy_check() == NRFX_SUCCESS)
    };

    return status;
}

static ARM_FLASH_INFO * ARM_QSPI_Flash_GetInfo(void)
{
    return &FlashInfo;
}

ARM_DRIVER_FLASH Driver_FLASH1 = {
    .GetVersion      = ARM_QSPI_Flash_GetVersion,
    .GetCapabilities = ARM_QSPI_Flash_GetCapabilities,
    .Initialize      = ARM_QSPI_Flash_Initialize,
    .Uninitialize    = ARM_QSPI_Flash_Uninitialize,
    .PowerControl    = ARM_QSPI_Flash_PowerControl,
    .ReadData        = ARM_QSPI_Flash_ReadData,
    .ProgramData     = ARM_QSPI_Flash_ProgramData,
    .EraseSector     = ARM_QSPI_Flash_EraseSector,
    .EraseChip       = ARM_QSPI_Flash_EraseChip,
    .GetStatus       = ARM_QSPI_Flash_GetStatus,
    .GetInfo         = ARM_QSPI_Flash_GetInfo
};

#endif /* RTE_QSPI0 */
