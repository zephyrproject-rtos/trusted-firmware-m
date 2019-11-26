/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_flash.h"
#ifdef ITS_RAM_FS
#include "its_flash_ram.h"
#else
#include "its_flash_nor.h"
#endif

#include "Driver_Flash.h"
#include "flash_layout.h"
#include "secure_fw/services/internal_trusted_storage/its_utils.h"

#ifndef ITS_FLASH_DEV_NAME
#error "ITS_FLASH_DEV_NAME must be defined by the target in flash_layout.h"
#endif

#ifndef ITS_FLASH_AREA_ADDR
#error "ITS_FLASH_AREA_ADDR must be defined by the target in flash_layout.h"
#endif

/* Adjust to a size that will allow all assets to fit */
#ifndef ITS_FLASH_AREA_SIZE
#error "ITS_FLASH_AREA_SIZE must be defined by the target in flash_layout.h"
#endif

/* Adjust to match the size of the flash device's physical erase unit */
#ifndef ITS_SECTOR_SIZE
#error "ITS_SECTOR_SIZE must be defined by the target in flash_layout.h"
#endif

/* Adjust so that the maximum required asset size will fit in one block */
#ifndef ITS_SECTORS_PER_BLOCK
#error "ITS_SECTORS_PER_BLOCK must be defined by the target in flash_layout.h"
#endif

/* Adjust to match the size of the flash device's physical program unit */
#ifndef ITS_FLASH_PROGRAM_UNIT
#error "ITS_FLASH_PROGRAM_UNIT must be defined by the target in flash_layout.h"
#endif

/* FIXME: Support other flash program units */
#if ((ITS_FLASH_PROGRAM_UNIT != 1) && (ITS_FLASH_PROGRAM_UNIT != 2) \
     && (ITS_FLASH_PROGRAM_UNIT != 4) && (ITS_FLASH_PROGRAM_UNIT != 8))
#error "The supported ITS_FLASH_PROGRAM_UNIT values are 1, 2, 4 or 8 bytes"
#endif

/* Calculate the block layout */
#define FLASH_INFO_BLOCK_SIZE (ITS_SECTOR_SIZE * ITS_SECTORS_PER_BLOCK)
#define FLASH_INFO_NUM_BLOCKS (ITS_FLASH_AREA_SIZE / FLASH_INFO_BLOCK_SIZE)

/* Maximum file size */
#define FLASH_INFO_MAX_FILE_SIZE ITS_UTILS_ALIGN(ITS_MAX_ASSET_SIZE, \
                                                 ITS_FLASH_PROGRAM_UNIT)

/* Maximum number of files */
#define FLASH_INFO_MAX_NUM_FILES ITS_NUM_ASSETS

/* Default value of each byte in the flash when erased */
#define FLASH_INFO_ERASE_VAL 0xFFU

#ifdef ITS_RAM_FS
/* Allocate a static buffer to emulate storage in RAM */
static uint8_t its_block_data[FLASH_INFO_BLOCK_SIZE * FLASH_INFO_NUM_BLOCKS];
#define FLASH_INFO_DEV its_block_data
#else
/* Import the CMSIS flash device driver */
extern ARM_DRIVER_FLASH ITS_FLASH_DEV_NAME;
#define FLASH_INFO_DEV &ITS_FLASH_DEV_NAME
#endif

const struct its_flash_info_t its_flash_info_internal = {
#ifdef ITS_RAM_FS
    .init = its_flash_ram_init,
    .read = its_flash_ram_read,
    .write = its_flash_ram_write,
    .flush = its_flash_ram_flush,
    .erase = its_flash_ram_erase,
#else
    .init = its_flash_nor_init,
    .read = its_flash_nor_read,
    .write = its_flash_nor_write,
    .flush = its_flash_nor_flush,
    .erase = its_flash_nor_erase,
#endif
    .flash_dev = (void *)FLASH_INFO_DEV,
    .flash_area_addr = ITS_FLASH_AREA_ADDR,
    .sector_size = ITS_SECTOR_SIZE,
    .block_size = FLASH_INFO_BLOCK_SIZE,
    .num_blocks = FLASH_INFO_NUM_BLOCKS,
    .program_unit = ITS_FLASH_PROGRAM_UNIT,
    .max_file_size = FLASH_INFO_MAX_FILE_SIZE,
    .max_num_files = FLASH_INFO_MAX_NUM_FILES,
    .erase_val = FLASH_INFO_ERASE_VAL,
};

/* Checks at compile time that the flash device configuration is valid */
#include \
"secure_fw/services/internal_trusted_storage/flash_fs/its_flash_fs_check_info.h"
