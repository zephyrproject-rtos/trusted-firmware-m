/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file    Driver_Flash_memcpy.c
 * \brief   This file implements a simple memcpy wrapper to load from
 *          external host DRAM.
 */

#include "Driver_Flash.h"
#include <string.h>

static const ARM_FLASH_CAPABILITIES Caps = {
    0u,
    0u,
    0u,
};
static ARM_FLASH_CAPABILITIES GetCapabilities(void)
{
    return Caps;
}

static const ARM_FLASH_INFO Info = {
    .erased_value = 0x00,
    .page_size = 1,
    .program_unit = 1,
    .sector_count = 1,
    .sector_info = NULL,
    .sector_size = 1,
};

static int32_t Initialize(ARM_Flash_SignalEvent_t cb)
{
    (void)cb;
    return ARM_DRIVER_OK;
}

static int32_t Uninitialize(void)
{
    return ARM_DRIVER_OK;
}

static int32_t ReadData(uint32_t offs, void *data, uint32_t cnt)
{
    memcpy(data, (void *)offs, cnt);

    return cnt;
}

static ARM_FLASH_INFO* GetInfo(void)
{
    return &Info;
}

ARM_DRIVER_FLASH ram_driver = {
    .GetVersion = NULL,
    .GetCapabilities = GetCapabilities,
    .Initialize = Initialize,
    .Uninitialize = Uninitialize,
    .PowerControl = NULL,
    .ReadData = ReadData,
    .ProgramData = NULL,
    .EraseSector = NULL,
    .EraseChip = NULL,
    .GetStatus = NULL,
    .GetInfo = GetInfo,
};
