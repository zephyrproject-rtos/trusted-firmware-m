/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_CORE_H__
#define __SST_CORE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_sst_defs.h"
#include "crypto/sst_crypto_interface.h"
#include "assets/sst_asset_defs.h"

#define SST_SUPPORTED_VERSION   0x01

#define SST_OTHER_META_BLOCK(metablock) \
((metablock == SST_METADATA_BLOCK0) ? \
(SST_METADATA_BLOCK1) : (SST_METADATA_BLOCK0))

/* Set to 0 to disable block 0 authentication */
#define SST_B0_AUTH 0
/* Set to 0 to disable data encryption */
#define SST_DATA_ENCRYPT 0
/* Return value when metadata table is full */
#define SST_METADATA_INVALID_INDEX 0xFFFF

/* Physical ID of the two metadata blocks */
/* NOTE: the earmarked area may not always start at
 * block number 0. however, the flash interface
 * can always add the required offset.
 */
#define SST_METADATA_BLOCK0 0
#define SST_METADATA_BLOCK1 1

/* Structure to store information about each physical flash memory block */
struct sst_block_metadata {
    uint32_t phys_id;     /*!< Physical id of this logical block */
    uint32_t data_start;  /*!< Offset from the beginning of the
                           *   block to the location where the data
                           *   starts
                           */
    uint32_t free_size;   /*!< Number of bytes free at end of
                           *   block (set during block compaction
                           *   for gap reuse)
                           */
};

/* Structure to store the metadata block header. The active_swap_count must be
 * the last member to allow it to be programmed last. The tag value needs to be
 * on the top to allow authentication of the metadata block without having to
 * copy/reorganize the authenticated content, rest of the ordering is done for
 * better data packing.
 */
struct sst_metadata_block_header {
    union sst_crypto_t crypto;      /*!< Crypto metadata */
    uint32_t scratch_idx;           /*!< Physical block ID of the data
                                     *   section's scratch block
                                     */
    uint8_t fs_version;             /*!< SST system version. Currently
                                     *   0x01
                                     */
    uint8_t active_swap_count;      /*!< Physical block ID of the data */
};


/* Object metadata associated with each object */
struct sst_assetmeta {
    uint32_t lblock;      /*!< Logical datablock where object is stored */
    uint32_t data_index;  /*!< Offset in the logical data block */
    uint16_t cur_size;    /*!< Size in storage system for this fragment */
    uint16_t max_size;    /*!< Maximum size of this asset */
    uint16_t unique_id;   /*!< Unique ID of this asset, compiler generated
                           *   based on user defined user_defs.h
                           */
    union sst_crypto_t crypto; /*!< Crypto metadata */
};

struct sst_asset_system_context {
    struct sst_metadata_block_header meta_block_header; /*!< Block metadata
                                                         *   header
                                                         */
    uint32_t active_metablock;           /*!< Active metadata block */
    uint32_t scratch_metablock;          /*!< Scratch meta block */
    uint8_t  sst_key[SST_KEY_LEN_BYTES]; /*!< Secure storage system key */
};


/**
 * \brief Converts an asset uuid into an assethandle, looking up the index in
 *        the metadata table for the uuid specified.
 *
 * \param[in]  asset_uuid  Asset UUID
 * \param[out] hdl         Handle to be returned
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t, and
 *         asset handle is populated in the pointer passed if
 *         the function succeeds.
 */
enum tfm_sst_err_t sst_core_object_handle(uint16_t asset_uuid, uint32_t *hdl);

/**
 * \brief Creates an object in the storage
 *        If the object doesn't exist already and there is enough
 *        space avaailable, a metadata entry and
 *        space in data block is reserved.
 *
 * \param[in] uuid  Unique identifier for the object
 * \param[in] size  Size of the object to be created
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_core_object_create(uint16_t uuid, uint32_t size);

/**
 * \brief Writes data to an existing object
 *        Requested buffer is written into give offset of the object,
 *        provided boundary checks don't fail
 *
 * \param[in] asset_handle  Handle of the object
 * \param[in] data          Pointer to buf containing data to be written
 * \param[in] offset        Offset in the object
 * \param[in] size          Size of the incoming buffer
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_core_object_write(uint32_t asset_handle,
                                         const uint8_t *data,
                                         uint32_t offset, uint32_t size);

/**
 * \brief Prepares the core. Authenticates/validates the metadata
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_core_prepare(void);

/**
 * \brief Reads and object or part of it to given buffer
 *        Requested object is read into give buffer,
 *        provided boundary checks don't fail
 *
 * \param[in]  asset_handle  Handle of the object
 * \param[out] data          Pointer to buf
 * \param[in]  offset        Offset in the object
 * \param[in]  size          Size to be read
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_core_object_read(uint32_t asset_handle, uint8_t *data,
                                        uint32_t offset, uint32_t size);

/**
 * \brief Deletes object referred by the handle
 *
 * \param[in] asset_handle  Handle of the object
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_core_object_delete(uint32_t asset_handle);

/**
 * \brief Reads metadata associated with an object
 *
 * \param[in]  object_index Index of the ojbect to be read
 * \param[out] meta         Pointer to meta buffer to read values
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_meta_read_object_meta(uint32_t object_index,
                                             struct sst_assetmeta *meta);

/**
 * \brief wipe the storage and regenerate metadata block
 *
 * \return Returns error code as specified in \ref sst_errno_t
 */
enum tfm_sst_err_t sst_core_wipe_all(void);

#ifdef __cplusplus
}
#endif

#endif /* SST_METADATA_H_ */
