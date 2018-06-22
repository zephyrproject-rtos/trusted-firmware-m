/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_core.h"

#include <string.h>

#include "flash/sst_flash.h"
#include "sst_object_defs.h"
#include "sst_utils.h"

#if ((SST_TOTAL_NUM_OF_BLOCKS < 2) || (SST_TOTAL_NUM_OF_BLOCKS == 3))
  /* The minimum number of blocks is 2. In this case, metadata and data are
   * stored in the same physical block, and the other block is required for
   * power failure safe operation.
   * If at least 1 data block is available, 1 data scratch block is required for
   * power failure safe operation. So, in this case, the minimum number of
   * blocks is 4 (2 metadata block + 2 data blocks).
   */
  #error "Total number of blocks should be 2 or bigger than 3"
#endif

/* The numbers in the defines are physical block indexes, starting from 0,
 * except for SST_NUM_DEDICATED_DBLOCKS.
 */
#if (SST_TOTAL_NUM_OF_BLOCKS == 2)
  /* Metadata and data are stored in the same physical block, and the other
   * block is required for power failure safe operation.
   */

  /* Initial position of scratch block is the scratch metadata block */
  #define SST_INIT_SCRATCH_DBLOCK 1

  /* Metadata and data are stored in the same block */
  #define SST_INIT_DBLOCK_START 0

  /* There are no dedicated data blocks when only two blocks are available */
  #define SST_NUM_DEDICATED_DBLOCKS 0

#else
  /* Initial position of scratch block is immediately after metadata blocks */
  #define SST_INIT_SCRATCH_DBLOCK 2

  /* One metadata block and two scratch blocks are reserved. One scratch block
   * for metadata operations and the other for assets data operations.
   */
  #define SST_INIT_DBLOCK_START 3

  /* Number of blocks dedicated just for data is the number of blocks available
   * beyond the initial datablock start index.
   */
  #define SST_NUM_DEDICATED_DBLOCKS (SST_TOTAL_NUM_OF_BLOCKS - \
                                     SST_INIT_DBLOCK_START)
#endif

/* Total number of datablocks is the number of dedicated datablocks plus
 * logical datablock 0 stored in the metadata block.
 */
#define SST_NUM_ACTIVE_DBLOCKS (SST_NUM_DEDICATED_DBLOCKS + 1)

#define SST_LOGICAL_DBLOCK0  0
#define SST_OBJECT_START_POSITION  0

#ifndef SST_FLASH_PROGRAM_UNIT
#error "SST_FLASH_PROGRAM_UNIT must be defined in flash_layout.h"
#endif

/* FIXME: Support other flash program units.*/
#if ((SST_FLASH_PROGRAM_UNIT != 1) && (SST_FLASH_PROGRAM_UNIT != 2) \
      && (SST_FLASH_PROGRAM_UNIT != 4))
#error "The supported SST_FLASH_PROGRAM_UNIT values are 1, 2 or 4 bytes"
#endif

/* SST_ALIGNED_MAX_OBJECT_SIZE aligns the SST_MAX_OBJECT_SIZE with the
 * SST_FLASH_PROGRAM_UNIT
 */
#define SST_ALIGNED_MAX_OBJECT_SIZE \
((SST_FLASH_PROGRAM_UNIT - (SST_MAX_OBJECT_SIZE % SST_FLASH_PROGRAM_UNIT)) \
  + SST_MAX_OBJECT_SIZE)

#define SST_ALL_METADATA_SIZE \
(sizeof(struct sst_metadata_block_header) + \
(SST_NUM_ACTIVE_DBLOCKS * sizeof(struct sst_block_metadata)) + \
(SST_MAX_NUM_OBJECTS * sizeof(struct sst_assetmeta)))

#ifndef SST_ENCRYPTION
/* SST data buffer is used for metadata and object data. */
static uint8_t sst_data_buf[SST_ALIGNED_MAX_OBJECT_SIZE];
static uint8_t *sst_buf_plain_text = sst_data_buf;
#else
/* SST data buffer is used for metadata authentication */
static uint8_t sst_data_buf[SST_ALL_METADATA_SIZE];
#endif

static struct sst_asset_system_context sst_system_ctx;

#ifdef SST_ENCRYPTION
/* Check at compilation time if metadata fits in sst_data_buf */
SST_UTILS_BOUND_CHECK(METADATA_NOT_FIT_IN_DATA_BUF,
                      SST_ALL_METADATA_SIZE, sizeof(sst_data_buf));

/* Size of metadata at the beginning of the metadata header that is not
 * authenticated. Everything after the MAC is authenticated.
 * FIXME: an alternative crypto policy may require an alternative definition.
 */
#define SST_NON_AUTH_METADATA_SIZE SST_TAG_LEN_BYTES

/* Offset in metadata at which authentication begins */
#define SST_AUTH_METADATA_OFFSET SST_NON_AUTH_METADATA_SIZE
#endif

/* It is not required that all assets fit in SST area at the same time.
 * So, it is possible that a create action fails because SST area is full.
 * However, the larger asset must have enough space in the SST area to be
 * created, at least, when the SST area is empty.
 */
/* Checks at compile time if the largest asset fits in the asset's data area */
SST_UTILS_BOUND_CHECK(LARGEST_ASSET_NOT_FIT_IN_DATA_BLOCK,
                      SST_ALIGNED_MAX_OBJECT_SIZE, SST_BLOCK_SIZE);

#if (SST_TOTAL_NUM_OF_BLOCKS == 2)
SST_UTILS_BOUND_CHECK(ASSET_NOT_FIT_IN_DATA_AREA, SST_ALIGNED_MAX_OBJECT_SIZE,
                      (SST_BLOCK_SIZE - SST_ALL_METADATA_SIZE));
#endif

/* Checks at compile time if the metadata fits in a flash block */
SST_UTILS_BOUND_CHECK(METADATA_NOT_FIT_IN_METADATA_BLOCK,
                      SST_ALL_METADATA_SIZE, SST_BLOCK_SIZE);

/**
 * \brief Gets the number of bytes aligned with the SST_FLASH_PROGRAM_UNIT.
 *
 * \param[in] nbr_bytes  Number of bytes to write
 *
 * \return Return number of bytes aligned with SST_FLASH_PROGRAM_UNIT
 */
static uint32_t sst_get_aligned_flash_bytes(uint32_t nbr_bytes)
{
    uint32_t align_flash_size;

    /* Check if write parameters are aligned with the flash write
     * program unit.
     */
    align_flash_size = (nbr_bytes % SST_FLASH_PROGRAM_UNIT);
    if (align_flash_size != 0) {
        /* Add offset to align object's size required with the flash write
         * program unit.
         */
        align_flash_size = (SST_FLASH_PROGRAM_UNIT - align_flash_size);
    }

    align_flash_size += nbr_bytes;

    return align_flash_size;
}

/**
 * \brief Gets offset of a logical block's metadata in metadata block
 *
 * \param[in] idx  Logical block number
 *
 * \return Return offset value in metadata block
 */
