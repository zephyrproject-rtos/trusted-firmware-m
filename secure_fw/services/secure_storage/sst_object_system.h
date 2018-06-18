/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_OBJECT_SYSTEM_H__
#define __SST_OBJECT_SYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "tfm_sst_defs.h"

/**
 * \brief Prepares the secure storage system for usage, populating internal
 *        structures.
 *        It identifies and validates the system metadata.
 *
 * \return Returns TFM_SST_ERR_SUCCESS or TFM_SST_ERR_INIT_FAILED
 */
enum tfm_sst_err_t sst_system_prepare(void);

/**
 * \brief Creates a new object with given object UUID.
 *
 * \param[in] uuid  Object UUID
 * \param[in] type  Object type
 * \param[in] size  Object size
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_create(uint32_t uuid,
                                     uint32_t type, uint32_t size);

/**
 * \brief Gets object's data referenced by object UUID, and stores it
 *        in the data buffer.
 *
 * \param[in]  uuid    Object UUID
 * \param[out] data    Data buffer to store the object data
 * \param[in]  offset  Offset from where the read is going to start
 * \param[in]  size    Data buffer size
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_read(uint32_t uuid,
                                   uint8_t *data, uint32_t offset,
                                   uint32_t size);
/**
 * \brief Writes data into the object referenced by object UUID.
 *
 * \param[in] uuid    Object UUID
 * \param[in] data    Data buffer to write into object object
 * \param[in] offset  Offset from where the write is going to start
 * \param[in] size    Data buffer size
 *
 * \return Returns number of bytes read or a relevant error \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_write(uint32_t uuid,
                                    const uint8_t *data, uint32_t offset,
                                    uint32_t size);
/**
 * \brief Deletes the object referenced by object UUID.
 *
 * \param[in] uuid  Object UUID
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_delete(uint32_t uuid);

/**
 * \brief Gets the object information referenced by object UUID.
 *
 * \param[in]  uuid  Object UUID
 * \param[out] info  Pointer to store the object's information
 *                   \ref struct tfm_sst_asset_info_t
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_get_info(uint32_t uuid,
                                       struct tfm_sst_asset_info_t *info);

/**
 * \brief Gets the object attributes referenced by object UUID.
 *
 * \param[in]  uuid   Object UUID
 * \param[out] attrs  Pointer to store the object's attributes
 *                    \ref tfm_sst_asset_attrs_t
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_get_attributes(uint32_t uuid,
                                           struct tfm_sst_asset_attrs_t *attrs);
/**
 * \brief Sets the specific object attributes referenced by object UUID.
 *
 * \param[in] uuid   Object UUID \ref tfm_sst_asset_id_t
 * \param[in] attrs  Pointer to new the object's attributes
 *                   \ref tfm_sst_asset_attrs_t
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_set_attributes(uint32_t uuid,
                                     const struct tfm_sst_asset_attrs_t *attrs);

/**
 * \brief Wipes secure storage system and all object data.
 *
 * \return Returns error code specified in \ref sst_errno_t
 */
enum tfm_sst_err_t sst_system_wipe_all(void);

#ifdef __cplusplus
}
#endif

#endif /* __SST_OBJECT_SYSTEM_H__ */
