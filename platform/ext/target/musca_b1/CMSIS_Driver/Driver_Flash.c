/*
 * Copyright (c) 2013-2018 ARM Limited. All rights reserved.
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
 *
 * This file is a derivative of a previous version of
 * platform/ext/target/musca_a/CMSIS_Driver/Driver_Flash.c
 * Git SHA: 807702a58295842dede30693d46c8a928cba0895
 */

/* FIXME: This interim flash driver uses code SRAM to emulate flash. It should
 * be replaced with a real flash driver.
 */

#include <string.h>
#include <stdint.h>
#include "Driver_Flash.h"
#include "RTE_Device.h"
#include "flash_layout.h"
#include "region_defs.h"

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION    ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

#define FLASH_REDIRECT_BASE  SST_FLASH_AREA_ADDR
#define FLASH_REDIRECT_LIMIT (FLASH_REDIRECT_BASE   \
                              + FLASH_SST_AREA_SIZE \
                              + FLASH_NV_COUNTERS_AREA_SIZE)
#define FLASH_REDIRECT_DEST  (S_CODE_SRAM_ALIAS_BASE + FLASH_AREA_BL2_SIZE)

#define FLASH0_BASE_S         0x10000000
#define FLASH0_BASE_NS        0x00000000
#define FLASH0_SIZE           0x00800000 /* 8 MB */
#define FLASH0_SECTOR_SIZE    0x00001000 /* 4 kB */
#define FLASH0_PAGE_SIZE      0x00001000 /* 4 kB */
#define FLASH0_PROGRAM_UNIT   0x4        /* Minimum write size */

