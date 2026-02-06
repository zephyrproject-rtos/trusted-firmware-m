/*
 * Copyright 2017-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <string.h>

#include "mflash_drv.h"
#include "fsl_romapi.h"
#include "pin_mux.h"

static flash_config_t g_flash_instance;

static uint32_t pflashBlockBase;
static uint32_t pflashTotalSize;
static uint32_t pflashSectorSize;
static uint32_t pflashPageSize;

static void speculation_buffer_clear(void)
{
    /* Clear Flash/Flash data speculation. */
    if(((SYSCON->NVM_CTRL & SYSCON_NVM_CTRL_DIS_MBECC_ERR_INST_MASK) == 0U)
        && ((SYSCON->NVM_CTRL & SYSCON_NVM_CTRL_DIS_MBECC_ERR_DATA_MASK) == 0U))
    {
        if((SYSCON->NVM_CTRL & SYSCON_NVM_CTRL_DIS_FLASH_SPEC_MASK) == 0U)
        {
            /* Disable flash speculation first. */
            SYSCON->NVM_CTRL |= SYSCON_NVM_CTRL_DIS_FLASH_SPEC_MASK;
            /* Re-enable flash speculation. */
            SYSCON->NVM_CTRL &= ~SYSCON_NVM_CTRL_DIS_FLASH_SPEC_MASK;
        }
        if((SYSCON->NVM_CTRL & SYSCON_NVM_CTRL_DIS_DATA_SPEC_MASK) == 0U)
        {
            /* Disable flash data speculation first. */
            SYSCON->NVM_CTRL |= SYSCON_NVM_CTRL_DIS_DATA_SPEC_MASK;
            /* Re-enable flash data speculation. */
            SYSCON->NVM_CTRL &= ~SYSCON_NVM_CTRL_DIS_DATA_SPEC_MASK;
        }
    }
}

/* API - initialize 'mflash' */
int32_t mflash_drv_init(void)
{
    status_t result;

    result = FLASH_Init(&g_flash_instance);
    if (result != kStatus_Success)
    {
        return result;
    }

    result = FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashBlockBaseAddr, &pflashBlockBase);
    if (result != kStatus_Success)
    {
        return result;
    }

    result = FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashSectorSize, &pflashSectorSize);
    if (result != kStatus_Success)
    {
        return result;
    }

    result = FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashTotalSize, &pflashTotalSize);
    if (result != kStatus_Success)
    {
        return result;
    }

    result = FLASH_GetProperty(&g_flash_instance, kFLASH_PropertyPflashPageSize, &pflashPageSize);
    if (result != kStatus_Success)
    {
        return result;
    }

    if ((pflashSectorSize != MFLASH_SECTOR_SIZE) || (pflashPageSize != MFLASH_PAGE_SIZE))
    {
        return kStatus_Fail;
    }

    return kStatus_Success;
}

/* API - Erase single sector */
int32_t mflash_drv_sector_erase(uint32_t sector_addr)
{
    status_t ret;

    if (false == mflash_drv_is_sector_aligned(sector_addr))
    {
        return kStatus_InvalidArgument;
    }
    else
    {
        ret = FLASH_EraseSector(&g_flash_instance, sector_addr, MFLASH_SECTOR_SIZE, kFLASH_ApiEraseKey);
    }

    speculation_buffer_clear();

    return ret;
}

/* API - Page program */
int32_t mflash_drv_page_program(uint32_t page_addr, uint32_t *data)
{
    status_t ret;

    if (false == mflash_drv_is_page_aligned(page_addr))
    {
        return kStatus_InvalidArgument;
    }
    else
    {
        ret = FLASH_ProgramPage(&g_flash_instance, page_addr, (uint8_t *)data, MFLASH_PAGE_SIZE);
    }

    speculation_buffer_clear();

    return ret;
}

/* API - Phrase program */
int32_t mflash_drv_phrase_program(uint32_t page_addr, uint32_t *data)
{
    status_t ret;

    if (false == mflash_drv_is_phrase_aligned(page_addr))
    {
        return kStatus_InvalidArgument;
    }
    else
    {
        ret = FLASH_ProgramPhrase(&g_flash_instance, page_addr, (uint8_t *)data, MFLASH_PHRASE_SIZE);
    }

    speculation_buffer_clear();

    return ret;
}

/* API - Read data */
int32_t mflash_drv_read(uint32_t addr, uint32_t *buffer, uint32_t len)
{
    (void)memcpy(buffer, (void *)addr, len);
    return kStatus_Success;
}

/* API - Get pointer to FLASH region */
void *mflash_drv_phys2log(uint32_t addr, uint32_t len)
{
    /* FLASH is directly mapped in the address space */
    return (void *)(addr);
}

/* API - Get pointer to FLASH region */
uint32_t mflash_drv_log2phys(void *ptr, uint32_t len)
{
    /* FLASH is directly mapped in the address space */
    return ((uint32_t)ptr);
}
