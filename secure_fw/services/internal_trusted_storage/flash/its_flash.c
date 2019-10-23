/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_flash.h"

#include "Driver_Flash.h"
#include "tfm_memory_utils.h"

#define BLOCK_START_OFFSET  0
#define MAX_BLOCK_DATA_COPY 256

extern const struct its_flash_info_t its_flash_info_internal;

static const struct its_flash_info_t *const flash_infos[] = {
    [ITS_FLASH_ID_INTERNAL] = &its_flash_info_internal,
};

/**
 * \brief Gets physical address of the given block ID.
 *
 * \param[in] info      Flash device information
 * \param[in] block_id  Block ID
 * \param[in] offset    Offset position from the init of the block
 *
 * \returns Returns physical address for the given block ID.
 */
static uint32_t get_phys_address(const struct its_flash_info_t *info,
                                 uint32_t block_id, size_t offset)
{
    return info->flash_area_addr + (block_id * info->block_size) + offset;
}

#ifdef ITS_RAM_FS
static psa_status_t flash_init(const struct its_flash_info_t *info)
{
    /* Nothing needs to be done in case of flash emulated in RAM */
    return PSA_SUCCESS;
}

static psa_status_t flash_read(const struct its_flash_info_t *info,
                               uint32_t flash_addr, size_t size, uint8_t *buff)
{
    uint32_t idx = flash_addr - info->flash_area_addr;

    (void)tfm_memcpy(buff, (uint8_t *)info->flash_dev + idx, size);

    return PSA_SUCCESS;
}

static psa_status_t flash_write(const struct its_flash_info_t *info,
                                uint32_t flash_addr, size_t size,
                                const uint8_t *buff)
{
    uint32_t idx = flash_addr - info->flash_area_addr;

    (void)tfm_memcpy((uint8_t *)info->flash_dev + idx, buff, size);

    return PSA_SUCCESS;
}

static psa_status_t flash_erase(const struct its_flash_info_t *info,
                                uint32_t flash_addr)
{
    uint32_t idx = flash_addr - info->flash_area_addr;

    (void)tfm_memset((uint8_t *)info->flash_dev + idx, info->erase_val,
                     info->sector_size);

    return PSA_SUCCESS;
}
#else /* ITS_RAM_FS */
static psa_status_t flash_init(const struct its_flash_info_t *info)
{
    int32_t err;

    err = ((ARM_DRIVER_FLASH *)info->flash_dev)->Initialize(NULL);
    if (err != ARM_DRIVER_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}

static psa_status_t flash_read(const struct its_flash_info_t *info,
                               uint32_t flash_addr, size_t size, uint8_t *buff)
{
    int32_t err;

    err = ((ARM_DRIVER_FLASH *)info->flash_dev)->ReadData(flash_addr, buff,
                                                          size);
    if (err != ARM_DRIVER_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}

static psa_status_t flash_write(const struct its_flash_info_t *info,
                                uint32_t flash_addr, size_t size,
                                const uint8_t *buff)
{
    int32_t err;

    err = ((ARM_DRIVER_FLASH *)info->flash_dev)->ProgramData(flash_addr, buff,
                                                             size);
    if (err != ARM_DRIVER_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}

static psa_status_t flash_erase(const struct its_flash_info_t *info,
                                uint32_t flash_addr)
{
    int32_t err;

    err = ((ARM_DRIVER_FLASH *)info->flash_dev)->EraseSector(flash_addr);
    if (err != ARM_DRIVER_OK) {
        return PSA_ERROR_STORAGE_FAILURE;
    }

    return PSA_SUCCESS;
}
#endif /* ITS_RAM_FS */

const struct its_flash_info_t *its_flash_get_info(enum its_flash_id_t id)
{
    return flash_infos[id];
}

psa_status_t its_flash_init(const struct its_flash_info_t *info)
{
    return flash_init(info);
}

psa_status_t its_flash_read(const struct its_flash_info_t *info,
                            uint32_t block_id, uint8_t *buff,
                            size_t offset, size_t size)
{
    uint32_t flash_addr;

    /* Gets flash address location defined by block ID and offset
     * parameters.
     */
    flash_addr = get_phys_address(info, block_id, offset);

    return flash_read(info, flash_addr, size, buff);
}

psa_status_t its_flash_write(const struct its_flash_info_t *info,
                             uint32_t block_id, const uint8_t *buff,
                             size_t offset, size_t size)
{
    uint32_t flash_addr;

    /* Gets flash address location defined by block ID and offset
     * parameters.
     */
    flash_addr = get_phys_address(info, block_id, offset);

    return flash_write(info, flash_addr, size, buff);
}

psa_status_t its_flash_block_to_block_move(const struct its_flash_info_t *info,
                                           uint32_t dst_block,
                                           size_t dst_offset,
                                           uint32_t src_block,
                                           size_t src_offset,
                                           size_t size)
{
    psa_status_t err;
    uint8_t dst_block_data_copy[MAX_BLOCK_DATA_COPY];
    uint32_t dst_flash_addr;
    uint32_t src_flash_addr;
    size_t bytes_to_move;

    /* Gets flash addresses defined by block ID and offset parameters */
    src_flash_addr = get_phys_address(info, src_block, src_offset);
    dst_flash_addr = get_phys_address(info, dst_block, dst_offset);

    while (size > 0) {
        /* Calculates the number of bytes to move */
        bytes_to_move = ITS_UTILS_MIN(size, MAX_BLOCK_DATA_COPY);

        /* Reads data from source block and store it in the in-memory copy of
         * destination content.
         */
        err = flash_read(info, src_flash_addr, bytes_to_move,
                         dst_block_data_copy);
        if (err != PSA_SUCCESS) {
            return err;
        }

        /* Writes in flash the in-memory block content after modification */
        err = flash_write(info, dst_flash_addr, bytes_to_move,
                          dst_block_data_copy);
        if (err != PSA_SUCCESS) {
            return err;
        }

        /* Updates pointers to the source and destination flash regions */
        src_flash_addr += bytes_to_move;
        dst_flash_addr += bytes_to_move;

        /* Decrement remaining size to move */
        size -= bytes_to_move;
    };

    return PSA_SUCCESS;
}

psa_status_t its_flash_erase_block(const struct its_flash_info_t *info,
                                   uint32_t block_id)
{
    psa_status_t status;
    uint32_t flash_addr;
    size_t offset = BLOCK_START_OFFSET;

    while (offset < info->block_size) {
        /* Gets the flash address defined by block ID and offset parameters */
        flash_addr = get_phys_address(info, block_id, offset);

        status = flash_erase(info, flash_addr);
        if (status != PSA_SUCCESS) {
            return status;
        }

        /* Move to next sector */
        offset += info->sector_size;
    }

    return PSA_SUCCESS;
}
