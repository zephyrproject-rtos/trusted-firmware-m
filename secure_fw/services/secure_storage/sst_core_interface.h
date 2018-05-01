/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_CORE_INTERFACE_H__
#define __SST_CORE_INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "tfm_sst_defs.h"

/**
 * \brief Prepares the storage system for usage, populating internal structures.
 *        It identifies and validates the system metadata.
 *
 * \return Returns TFM_SST_ERR_SUCCESS or TFM_SST_ERR_INIT_FAILED
 */
enum tfm_sst_err_t sst_object_prepare(void);

/**
 * \brief Gets handler for the given asset uuid. If an asset is deleted, the
 *        linked asset handle reference is no longer valid and will give
 *        TFM_SST_ERR_ASSET_REF_INVALID if used.
 *
 * \param[in]  asset_uuid  Asset UUID
 * \param[out] hdl         handle if successful
 *
 * \return Returns asset handle. If asset is not found, it returns
 *         TFM_SST_ERR_ASSET_NOT_FOUND. If SST area is not prepared, it returns
 *         TFM_SST_ERR_ASSET_NOT_PREPARED.
 */
enum tfm_sst_err_t sst_object_handle(uint16_t asset_uuid, uint32_t *hdl);

/**
 * \brief Creates a new asset with given uuid.
 *
 * \param[in] asset_uuid  Asset UUID
 * \param[in] size        object size
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_create(uint16_t asset_uuid, uint32_t size);

/**
 * \brief Gets asset's data referenced by asset handle, and stores it
 *        in the data buffer.
 *
 * \param[in]  asset_handle  Asset handler
 * \param[out] data          Data buffer to store the asset data
 * \param[in]  offset        Offset from where the read is going to start
 * \param[in]  size          Data buffer size
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_read(uint32_t asset_handle, uint8_t *data,
                                   uint32_t offset, uint32_t size);

/**
 * \brief Writes data into the asset referenced by asset uuid.
 *
 * \param[in] asset_handl  Asset handle
 * \param[in] data         Data buffer to write into asset object
 * \param[in] offset       Offset from where the write is going to start
 * \param[in] size         Data buffer size
 *
 * \return Returns number of bytes read or a relevant error \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_write(uint32_t asset_handle, const uint8_t *data,
                                    uint32_t offset, uint32_t size);

/**
 * \brief Deletes the asset referenced by asset handler.
 *
 * \param[in] asset_handle  Asset handler
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_delete(uint32_t asset_handle);

/**
 * \brief Gets the asset attributes referenced by asset handler.
 *
 * \param[in]  asset_handle  Asset handler
 * \param[out] attributes    Pointer to the attributes structure to store the
 *                           attributes values
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_get_attributes(uint32_t asset_handle,
                                          struct tfm_sst_attribs_t *attributes);

/**
 * \brief Wipes storage system and all asset data.
 *
 * \return Returns error code specified in \ref sst_errno_t
 */
enum tfm_sst_err_t sst_object_wipe_all(void);

#ifdef __cplusplus
}
#endif

#endif /* __SST_CORE_INTERFACE_H__ */