static uint32_t sst_meta_block_meta_offset(uint32_t idx)
{
    uint32_t offset;

    offset = sizeof(struct sst_metadata_block_header) +
             (idx * sizeof(struct sst_block_metadata));

    return offset;
}

/**
 * \brief Gets offset of an object's metadata in metadata block
 *
 * \param[in] idx  Object's index
 *
 * \return Return offset value in metadata block
 */
static uint32_t sst_meta_object_meta_offset(uint32_t object_index)
{
    uint32_t offset;

    offset = sizeof(struct sst_metadata_block_header)
             + (SST_NUM_ACTIVE_DBLOCKS * sizeof(struct sst_block_metadata))
             + (object_index * sizeof(struct sst_assetmeta));

    return offset;
}

/**
 * \brief Gets current scratch metadata block
 *
 * \return scratch metablock's id
 */
static uint32_t sst_meta_cur_meta_scratch(void)
{
    return sst_system_ctx.scratch_metablock;
}

/**
 * \brief Gets current active metadata block
 *
 * \return scratch metablock's id
 */
static uint32_t sst_meta_cur_meta_active(void)
{
    return sst_system_ctx.active_metablock;
}

/**
 * \brief Swaps metablocks. Scratch becomes active and active becomes scratch
 */
static void sst_meta_swap_metablocks(void)
{
    uint32_t tmp_block;

    tmp_block = sst_system_ctx.scratch_metablock;
    sst_system_ctx.scratch_metablock = sst_system_ctx.active_metablock;
    sst_system_ctx.active_metablock = tmp_block;
}

/**
 * \brief Gets current scratch datablock
 *
 * \prama[in] lblock  Logical block
 *
 * \return current scratch data block
 */
static uint32_t sst_meta_cur_data_scratch(uint32_t lblock)
{
    if (lblock == SST_LOGICAL_DBLOCK0) {
        /* Scratch logical data block 0 physical IDs */
        return sst_meta_cur_meta_scratch();
    }

    return sst_system_ctx.meta_block_header.scratch_idx;
}

/**
 * \brief Sets current data scratch block
 *
 * \param[in] index   Physical id of scratch data block
 * \param[in] lblock  Logical block number
 */
static void sst_meta_set_data_scratch(uint32_t index, uint32_t lblock)
{
    if (lblock != SST_LOGICAL_DBLOCK0) {
        sst_system_ctx.meta_block_header.scratch_idx = index;
    }
}

/**
 * \brief Finds the potential most recent valid metablock
 *        Based on update count.
 *
 * \param[in] meta0  Header metadata of meta block 0
 * \param[in] meta1  Header metadata of meta block 1
 *
 * \return most recent metablock
 */
static uint8_t sst_meta_latest_meta_block(
                                        struct sst_metadata_block_header *meta0,
                                        struct sst_metadata_block_header *meta1)
{
    uint8_t cur_meta;
    uint8_t meta0_swap_count = meta0->active_swap_count;
    uint8_t meta1_swap_count = meta1->active_swap_count;

    /* Logic: if the swap count is 0, then it has rolled over. The metadata
     * block with a swap count of 0 is the latest one, unless the other block
     * has a swap count of 1, in which case the roll over occurred in the
     * previous update. In all other cases, the block with the highest swap
     * count is the latest one.
     */
    if ((meta1_swap_count == 0) && (meta0_swap_count != 1)) {
        /* Metadata block 1 swap count has rolled over and metadata block 0
         * swap count has not, so block 1 is the latest
         */
        cur_meta = SST_METADATA_BLOCK1;

    } else if ((meta0_swap_count == 0) && (meta1_swap_count != 1)) {
        /* Metadata block 0 swap count has rolled over and metadata block 1
         * swap count has not, so block 0 is the latest.
         */
        cur_meta = SST_METADATA_BLOCK0;

    } else if (meta1_swap_count > meta0_swap_count) {
        /* Neither swap count has just rolled over and metadata block 1 has a
         * higher swap count, so block 1 is the latest.
         */
        cur_meta = SST_METADATA_BLOCK1;

    } else {
        /* Neither swap count has just rolled over and metadata block 0 has a
         * higher or equal swap count, so block 0 is the latest.
         */
        cur_meta = SST_METADATA_BLOCK0;
    }

    return cur_meta;
}

