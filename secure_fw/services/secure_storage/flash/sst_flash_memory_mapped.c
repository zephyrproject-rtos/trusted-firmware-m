/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_flash.h"

#include <string.h>
#include "tfm_sst_defs.h"
#include "secure_fw/services/secure_storage/sst_utils.h"

/*************************************/
/* Flash memory management functions */
/*************************************/

#ifdef SST_TESTS
#define SST_DATA_VISIBLITY
#else
#define SST_DATA_VISIBLITY static
#endif

SST_DATA_VISIBLITY
uint8_t block_data[SST_BLOCK_SIZE*SST_TOTAL_NUM_OF_BLOCKS] = {0};

enum tfm_sst_err_t sst_flash_read(uint32_t block_id, uint8_t *buff,
                                  uint32_t offset, uint32_t size)
{
    uint8_t *p_block_data;

    /* Calculate block data pointer to the address location defined by
     * block ID and offset parameters.
     */
    p_block_data = block_data + (block_id * SST_BLOCK_SIZE) + offset;

    sst_utils_memcpy(buff, p_block_data, size);

    return TFM_SST_ERR_SUCCESS;
}

enum tfm_sst_err_t sst_flash_write(uint32_t block_id, const uint8_t *buff,
                                   uint32_t offset, uint32_t size)
{
    uint8_t *p_block_data;

    /* Calculate block data pointer to the address location defined by
     * block ID and offset parameters.
     */
    p_block_data = block_data + (block_id * SST_BLOCK_SIZE) + offset;

    sst_utils_memcpy(p_block_data, buff, size);

    return TFM_SST_ERR_SUCCESS;
}

enum tfm_sst_err_t sst_flash_block_to_block_move(uint32_t dst_block,
                                                 uint32_t dst_offset,
                                                 uint32_t src_block,
                                                 uint32_t src_offset,
                                                 uint32_t size)
{
    uint8_t *p_dst_block_data;
    uint8_t *p_src_block_data;

    /* Calculate block data pointer to the address location defined by
     * source block ID and offset parameters.
     */
    p_src_block_data = block_data + (src_block * SST_BLOCK_SIZE) + src_offset;

    /* Calculate block data pointer to the address location defined by
     * source block ID and offset parameters.
     */
    p_dst_block_data = block_data + (dst_block * SST_BLOCK_SIZE) + dst_offset;

    sst_utils_memcpy(p_dst_block_data, p_src_block_data, size);

    return TFM_SST_ERR_SUCCESS;
}

enum tfm_sst_err_t sst_flash_erase_block(uint32_t block_id)
{
    uint8_t *p_block_data;

    /* Calculate block data pointer to the address location defined by
     * block ID and offset parameters.
     */
    p_block_data = block_data + (block_id * SST_BLOCK_SIZE);

    sst_utils_memset(p_block_data, SST_FLASH_DEFAULT_VAL, SST_BLOCK_SIZE);

    return TFM_SST_ERR_SUCCESS;
}
