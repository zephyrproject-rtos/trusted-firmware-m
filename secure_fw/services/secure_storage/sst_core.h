/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
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
    uint32_t cur_size;    /*!< Size in storage system for this fragment */
    uint32_t max_size;    /*!< Maximum size of this asset */
    uint32_t unique_id;   /*!< Unique ID of this asset, compiler generated
                           *   based on user defined user_defs.h
                           */
};

struct sst_asset_system_context {
    struct sst_metadata_block_header meta_block_header; /*!< Block metadata
                                                         *   header
                                                         */
    uint32_t active_metablock;           /*!< Active metadata block */
    uint32_t scratch_metablock;          /*!< Scratch meta block */
    uint8_t  sst_key[SST_KEY_LEN_BYTES]; /*!< Secure storage system key */
};

/* Structure to store the core object information concerning the content
 * information.
 */
struct sst_core_obj_info_t {
    uint32_t size_current; /*!< The current size of the flash object data */
    uint32_t size_max;     /*!< The maximum size of the flash object data in
                            *   bytes.
                            */
};

/**
 * \brief Prepares the core. Authenticates/validates the metadata.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_core_prepare(void);

/**
 * \brief Checks if an object exists.
 *
 * \param[in]  obj_uuid  Unique identifier for the object
 *
 * \return Returns PSA_SST_ERR_SUCCESS if object exist.  If object does not
 *         exist, it returns PSA_SST_ERR_ASSET_NOT_FOUND. If SST area is not
 *         prepared, it returns PSA_SST_ERR_ASSET_NOT_PREPARED.
 */
enum psa_sst_err_t sst_core_object_exist(uint32_t obj_uuid);

/**
 * \brief Creates an object in the storage area.
 *
 * \param[in] obj_uuid  Unique identifier for the object
 * \param[in] size      Size of the object to be created
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_core_object_create(uint32_t obj_uuid, uint32_t size);

/**
 * \brief Gets the object information referenced by object UUID.
 *
 * \param[in]  obj_uuid  Unique identifier of the object
 * \param[out] info      Pointer to the information structure to store the
 *                       object information values \ref sst_core_obj_info_t
 *
 * \return Returns error code specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_core_object_get_info(uint32_t obj_uuid,
                                            struct sst_core_obj_info_t *info);

/**
 * \brief Writes data to an existing object.
 *
 * \param[in] obj_uuid  Unique identifier of the object
 * \param[in] data      Pointer to buf containing data to be written
 * \param[in] offset    Offset in the object
 * \param[in] size      Size of the incoming buffer
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_core_object_write(uint32_t obj_uuid,
                                         const uint8_t *data,
                                         uint32_t offset, uint32_t size);

/**
 * \brief Reads an object or part of it to given buffer.
 *
 * \param[in]  obj_uuid  Unique identifier of the object
 * \param[out] data      Pointer to buf
 * \param[in]  offset    Offset in the object
 * \param[in]  size      Size to be read
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_core_object_read(uint32_t obj_uuid, uint8_t *data,
                                        uint32_t offset, uint32_t size);

/**
 * \brief Deletes object referred by the UUID.
 *
 * \param[in] obj_uuid  Unique identifier of the object
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_core_object_delete(uint32_t obj_uuid);

/**
 * \brief Reads metadata associated with an object.
 *
 * \param[in]  object_index Index of the ojbect to be read
 * \param[out] meta         Pointer to meta buffer to read values
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
enum psa_sst_err_t sst_meta_read_object_meta(uint32_t object_index,
                                             struct sst_assetmeta *meta);

/**
 * \brief Wipes all the storage and regenerate metadata block.
 *
 * \return Returns error code as specified in \ref sst_errno_t
 */
enum psa_sst_err_t sst_core_wipe_all(void);

#ifdef __cplusplus
}
#endif

#endif /* __SST_CORE_H__ */