#ifdef SST_VALIDATE_METADATA_FROM_FLASH
/**
 * \brief Validates block metadata in order to guarantee that a corruption or
 *        malicious change in stored metadata doesn't result in an invalid
 *        access
 *
 * \param[in] block_meta  Pointer to block meta structure
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_validate_block_meta(
                                    const struct sst_block_metadata *block_meta)
{
    enum psa_sst_err_t err;
    /* for data blocks data start at position 0 */
    uint32_t valid_data_start_value = 0;

    if (block_meta->phys_id >= SST_TOTAL_NUM_OF_BLOCKS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* boundary check: block data start + free size can not be bigger
     * than max block size.
     */
    err = sst_utils_check_contained_in(0, SST_BLOCK_SIZE,
                                       block_meta->data_start,
                                       block_meta->free_size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    if (block_meta->phys_id == SST_METADATA_BLOCK0 ||
        block_meta->phys_id == SST_METADATA_BLOCK1) {

        /* for metadata + data block, data index must start after the
         * metadata area.
         */
        valid_data_start_value =
                               sst_meta_object_meta_offset(SST_MAX_NUM_OBJECTS);
    }

    if (block_meta->data_start != valid_data_start_value) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    return PSA_SST_ERR_SUCCESS;
}
#endif

/**
 * \brief Reads specified logical block's metadata
 *
 * \param[in] lblock        Logical block number
 * \param[out] block_meta   Pointer to block meta structure
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_read_block_metadata(uint32_t lblock,
                                          struct sst_block_metadata *block_meta)
{
    enum psa_sst_err_t err;
    uint32_t pos;
    uint32_t metablock;

    metablock = sst_meta_cur_meta_active();
    pos = sst_meta_block_meta_offset(lblock);
    err = sst_flash_read(metablock, (uint8_t *)block_meta,
                         pos, sizeof(struct sst_block_metadata));

#ifdef SST_VALIDATE_METADATA_FROM_FLASH
    if (err == PSA_SST_ERR_SUCCESS) {
        err = sst_meta_validate_block_meta(block_meta);
    }
#endif

    return err;
}

#ifdef SST_VALIDATE_METADATA_FROM_FLASH
/**
 * \brief Validates object metadata in order to guarantee that a corruption or
 *        malicious change in stored metadata doesn't result in an invalid
 *        access
 *
 * \param[in] meta  Pointer to object meta structure
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_validate_object_meta(
                                               const struct sst_assetmeta *meta)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SUCCESS;

    /* logical block ID can not be bigger or equal than number of
     * active blocks.
     */
    if (meta->lblock >= SST_NUM_ACTIVE_DBLOCKS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* meta->unique_id can be 0 if the object is not in use. If it is in
     * use, check the metadata.
     */
    if (sst_utils_validate_uuid(meta->unique_id) == PSA_SST_ERR_SUCCESS) {
        /* validate objects values if object is in use */
        if (meta->max_size > SST_MAX_OBJECT_SIZE) {
            return PSA_SST_ERR_ASSET_NOT_FOUND;
        }

        /* the current object's data size must be smaller or equal than
         * object's data max size.
         */
        if (meta->cur_size > meta->max_size) {
            return PSA_SST_ERR_ASSET_NOT_FOUND;
        }

        if (meta->lblock == SST_LOGICAL_DBLOCK0) {
            /* in block 0, data index must be located after the metadata */
            if (meta->data_index <
                sst_meta_object_meta_offset(SST_MAX_NUM_OBJECTS)) {
                return PSA_SST_ERR_ASSET_NOT_FOUND;
            }
        }

        /* Boundary check the incoming request */
        err = sst_utils_check_contained_in(0, SST_BLOCK_SIZE, meta->data_index,
                                           meta->max_size);
    }

    return err;
}
#endif

/**
 * \brief Reads specified object's metadata
 *
 * \param[in]  object_index  Object's index
 * \param[out] meta          Pointer to object meta structure
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_meta_read_object_meta(uint32_t object_index,
                                           struct sst_assetmeta *meta)
{
    enum psa_sst_err_t err;
    uint32_t offset;

    offset = sst_meta_object_meta_offset(object_index);
    err = sst_flash_read(sst_system_ctx.active_metablock,
                         (uint8_t *)meta, offset, sizeof(struct sst_assetmeta));

#ifdef SST_VALIDATE_METADATA_FROM_FLASH
    if (err == PSA_SST_ERR_SUCCESS) {
        err = sst_meta_validate_object_meta(meta);
    }
#endif

    return err;
}

/**
 * \brief Gets a free object metadata table entry
 *
 * \return Return index of a free object meta entry
 */
static uint16_t sst_get_free_object_index(void)
{
    uint16_t i;
    enum psa_sst_err_t err;
    struct sst_assetmeta tmp_metadata;

    for (i = 0; i < SST_MAX_NUM_OBJECTS; i++) {
        err = sst_meta_read_object_meta(i, &tmp_metadata);
        if (err == PSA_SST_ERR_SUCCESS) {
            /* Check if this entry is free by checking if unique_id values is an
             * invalid UUID.
             */
            if (sst_utils_validate_uuid(tmp_metadata.unique_id) !=
                                                          PSA_SST_ERR_SUCCESS) {
                /* Found */
                return i;
            }
        } else {
            return SST_METADATA_INVALID_INDEX;
        }
    }

    return SST_METADATA_INVALID_INDEX;
}

/**
 * \brief Converts logical data block number to physical number
 *
 * \param[in] lblock  Logical block number
 *
 * \return Return physical block number
 */
static uint32_t sst_dblock_lo_to_phy(uint32_t lblock)
{
    struct sst_block_metadata block_meta;
    enum psa_sst_err_t err;

    err = sst_meta_read_block_metadata(lblock, &block_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return SST_BLOCK_INVALID_ID;
    }

    return block_meta.phys_id;
}

/**
 * \brief Updates the scratch datablock with requested data
 *        and copy rest of the data in the current block
 *
 * \param[in] cur_logical_block  Current datablock
 * \param[in] block_meta         Pointer to block meta
 * \param[in] data               Pointer to data buffer
 * \param[in] offset             Offset in the block
 * \param[in] size               Size of the incoming data
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_dblock_update_scratch(uint32_t cur_logical_block,
                                    const struct sst_block_metadata *block_meta,
                                    const uint8_t *data,
                                    uint32_t offset,
                                    uint32_t size)
{
    uint32_t scratch_block;
    uint32_t end_data;
    enum psa_sst_err_t err;

    scratch_block = sst_meta_cur_data_scratch(cur_logical_block);

    err = sst_flash_write(scratch_block, data, offset, size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    if (offset > block_meta->data_start) {
        /* Copy rest of the block data from previous block */
        /* Data before updated content */
        err = sst_flash_block_to_block_move(scratch_block,
                                            block_meta->data_start,
                                            block_meta->phys_id,
                                            block_meta->data_start,
                                            (offset - block_meta->data_start));
        if (err != PSA_SST_ERR_SUCCESS) {
            return err;
        }
    }

    /* Calculate amount of used data in the block */
    end_data = (SST_BLOCK_SIZE - (offset + size) - block_meta->free_size);

    /* Data after updated content */
    err = sst_flash_block_to_block_move(scratch_block, (offset + size),
                                        block_meta->phys_id, (offset + size),
                                        end_data);
    return err;
}

/**
 * \brief Writes a metadata entry into scratch metadata block
 *
 * \param[in] object_index  Object's index in the metadata table
 * \param[in] obj_meta      Metadata pointer
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_mblock_update_scratch_object_meta(
                                                 uint32_t object_index,
                                                 struct sst_assetmeta *obj_meta)
{
    uint32_t scratch_block;
    enum psa_sst_err_t err;
    uint32_t pos;

    scratch_block = sst_meta_cur_meta_scratch();
    /* Calculate the position */
    pos = sst_meta_object_meta_offset(object_index);
    err = sst_flash_write(scratch_block, (uint8_t *)obj_meta, pos,
                          sizeof(struct sst_assetmeta));
    return err;
}

/**
 * \brief Erases data and meta scratch blocks
 */
static enum psa_sst_err_t sst_meta_erase_scratch_blocks(void)
{
    enum psa_sst_err_t err;
    uint32_t scratch_datablock;
    uint32_t scratch_metablock;

    scratch_metablock = sst_meta_cur_meta_scratch();

    /* For the atomicity of the data update process
     * and power-failure-safe operation, it is necessary that
     * metadata scratch block is erased before data block.
     */
    err = sst_flash_erase_block(scratch_metablock);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* If the number of total blocks is not bigger than 2, it means there are
     * not blocks which contains only data, and all data is stored in the
     * same block where the meta data is stored. If the number of blocks
     * is bigger than 2, the code needs to erase the scratch block used
     * to process any change in the data block which contains only data.
     */
    if (SST_TOTAL_NUM_OF_BLOCKS > 2) {
        scratch_datablock = sst_meta_cur_data_scratch(SST_LOGICAL_DBLOCK0+1);
        err = sst_flash_erase_block(scratch_datablock);
    }

    return err;
}

/**
 * \brief Puts logical block's metadata in scratch metadata block
 *
 * \param[in] lblock      Logical block number
 * \param[in] block_meta  Pointer to block's metadata
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_mblock_update_scratch_block_meta(uint32_t lblock,
                                          struct sst_block_metadata *block_meta)
{
    uint32_t meta_block;
    enum psa_sst_err_t err;
    uint32_t pos;

    meta_block = sst_meta_cur_meta_scratch();
    /* Calculate the position */
    pos = sst_meta_block_meta_offset(lblock);
    err = sst_flash_write(meta_block, (uint8_t *)block_meta, pos,
                          sizeof(struct sst_block_metadata));
    return err;
}