/* FIXME: The following utility functions should be moved to a common place. */
static int32_t memcpy4(void *to, const void *from, size_t size)
{
    /* make sure that the parameters are aligned to 4 */
    if (((uint32_t)to & 0x03u) || ((uint32_t)from & 0x03u) || (size & 0x03u)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    for (int i = 0; i < size/4; ++i)
    {
        ((uint32_t*)to)[i] = ((uint32_t*)from)[i];
    }

    return ARM_DRIVER_OK;
}

static int32_t memset4(void *addr, int pattern, size_t count)
{
    uint32_t pattern32;

    /* make sure that the parameters are aligned to 4 */
    if (((uint32_t)addr & 0x03u) || (count & 0x03u)) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    pattern32 = (uint32_t)pattern |
                (uint32_t)pattern << 8 |
                (uint32_t)pattern << 16 |
                (uint32_t)pattern << 24;

    for (int i = 0; i < count/4; ++i)
    {
        ((uint32_t*)addr)[i] = pattern32;
    }

    return ARM_DRIVER_OK;
}

/*
 * ARM FLASH device structure
 */
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

#if (RTE_FLASH0)
static ARM_FLASH_INFO ARM_FLASH0_DEV_DATA = {
    .sector_info  = NULL,                  /* Uniform sector layout */
    .sector_count = FLASH0_SIZE / FLASH0_SECTOR_SIZE,
    .sector_size  = FLASH0_SECTOR_SIZE,
    .page_size    = FLASH0_PAGE_SIZE,
    .program_unit = FLASH0_PROGRAM_UNIT,
    .erased_value = 0xFF};

static struct arm_flash_dev_t ARM_FLASH0_DEV = {
#if (__DOMAIN_NS == 1)
    .memory_base = FLASH0_BASE_NS,
#else
    .memory_base = FLASH0_BASE_S,
#endif /* __DOMAIN_NS == 1 */
    .data        = &(ARM_FLASH0_DEV_DATA)};

struct arm_flash_dev_t *FLASH0_DEV = &ARM_FLASH0_DEV;

/*
 * Functions
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

static int32_t ARM_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    volatile uint32_t mem_base = FLASH0_DEV->memory_base;
    uint32_t start_addr = mem_base + addr;
    int32_t rc = 0;

    /* Check flash memory boundaries */
    rc = is_range_valid(FLASH0_DEV, addr + cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Redirecting SST storage to code sram */
    if (addr >= FLASH_REDIRECT_BASE && addr <= FLASH_REDIRECT_LIMIT) {
        start_addr = FLASH_REDIRECT_DEST + (addr - FLASH_REDIRECT_BASE);
    }

    /* Flash interface just emulated over SRAM, use memcpy */
    memcpy(data, (void *)start_addr, cnt);
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_ProgramData(uint32_t addr, const void *data,
                                     uint32_t cnt)
{
    volatile uint32_t mem_base = FLASH0_DEV->memory_base;
    uint32_t start_addr = mem_base + addr;
    int32_t rc = 0;

    /* Check flash memory boundaries and alignment with minimal write size */
    rc  = is_range_valid(FLASH0_DEV, addr + cnt);
    rc |= is_write_aligned(FLASH0_DEV, addr);
    rc |= is_write_aligned(FLASH0_DEV, cnt);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Redirecting SST storage to code sram */
    if (addr >= FLASH_REDIRECT_BASE && addr <= FLASH_REDIRECT_LIMIT) {
        start_addr = FLASH_REDIRECT_DEST + (addr - FLASH_REDIRECT_BASE);
        /* Flash interface emulated over CODE SRAM. Writing it on 4 aligned
         * addresses is necessary, so use a special memcpy function.
         */
        rc = memcpy4((void *)start_addr, data, cnt);
        if (rc != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }
    } else {
        /* Flash interface just emulated over SRAM, use memcpy */
        memcpy((void *)start_addr, data, cnt);
    }
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseSector(uint32_t addr)
{
    volatile uint32_t mem_base = FLASH0_DEV->memory_base;
    uint32_t rc = 0;

    rc  = is_range_valid(FLASH0_DEV, addr);
    rc |= is_sector_aligned(FLASH0_DEV, addr);
    if (rc != 0) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }

    /* Redirecting SST storage to code sram */
    if (addr >= FLASH_REDIRECT_BASE && addr <= FLASH_REDIRECT_LIMIT) {
        /* Flash interface emulated over CODE SRAM. Writing it on 4 aligned
         * addresses is necessary, so use a special memset function.
         */
        rc = memset4((void *)(FLASH_REDIRECT_DEST
                              + (addr - FLASH_REDIRECT_BASE)),
                     FLASH0_DEV->data->erased_value,
                     FLASH0_DEV->data->sector_size);
        if (rc != ARM_DRIVER_OK) {
            return ARM_DRIVER_ERROR_PARAMETER;
        }

    } else {
        /* Flash interface just emulated over SRAM, use memset */
        memset((void *)mem_base + addr,
               FLASH0_DEV->data->erased_value,
               FLASH0_DEV->data->sector_size);
    }
    return ARM_DRIVER_OK;
}

static int32_t ARM_Flash_EraseChip(void)
{
    uint32_t i;
    uint32_t addr = FLASH0_DEV->memory_base;
    int32_t rc = ARM_DRIVER_ERROR_UNSUPPORTED;

    /* Check driver capability erase_chip bit */
    if (DriverCapabilities.erase_chip == 1) {
        for (i = 0; i < FLASH0_DEV->data->sector_count; i++) {
            /* Redirecting SST storage to code sram */
            if (addr >= FLASH_REDIRECT_BASE && addr <= FLASH_REDIRECT_LIMIT) {
                rc = memset4((void *)(FLASH_REDIRECT_DEST +
                        (addr - FLASH0_DEV->memory_base - FLASH_REDIRECT_BASE)),
                        FLASH0_DEV->data->erased_value,
                        FLASH0_DEV->data->sector_size);
                if (rc != ARM_DRIVER_OK) {
                    return ARM_DRIVER_ERROR_PARAMETER;
                }

            } else {
                /* Flash interface just emulated over SRAM, use memset */
                memset((void *)addr,
                       FLASH0_DEV->data->erased_value,
                       FLASH0_DEV->data->sector_size);
            }
            addr += FLASH0_DEV->data->sector_size;
            rc = ARM_DRIVER_OK;
        }
    }
    return rc;
}

static ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

static ARM_FLASH_INFO *ARM_Flash_GetInfo(void)
{
    return FLASH0_DEV->data;
}

ARM_DRIVER_FLASH Driver_FLASH0 = {
    ARM_Flash_GetVersion,
    ARM_Flash_GetCapabilities,
    ARM_Flash_Initialize,
    ARM_Flash_Uninitialize,
    ARM_Flash_PowerControl,
    ARM_Flash_ReadData,
    ARM_Flash_ProgramData,
    ARM_Flash_EraseSector,
    ARM_Flash_EraseChip,
    ARM_Flash_GetStatus,
    ARM_Flash_GetInfo
};
#endif /* RTE_FLASH0 */
