/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SST_API__
#define __TFM_SST_API__

#ifdef __cplusplus
extern "C" {
#endif

#include "tfm_sst_defs.h"

/**
 * \brief Allocates space for the asset, referenced by asset UUID,
 *        without setting any data in the asset.
 *
 * \param[in] asset_uuid  Asset UUID
 *
 * \return Returns an TFM_SST_ERR_SUCCESS if asset is created correctly.
 *         Otherwise, error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t  tfm_sst_create(uint32_t asset_uuid);

/**
 * \brief Gets asset's information referenced by asset UUID.
 *
 * \param[in]  asset_uuid  Asset UUID
 * \param[out] info        Pointer to store the asset's information
 *                         \ref tfm_sst_asset_info_t
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t tfm_sst_get_info(uint32_t asset_uuid,
                                    struct tfm_sst_asset_info_t *info);

/**
 * \brief Gets asset's attributes referenced by asset UUID.
 *
 * \param[in]  asset_uuid  Asset UUID
 * \param[out] attrs       Pointer to store the asset's attributes
 *                         \ref tfm_sst_asset_attrs_t
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t tfm_sst_get_attributes(uint32_t asset_uuid,
                                          struct tfm_sst_asset_attrs_t *attrs);

/**
 * \brief Sets asset's attributes referenced by asset UUID.
 *
 * \param[in] asset_uuid  Asset UUID
 * \param[in] attrs       Pointer to new the asset's attributes
 *                        \ref tfm_sst_asset_attrs_t
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t tfm_sst_set_attributes(
                                     uint32_t asset_uuid,
                                     const struct tfm_sst_asset_attrs_t *attrs);

/**
 * \brief Reads asset's data from asset referenced by asset UUID.
 *
 * \param[in]  asset_uuid  Asset UUID
 * \param[out] data        Pointer to data vector \ref tfm_sst_buf_t to store
 *                         data, size and offset
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t tfm_sst_read(uint32_t asset_uuid,
                                struct tfm_sst_buf_t* data);

/**
 * \brief Writes data into an asset referenced by asset UUID.
 *
 * \param[in] asset_uuid  Asset UUID
 * \param[in] data        Pointer to data vector \ref tfm_sst_buf_t which
 *                        contains the data to write
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t tfm_sst_write(uint32_t asset_uuid,
                                 struct tfm_sst_buf_t* data);

/**
 * \brief Deletes the asset referenced by the asset UUID.
 *
 * \param[in] asset_uuid  Asset UUID
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t tfm_sst_delete(uint32_t asset_uuid);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SST_API__ */