/**
 * \brief Copies rest of the object metadata, except for the one pointed by
 *        index
 *
 * \param[in] idx  Object index to skip
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_mblock_copy_remaining_object_meta(
                                                          uint32_t object_index)
{
    uint32_t scratch_block;
    uint32_t meta_block;
    enum psa_sst_err_t err;
    uint32_t end;
    uint32_t pos;

    scratch_block = sst_meta_cur_meta_scratch();
    meta_block = sst_meta_cur_meta_active();
    /* Calculate the position */
    pos = sst_meta_object_meta_offset(0);
    /* Copy rest of the block data from previous blok */
    /* Data before updated content */
    err = sst_flash_block_to_block_move(scratch_block, pos, meta_block, pos,
                                        (object_index *
                                         sizeof(struct sst_assetmeta)));
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Data after updated content */
    pos = sst_meta_object_meta_offset(object_index+1);

    /* Get end of object meta position which is the position after the last
     * byte of object meta.
     */
    end = sst_meta_object_meta_offset(SST_MAX_NUM_OBJECTS);
    if (end > pos) {
        err = sst_flash_block_to_block_move(scratch_block, pos, meta_block,
                                            pos, (end - pos));
    }

    return err;
}

/**
 * \brief Copies rest of the block metadata
 *
 * \param[in] lblock  Logical block number to skip
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_mblock_copy_remaining_block_meta(uint32_t lblock)
{
    struct sst_block_metadata block_meta;
    enum psa_sst_err_t err;
    uint32_t meta_block;
    uint32_t pos;
    uint32_t scratch_block;
    uint32_t size;

    scratch_block = sst_meta_cur_meta_scratch();
    meta_block = sst_meta_cur_meta_active();

    if (lblock != SST_LOGICAL_DBLOCK0) {
        /* The objects' data in the logical block 0 is stored in same physical
         * block where the metadata is stored. A change in the metadata requires
         * a swap of physical blocks. So, the physical block ID of logical block
         * 0 needs to be updated to reflect this change, if the object processed
         * is not located in logical block 0. If it is located in block 0,
         * the physical block ID has been updated while processing the object's
         * data.
         */
        err = sst_meta_read_block_metadata(SST_LOGICAL_DBLOCK0, &block_meta);
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }

        /* Update physical ID for logical block 0 to match with the
         * metadata block physical ID.
         */
        block_meta.phys_id = scratch_block;
        err = sst_mblock_update_scratch_block_meta(SST_LOGICAL_DBLOCK0,
                                                   &block_meta);
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }

        /* Copy the rest of metadata blocks between logical block 0 and
         * the logical block provided in the function.
         */
        if (lblock > 1) {
            pos = sst_meta_object_meta_offset(SST_LOGICAL_DBLOCK0 + 1);

            size = sst_meta_object_meta_offset(lblock) - pos;

            /* Copy rest of the block data from previous block */
            /* Data before updated content */
            err = sst_flash_block_to_block_move(scratch_block, pos, meta_block,
                                                pos, size);
            if (err != PSA_SST_ERR_SUCCESS) {
                return err;
            }
        }
    }

    /* Move meta blocks data after updated content */
    pos = sst_meta_block_meta_offset(lblock+1);

    size = sst_meta_object_meta_offset(0) - pos;

    err = sst_flash_block_to_block_move(scratch_block, pos,
                                        meta_block, pos, size);

    return err;
}

/**
 * \brief Reads the object's content
 *
 * \param[in]  meta    Object's metadata
 * \param[in]  offset  Offset in the object
 * \param[in]  size    Size to be read
 * \param[out] buf     Buffer pointer to store the data
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_block_object_read_raw(struct sst_assetmeta *meta,
                                                    uint32_t offset,
                                                    uint32_t size,
                                                    uint8_t *buf)
{
    enum psa_sst_err_t err;
    uint32_t phys_block;
    uint32_t pos;

    phys_block = sst_dblock_lo_to_phy(meta->lblock);
    if (phys_block == SST_BLOCK_INVALID_ID) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }
    pos = (meta->data_index + offset);

    err = sst_flash_read(phys_block, buf, pos, size);

    return err;
}

#ifdef SST_ENCRYPTION
/**
 * \brief Gets the length of metadata to be authenticated
 *        which is total metadata size minus the size of MAC
 *        in the beginning
 *
 * \return Size of autheticated metadata block memory
 */
static uint32_t authenticated_meta_data_size(void)
{
    uint32_t auth_size;

    auth_size = SST_ALL_METADATA_SIZE - SST_NON_AUTH_METADATA_SIZE;

    return auth_size;
}

/**
 * \brief Generates metadata block's authentication and commit to flash
 *
 * \param[in] block_id  ID of the current metadata block to authenticate
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_auth_and_update(uint32_t block_id)
{
    enum psa_sst_err_t err;
    uint32_t size;
    uint8_t *addr;
    union sst_crypto_t *crypto = &sst_system_ctx.meta_block_header.crypto;

    /* Get new IV */
    sst_crypto_get_iv(crypto);

    /* Get size and address of authenticated part of metadata header */
    size = sizeof(struct sst_metadata_block_header)
           - SST_NON_AUTH_METADATA_SIZE;

    addr = (uint8_t *)(&sst_system_ctx.meta_block_header)
           + SST_AUTH_METADATA_OFFSET;

    /* Commit metadata header to flash, except for the non-authenticated part */
    err = sst_flash_write(block_id, addr, SST_AUTH_METADATA_OFFSET, size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    size = authenticated_meta_data_size();
    addr = sst_data_buf;

    /* Read all metadata to be authenticated
     * FIXME: no need to read back metadata header we just wrote.
     */
    err = sst_flash_read(block_id, addr, SST_AUTH_METADATA_OFFSET, size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    err = sst_crypto_getkey(sst_system_ctx.sst_key, SST_KEY_LEN_BYTES);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    err = sst_crypto_setkey(sst_system_ctx.sst_key, SST_KEY_LEN_BYTES);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    err = sst_crypto_generate_auth_tag(crypto, addr, size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Commit non-authenticated part of metadata header to flash */
    err = sst_flash_write(block_id, (uint8_t *)crypto, 0,
                          SST_NON_AUTH_METADATA_SIZE);
    return err;
}

/**
 * \brief Authenticates metadata block
 *
 * \param[in] block  Pointer to authenticated metablock memory
 *
 * \return Return offset value in metadata block
 */
enum psa_sst_err_t sst_mblock_authenticate(uint32_t block)
{
    struct sst_metadata_block_header *metablock_header;
    enum psa_sst_err_t err = 0;
    const uint8_t *addr;
    uint32_t addr_len;

    metablock_header = (struct sst_metadata_block_header *)sst_data_buf;

    /* Read block table and lookups (all metadata and header) */
    err = sst_flash_read(block, (uint8_t *)metablock_header, 0,
                         SST_ALL_METADATA_SIZE);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Address position and size of the meta-data to be authenticated */
    addr = sst_data_buf + SST_AUTH_METADATA_OFFSET;
    addr_len = authenticated_meta_data_size();

    err = sst_crypto_getkey(sst_system_ctx.sst_key, SST_KEY_LEN_BYTES);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    err = sst_crypto_setkey(sst_system_ctx.sst_key, SST_KEY_LEN_BYTES);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Authenticate meta-data */
    err = sst_crypto_authenticate(&metablock_header->crypto, addr, addr_len);

    return err;
}
#endif

/**
 * \brief Checks the validity of the metadata block's swap count
 *
 * \param[in] swap_count  Swap count to validate
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_validate_swap_count(uint8_t swap_count)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SUCCESS;

    /* When a flash block is erased, the default value
     * is usually 0xFF (i.e. all 1s). Since the swap count
     * is updated last (when encryption is disabled), it is
     * possible that due to a power failure, the swap count
     * value in metadata header is 0xFFFF..., which mean
     * it will appear to be most recent block. Which isn't
     * a problem in itself, as the rest of the metadata is fully
     * valid (as it would have been written before swap count).
     * However, this also means that previous update process
     * wasn't complete. So, if the value is 0xFF..., revert
     * back to previous metablock instead.
     */
    if (swap_count == SST_FLASH_DEFAULT_VAL) {
        err = PSA_SST_ERR_SYSTEM_ERROR;
    }

    return err;
}

/**
 * \brief Checks the validatity FS version
 *
 * \param[in] swap_count  Swap count to validate
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_validate_fs_version(uint8_t fs_version)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SUCCESS;

    /* Looks for exact version number.
     * FIXME: backward compatibility could be considered in future revisions.
     */
    if (fs_version != SST_SUPPORTED_VERSION) {
        err = PSA_SST_ERR_SYSTEM_ERROR;
    }

    return err;
}

