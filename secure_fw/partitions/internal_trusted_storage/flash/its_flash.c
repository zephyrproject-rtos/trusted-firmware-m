/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020 Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "its_flash.h"

#include "flash_fs/its_flash_fs.h"
#include "tfm_hal_its.h"
#include "tfm_hal_ps.h"

#ifndef ITS_MAX_BLOCK_DATA_COPY
#define ITS_MAX_BLOCK_DATA_COPY 256
#endif

extern struct its_flash_info_t its_flash_info_internal;
extern struct its_flash_info_t its_flash_info_external;

static struct its_flash_info_t *const flash_infos[] = {
    [ITS_FLASH_ID_INTERNAL] = &its_flash_info_internal,
    [ITS_FLASH_ID_EXTERNAL] = &its_flash_info_external,
};

const struct its_flash_info_t *its_flash_get_info(enum its_flash_id_t id)
{
    struct its_flash_info_t *ret = flash_infos[id];

    /* Ask the platform for the filesystem config */
    switch (id) {
    case ITS_FLASH_ID_INTERNAL:
        tfm_hal_its_fs_info(&ret->fs_info.flash_area_addr, &ret->fs_info.flash_area_size);
        break;

    case ITS_FLASH_ID_EXTERNAL:
        tfm_hal_ps_fs_info(&ret->fs_info.flash_area_addr, &ret->fs_info.flash_area_size);
        break;

    default:
        return NULL;
    }

    /* Derive num_blocks */
    ret->num_blocks = ret->fs_info.flash_area_size / ret->block_size;

    /* Check that the parameters are compatible */
    if (its_flash_fs_validate_params(ret) != PSA_SUCCESS) {
        ret = NULL;
    }

    return ret;
}

psa_status_t its_flash_block_to_block_move(const struct its_flash_info_t *info,
                                           uint32_t dst_block,
                                           size_t dst_offset,
                                           uint32_t src_block,
                                           size_t src_offset,
                                           size_t size)
{
    psa_status_t status;
    size_t bytes_to_move;
    uint8_t dst_block_data_copy[ITS_MAX_BLOCK_DATA_COPY];

    while (size > 0) {
        /* Calculates the number of bytes to move */
        bytes_to_move = ITS_UTILS_MIN(size, ITS_MAX_BLOCK_DATA_COPY);

        /* Reads data from source block and store it in the in-memory copy of
         * destination content.
         */
        status = info->read(info, src_block, dst_block_data_copy, src_offset,
                            bytes_to_move);
        if (status != PSA_SUCCESS) {
            return status;
        }

        /* Writes in flash the in-memory block content after modification */
        status = info->write(info, dst_block, dst_block_data_copy, dst_offset,
                             bytes_to_move);
        if (status != PSA_SUCCESS) {
            return status;
        }

        /* Updates pointers to the source and destination flash regions */
        dst_offset += bytes_to_move;
        src_offset += bytes_to_move;

        /* Decrement remaining size to move */
        size -= bytes_to_move;
    }

    return PSA_SUCCESS;
}