/**
 * \brief Validates header metadata in order to guarantee that a corruption or
 *        malicious change in stored metadata doesn't result in an invalid
 *        access and the header version is correct.
 *
 * \param[in] meta  Pointer to metadata block header
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_validate_header_meta(
                                         struct sst_metadata_block_header *meta)
{
    enum psa_sst_err_t err;

    err = sst_meta_validate_fs_version(meta->fs_version);
    if (err == PSA_SST_ERR_SUCCESS) {
        err = sst_meta_validate_swap_count(meta->active_swap_count);
    }

    return err;
}

/**
 * \brief Writes the scratch metadata's header
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_write_scratch_meta_header(void)
{
    enum psa_sst_err_t err;
    uint32_t scratch_metablock;
#ifndef SST_ENCRYPTION
    size_t swap_count_offset;
#endif

    scratch_metablock = sst_meta_cur_meta_scratch();

    /* Increment the swap count */
    sst_system_ctx.meta_block_header.active_swap_count += 1;

    err = sst_meta_validate_swap_count(
                        sst_system_ctx.meta_block_header.active_swap_count);
    if (err != PSA_SST_ERR_SUCCESS) {
        /* Reset the swap count to 0 */
        sst_system_ctx.meta_block_header.active_swap_count = 0;
    }

#ifdef SST_ENCRYPTION
    err = sst_meta_auth_and_update(scratch_metablock);
#else
    /* In case of no encryption, the swap count decides the latest metadata
     * block, so it is programmed last.
     */
    swap_count_offset = offsetof(struct sst_metadata_block_header,
                                 active_swap_count);

    /* Write the metadata block header up to the swap count */
    err = sst_flash_write(scratch_metablock,
                          (uint8_t *)(&sst_system_ctx.meta_block_header),
                          0, swap_count_offset);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Write the swap count, the last member in the metadata block header */
    err = sst_flash_write(scratch_metablock,
                          &sst_system_ctx.meta_block_header.active_swap_count,
                          swap_count_offset,
                          sizeof(
                           sst_system_ctx.meta_block_header.active_swap_count));
#endif

    return err;
}

/**
 * \brief Reads the active metadata block header into sst_system_ctx
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_read_meta_header(void)
{
    enum psa_sst_err_t err;

    err = sst_flash_read(sst_system_ctx.active_metablock,
                         (uint8_t *)&sst_system_ctx.meta_block_header, 0,
                         sizeof(struct sst_metadata_block_header));
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    err = sst_meta_validate_header_meta(&sst_system_ctx.meta_block_header);

    return err;
}

/**
 * \brief Writes the objects' data area of logical block 0 into the scratch
 *        block
 *
 * \note The objects' data in the logical block 0 is stored in same physical
 *       block where the metadata is stored. A change in the metadata requires a
 *       swap of physical blocks. So, the objects' data stored in the current
 *       medadata block needs to be copied in the scratch block, unless
 *       the data of the object processed is located in the logical block 0.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_mblock_migrate_data_to_scratch(void)
{
    struct sst_block_metadata block_meta;
    enum psa_sst_err_t err;
    uint32_t current_metablock;
    uint32_t scratch_metablock;
    uint32_t data_size;

    scratch_metablock = sst_meta_cur_meta_scratch();
    current_metablock = sst_meta_cur_meta_active();

    err = sst_meta_read_block_metadata(SST_LOGICAL_DBLOCK0, &block_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Calculate data size stored in the B0 block */
    data_size = (SST_BLOCK_SIZE - block_meta.data_start - block_meta.free_size);

    err = sst_flash_block_to_block_move(scratch_metablock,
                                        block_meta.data_start,
                                        current_metablock,
                                        block_meta.data_start,
                                        data_size);
    return err;
}

/**
 * \brief Finalizes an update operation
 *        Last step when a create/write/delete is performed
 *
 * \param[in] idx  Object's index
 *
 * \return Returns offset value in metadata block
 */
static enum psa_sst_err_t sst_meta_update_finalize(void)
{
    enum psa_sst_err_t err;

    /* Commit the metadata block header to flash */
    err = sst_meta_write_scratch_meta_header();
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }
    /* Update the running context */
    sst_meta_swap_metablocks();
    /* Erase meta block and current scratch block */
    err = sst_meta_erase_scratch_blocks();

    return err;
}

/**
 * \brief Reserves space for an object
 *
 * \param[out] object_meta  Ojbect metadata entry
 * \param[out] block_meta   Block metadata entry
 * \param[in]  size         Size of the object for which space is reserve
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_meta_reserve_object(
                                         struct sst_assetmeta *object_meta,
                                         struct sst_block_metadata *block_meta,
                                         uint32_t size)
{
    uint32_t i;
    enum psa_sst_err_t err;
    uint32_t size_in_flash;

    size_in_flash = sst_get_aligned_flash_bytes(size);

    for (i = 0; i < SST_NUM_ACTIVE_DBLOCKS; i++) {
        err = sst_meta_read_block_metadata(i, block_meta);
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }
        if (block_meta->free_size >= size_in_flash) {
            object_meta->lblock = i;
            object_meta->data_index = SST_BLOCK_SIZE - block_meta->free_size;
            block_meta->free_size -= size_in_flash;
            object_meta->max_size = size;
            return PSA_SST_ERR_SUCCESS;
        }
    }

    /* No block has large enough space to fit the requested object */
    return PSA_SST_ERR_STORAGE_SYSTEM_FULL;
}

/**
 * \brief Gets object index in the object system.
 *
 * \param[in]  obj_uuid  ID of the object
 * \param[out] obj_idx   Index of the object in the object system
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_core_get_object_idx(uint32_t obj_uuid,
                                                  uint32_t *obj_idx)
{
    uint32_t i;
    enum psa_sst_err_t err;
    struct sst_assetmeta tmp_metadata;

    for (i = 0; i < SST_MAX_NUM_OBJECTS; i++) {
        err = sst_meta_read_object_meta(i, &tmp_metadata);
        /* Read from flash failed */
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }

        /* Unique_id with value 0x00 means end of asset meta section */
        if (tmp_metadata.unique_id == obj_uuid) {
            /* Found */
            *obj_idx = i;
            return PSA_SST_ERR_SUCCESS;
        }

    }

    return PSA_SST_ERR_ASSET_NOT_FOUND;
}

enum psa_sst_err_t sst_core_object_exist(uint32_t obj_uuid)
{
    uint32_t idx;
    enum psa_sst_err_t err;

    err = sst_core_get_object_idx(obj_uuid, &idx);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_core_object_create(uint32_t object_uuid, uint32_t size)
{
    uint16_t object_index;
    enum psa_sst_err_t err;
    struct sst_assetmeta object_meta;
    struct sst_block_metadata block_meta;

    err = sst_meta_reserve_object(&object_meta, &block_meta, size);

    object_index = sst_get_free_object_index();
    if ((err != PSA_SST_ERR_SUCCESS) ||
       (object_index == SST_METADATA_INVALID_INDEX)) {
        return PSA_SST_ERR_STORAGE_SYSTEM_FULL;
    }

    object_meta.unique_id = object_uuid;
    object_meta.cur_size = 0;
    object_meta.max_size = size;

    err = sst_mblock_update_scratch_object_meta(object_index, &object_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* If the object is in logical block 0, then update the physical ID to the
     * current scratch metadata block so that it is correct after the metadata
     * blocks are swapped.
     */
    if (object_meta.lblock == SST_LOGICAL_DBLOCK0) {
        block_meta.phys_id = sst_meta_cur_meta_scratch();
    }

    err = sst_mblock_update_scratch_block_meta(object_meta.lblock,
                                               &block_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    err = sst_mblock_copy_remaining_object_meta(object_index);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    err = sst_mblock_copy_remaining_block_meta(object_meta.lblock);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* The objects' data in the logical block 0 is stored in same physical block
     * where the metadata is stored. A change in the metadata requires a
     * swap of physical blocks. So, the objects' data stored in the current
     * metadata block needs to be copied in the scratch block.
     */
    err = sst_mblock_migrate_data_to_scratch();
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Write metadata header, swap metadata blocks and erase scratch blocks */
    err = sst_meta_update_finalize();
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_core_object_get_info(uint32_t object_uuid,
                                            struct sst_core_obj_info_t *info)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SYSTEM_ERROR;
    struct sst_assetmeta tmp_metadata;
    uint32_t object_index;

    /* Get the meta data index */
    err = sst_core_get_object_idx(object_uuid, &object_index);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Read object metadata */
    err = sst_meta_read_object_meta(object_index, &tmp_metadata);
    if (err == 0) {
        /* Check if index is still referring to same object */
        if (object_uuid != tmp_metadata.unique_id) {
            err = PSA_SST_ERR_ASSET_NOT_FOUND;
        } else {
            info->size_max = tmp_metadata.max_size;
            info->size_current = tmp_metadata.cur_size;
        }
    }

    return err;
}

enum psa_sst_err_t sst_core_object_write(uint32_t object_uuid,
                                         const uint8_t *data, uint32_t offset,
                                         uint32_t size)
{
    uint32_t object_index;
    enum psa_sst_err_t err;
    uint32_t cur_phys_block;
    const uint8_t *prepared_buf;
    struct sst_assetmeta object_meta;
    struct sst_block_metadata block_meta;
    uint32_t align_flash_nbr_bytes;
#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    (void)offset;
#endif

    /* Get the object index */
    err = sst_core_get_object_idx(object_uuid, &object_index);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Read object metadata */
    err = sst_meta_read_object_meta(object_index, &object_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Update block metadata */
    err = sst_meta_read_block_metadata(object_meta.lblock, &block_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
    /* offset can not be bigger than the current asset's size to disallows gaps
     * without content inside the asset.
     */
    if (offset > object_meta.cur_size) {
        return PSA_SST_ERR_PARAM_ERROR;
    }
#endif

#ifndef SST_ENCRYPTION
    prepared_buf = sst_buf_plain_text;

    /* Clean previous data in sst_buf_plain_text */
    sst_utils_memset(sst_buf_plain_text, SST_DEFAULT_EMPTY_BUFF_VAL,
                     SST_ALIGNED_MAX_OBJECT_SIZE);

    if (object_meta.cur_size > 0) {
        /* Copy the current asset's data into the sst_buf_plain_text buffer. */
        err = sst_block_object_read_raw(&object_meta,
                                        SST_OBJECT_START_POSITION,
                                        object_meta.cur_size,
                                        sst_buf_plain_text);
        if (err != PSA_SST_ERR_SUCCESS) {
            return err;
        }
    }

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
    /* sst_am_write has checked that offset + size value is not bigger than
     * the asset's maximum size. So, it is not needed to check it at this
     * point.
     */
    if ((offset + size) > object_meta.cur_size) {
        /* Update the object metadata */
        object_meta.cur_size = offset + size;
    }

    /* Copy new data in the sst_buf_plain_text */
    sst_utils_memcpy(sst_buf_plain_text + offset, data, size);
#else
    if (size > object_meta.cur_size) {
        /* Update the object metadata */
        object_meta.cur_size = size;
    }

    /* Copy new data in the sst_buf_plain_text */
    sst_utils_memcpy(sst_buf_plain_text, data, size);
#endif /* SST_ENABLE_PARTIAL_ASSET_RW */

#else
    prepared_buf = data;
    object_meta.cur_size = size;

#endif /* SST_ENCRYPTION */

    align_flash_nbr_bytes = sst_get_aligned_flash_bytes(object_meta.cur_size);

    /* Copy the content into scratch data buffer */
    err = sst_dblock_update_scratch(object_meta.lblock, &block_meta,
                                    prepared_buf, object_meta.data_index,
                                    align_flash_nbr_bytes);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    cur_phys_block = block_meta.phys_id;
    /* Cur scratch block become the active datablock */
    block_meta.phys_id = sst_meta_cur_data_scratch(object_meta.lblock);
    /* swap the scratch data block */
    sst_meta_set_data_scratch(cur_phys_block, object_meta.lblock);

    /* Update block metadata in scratch metadata block */
    err = sst_mblock_update_scratch_block_meta(object_meta.lblock, &block_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Update object's metadata to reflect new attributes */
    err = sst_mblock_update_scratch_object_meta(object_index, &object_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Copy rest of the block metadata entries */
    err = sst_mblock_copy_remaining_block_meta(object_meta.lblock);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Copy rest of the object metadata entries */
    err = sst_mblock_copy_remaining_object_meta(object_index);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* The objects' data in the logical block 0 is stored in same physical block
     * where the metadata is stored. A change in the metadata requires a
     * swap of physical blocks. So, the objects' data stored in the current
     * metadata block needs to be copied in the scratch block, if the data
     * of the object processed is not located in the logical block 0. When an
     * object's data is located in the logical block 0, that copy has been done
     * while processing the object's data.
     */
    if (object_meta.lblock != SST_LOGICAL_DBLOCK0) {
        err = sst_mblock_migrate_data_to_scratch();
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }
    }

    /* Update the metablock header, swap scratch and active blocks,
     * erase scratch blocks.
     */
    err = sst_meta_update_finalize();

    return err;
}

/**
 * \brief Compacts data block for the given logical block.
 *
 * \param[in] lblock      Logical block to compact
 * \param[in] obj_size    Available data size to compact
 * \param[in] src_offset  Offset in the current data block which points to the
 *                        data positon to realocate
 * \param[in] dst_offset  Offset in the scratch block which points to the
 *                        data position to store the data to be realocated
 * \param[in] size        Number of bytes to be realocated
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_compact_dblock(uint32_t lblock, uint32_t obj_size,
                                       uint32_t src_offset, uint32_t dst_offset,
                                       uint32_t size)
{
    struct sst_block_metadata block_meta;
    enum psa_sst_err_t err;
    uint32_t scratch_dblock_id = 0;

    /* Read current block meta */
    err = sst_meta_read_block_metadata(lblock, &block_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Release data from block meta */
    block_meta.free_size += obj_size;

    /* Save scratch data block physical IDs */
    scratch_dblock_id = sst_meta_cur_data_scratch(lblock);

    /* Check if there are bytes to be compacted */
    if (size > 0) {
        /* Move data from source offset in current data block to scratch block
         * destination offset.
         */
        err = sst_flash_block_to_block_move(scratch_dblock_id, dst_offset,
                                            block_meta.phys_id, src_offset,
                                            size);
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }
    }

    if (dst_offset > block_meta.data_start) {
        /* Copy data from the beginning of data block until
         * the position where the data will be realocated later
         */
        err = sst_flash_block_to_block_move(scratch_dblock_id,
                                            block_meta.data_start,
                                            block_meta.phys_id,
                                            block_meta.data_start,
                                            (dst_offset-block_meta.data_start));
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }
    }

    /* Swap the scratch and current data blocks. Must swap even with nothing
     * to compact so that deleted object is left in scratch and erased as part
     * of finalization.
     */
    sst_meta_set_data_scratch(block_meta.phys_id, lblock);

    /* Set scratch block ID as the one which contains the new data block */
    block_meta.phys_id = scratch_dblock_id;

    /* Update block metadata in scratch metadata block */
    err = sst_mblock_update_scratch_block_meta(lblock, &block_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        /* Swap back the data block as there was an issue in the process */
        sst_meta_set_data_scratch(scratch_dblock_id, lblock);
        return err;
    }

    /* Copy rest of the block metadata entries */
    err = sst_mblock_copy_remaining_block_meta(lblock);
    if (err != PSA_SST_ERR_SUCCESS) {
        /* Swap back the data block as there was an issue in the process */
        sst_meta_set_data_scratch(scratch_dblock_id, lblock);
    }

    return err;
}

enum psa_sst_err_t sst_core_object_delete(uint32_t object_uuid)
{
    uint32_t del_obj_data_index;
    uint32_t del_obj_lblock;
    uint32_t del_obj_index;
    uint32_t del_obj_max_size;
    enum psa_sst_err_t err;
    uint32_t src_offset = SST_BLOCK_SIZE;
    uint32_t nbr_bytes_to_move = 0;
    uint32_t obj_idx;
    struct sst_assetmeta object_meta;

    /* Get the object index */
    err = sst_core_get_object_idx(object_uuid, &del_obj_index);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_meta_read_object_meta(del_obj_index, &object_meta);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    if (sst_utils_validate_uuid(object_meta.unique_id) != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Save logical block, data_index and max_size to be used later on */
    del_obj_lblock = object_meta.lblock;
    del_obj_data_index = object_meta.data_index;
    del_obj_max_size = sst_get_aligned_flash_bytes(object_meta.max_size);

    /* Remove object metadata */
    object_meta.unique_id = SST_INVALID_UUID;
    object_meta.lblock = 0;
    object_meta.max_size = 0;
    object_meta.cur_size = 0;

    /* Update object's metadata in to the scratch block */
    sst_mblock_update_scratch_object_meta(del_obj_index, &object_meta);

    /* Read all object metadata */
    for (obj_idx = 0; obj_idx < SST_MAX_NUM_OBJECTS; obj_idx++) {
        if (obj_idx == del_obj_index) {
            /* Skip deleted object */
            continue;
        }

        /* Read object meta for the given object index */
        err = sst_meta_read_object_meta(obj_idx, &object_meta);
        if (err != PSA_SST_ERR_SUCCESS) {
            return err;
        }

        /* Check if the object is located in the same logical block */
        if (object_meta.lblock == del_obj_lblock) {
            /* If an object is located after the data to delete, this
             * needs to be moved.
             */
            if (object_meta.data_index > del_obj_data_index) {
                /* Check if this is the position after the deleted
                 * data. This will be the first asset's data to move.
                 */
                if (src_offset > object_meta.data_index) {
                    src_offset = object_meta.data_index;
                }

                /* Set the new object data index location in the
                 * data block.
                 */
                object_meta.data_index -= del_obj_max_size;

                /* Increase number of bytes to move */
                nbr_bytes_to_move += sst_get_aligned_flash_bytes(
                                                          object_meta.max_size);
            }
        }
        /* Update object's metadata in to the scratch block */
        sst_mblock_update_scratch_object_meta(obj_idx, &object_meta);
    }

    /* Compact data block */
    err = sst_compact_dblock(del_obj_lblock, del_obj_max_size, src_offset,
                             del_obj_data_index, nbr_bytes_to_move);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* The objects' data in the logical block 0 is stored in same physical block
     * where the metadata is stored. A change in the metadata requires a
     * swap of physical blocks. So, the objects' data stored in the current
     * metadata block needs to be copied in the scratch block, if the data
     * of the object processed is not located in the logical block 0. When an
     * object's data is located in the logical block 0, that copy has been done
     * while processing the object's data.
     */
    if (del_obj_lblock != SST_LOGICAL_DBLOCK0) {
        err = sst_mblock_migrate_data_to_scratch();
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }
    }

    /* Update the metablock header, swap scratch and active blocks,
     * erase scratch blocks.
     */
    sst_meta_update_finalize();

    return err;
}

enum psa_sst_err_t sst_core_object_read(uint32_t object_uuid, uint8_t *data,
                                        uint32_t offset, uint32_t size)
{
    uint32_t object_index;
    enum psa_sst_err_t err;
    struct sst_assetmeta tmp_metadata;

    /* Get the object index */
    err = sst_core_get_object_idx(object_uuid, &object_index);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Read object metadata */
    err = sst_meta_read_object_meta(object_index, &tmp_metadata);

    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Check if index is still referring to same asset */
    if (object_uuid != tmp_metadata.unique_id) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Boundary check the incoming request */
    err = sst_utils_check_contained_in(0, tmp_metadata.cur_size,
                                       offset, size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    /* Read the object from flash */
    err = sst_block_object_read_raw(&tmp_metadata, offset, size, data);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    return PSA_SST_ERR_SUCCESS;
}

/**
 * \brief Validates and find the valid-active metablock
 *
 * \return Returns value as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_init_get_active_metablock(void)
{
    struct sst_metadata_block_header meta0;
    struct sst_metadata_block_header meta1;
    uint32_t cur_meta_block;
    enum psa_sst_err_t err;
#if SST_ENCRYPTION
    uint32_t i;
#endif
    uint8_t num_valid_meta_blocks = 0;
    /* First two blocks are reserved for metadata.
     * find out the valid one.
     */

    /* Read the header of both the metdata blocks */
    err = sst_flash_read(SST_METADATA_BLOCK0, (uint8_t *)&meta0,
                         0, sizeof(struct sst_metadata_block_header));
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    err = sst_flash_read(SST_METADATA_BLOCK1, (uint8_t *) &meta1,
                         0, sizeof(struct sst_metadata_block_header));
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* If there are two potential active metadata blocks,
     * an out of turn power down sequence didn't allow previous
     * update operation to complete. Need to find out the valid
     * metadata block now.
     */
    if (sst_meta_validate_header_meta(&meta0) == PSA_SST_ERR_SUCCESS) {
        num_valid_meta_blocks++;
        cur_meta_block = SST_METADATA_BLOCK0;
    }

    if (sst_meta_validate_header_meta(&meta1) == PSA_SST_ERR_SUCCESS) {
        num_valid_meta_blocks++;
        cur_meta_block = SST_METADATA_BLOCK1;
    }

    /* If there are more than 1 potential metablocks, the previous
     * update operation was interrupted by power failure. In which case,
     * need to find out which one is potentially latest metablock.
     */
    if (num_valid_meta_blocks > 1) {
        cur_meta_block = sst_meta_latest_meta_block(&meta0, &meta1);
    } else if (num_valid_meta_blocks == 0) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

#if SST_ENCRYPTION
    /* If authentication is enabled, MAC is programmed last.
     * So, if both of the blocks 'appear' valid, authentication
     * of metadata entries decides valid metadata.
     */
    for (i = 0; i < num_valid_meta_blocks; i++) {
        err = sst_mblock_authenticate(cur_meta_block);
        if (err == PSA_SST_ERR_SUCCESS) {
             /* Valid metablock found, stop here */
            err = PSA_SST_ERR_SUCCESS;
            break;
        } else {
            /* Primary candidate for valid metadata content failed
             * authentication. Try other one.
             */
            err = PSA_SST_ERR_SYSTEM_ERROR;
            cur_meta_block = SST_OTHER_META_BLOCK(cur_meta_block);
        }
    }

    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }
#endif
    sst_system_ctx.active_metablock = cur_meta_block;
    sst_system_ctx.scratch_metablock = SST_OTHER_META_BLOCK(cur_meta_block);

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_core_prepare(void)
{
    enum psa_sst_err_t err;

    /* Initialize Flash Interface */
    err = sst_flash_init();
    if(err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

#if SST_ENCRYPTION
    sst_crypto_init();
#endif

    err = sst_init_get_active_metablock();
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    err = sst_meta_read_meta_header();
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

#if SST_ENCRYPTION
    /* Provide the current IV value to the crypto layer */
    sst_crypto_set_iv(&sst_system_ctx.meta_block_header.crypto);
#endif

    /* Erase the other scratch metadata block */
    err = sst_meta_erase_scratch_blocks();

    return err;
}

enum psa_sst_err_t sst_core_wipe_all(void)
{
    uint32_t i;
    enum psa_sst_err_t err;
    uint32_t metablock_to_erase_first = SST_METADATA_BLOCK0;
    struct sst_block_metadata block_meta;
    struct sst_assetmeta object_metadata;

#if SST_ENCRYPTION
    sst_crypto_init();
#endif

    /* Erase both metadata blocks. If at least one metadata block is valid,
     * ensure that the active metadata block is erased last to prevent rollback
     * in the case of a power failure between the two erases.
     */
    if (sst_init_get_active_metablock() == PSA_SST_ERR_SUCCESS) {
        metablock_to_erase_first = sst_system_ctx.scratch_metablock;
    }

    err = sst_flash_erase_block(metablock_to_erase_first);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    err = sst_flash_erase_block(SST_OTHER_META_BLOCK(metablock_to_erase_first));
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    sst_system_ctx.meta_block_header.active_swap_count = 0;
    sst_system_ctx.meta_block_header.scratch_idx = SST_INIT_SCRATCH_DBLOCK;
    sst_system_ctx.meta_block_header.fs_version = SST_SUPPORTED_VERSION;
    sst_system_ctx.scratch_metablock = SST_METADATA_BLOCK1;
    sst_system_ctx.active_metablock = SST_METADATA_BLOCK0;

    /* Fill the block metadata for logical datablock 0, which has the physical
     * id of the active metadata block. For this datablock, the space available
     * for data is from the end of the metadata to the end of the block.
     */
    block_meta.data_start = SST_ALL_METADATA_SIZE;
    block_meta.free_size = (SST_BLOCK_SIZE - block_meta.data_start);
    block_meta.phys_id = SST_METADATA_BLOCK0;
    sst_mblock_update_scratch_block_meta(SST_LOGICAL_DBLOCK0, &block_meta);

    /* Fill the block metadata for the dedicated datablocks, which have logical
     * ids beginning from 1 and physical ids initially beginning from
     * SST_INIT_DBLOCK_START. For these datablocks, the space available for
     * data is the entire block.
     */
    block_meta.data_start = 0;
    block_meta.free_size = SST_BLOCK_SIZE;
    for (i = SST_INIT_DBLOCK_START; i < SST_NUM_DEDICATED_DBLOCKS; i++) {
        /* If a flash error is detected, the code erases the rest
         * of the blocks anyway to remove all data stored in them.
         */
        err |= sst_flash_erase_block(i);
    }

    /* If an error is detected while erasing the flash, then return a
     * system error to abort core wipe process.
     */
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    for (i = 0; i < SST_NUM_DEDICATED_DBLOCKS; i++) {
        block_meta.phys_id = i + SST_INIT_DBLOCK_START;
        err = sst_mblock_update_scratch_block_meta(i + 1, &block_meta);
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }
    }

    /* Initialize object metadata table */
    sst_utils_memset(&object_metadata, SST_DEFAULT_EMPTY_BUFF_VAL,
                     sizeof(struct sst_assetmeta));
    for (i = 0; i < SST_MAX_NUM_OBJECTS; i++) {
        /* In the beginning phys id is same as logical id */
        /* Update object's metadata to reflect new attributes */
        err = sst_mblock_update_scratch_object_meta(i, &object_metadata);
        if (err != PSA_SST_ERR_SUCCESS) {
            return PSA_SST_ERR_SYSTEM_ERROR;
        }
    }

    /* FIXME: erase all the blocks first */
    err = sst_meta_write_scratch_meta_header();
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    /* Swap active and scratch metablocks */
    sst_meta_swap_metablocks();

    return PSA_SST_ERR_SUCCESS;
}
