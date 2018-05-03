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
 * \brief Gets handler for the given object uuid. If an object is deleted, the
 *        linked object handle reference is no longer valid and will give
 *        TFM_SST_ERR_ASSET_REF_INVALID if used.
 *
 * \param[in]  object_uuid  Object UUID
 * \param[out] hdl          Handle if successful
 *
 * \return Returns object handle. If object is not found, it returns
 *         TFM_SST_ERR_ASSET_NOT_FOUND. If SST area is not prepared, it returns
 *         TFM_SST_ERR_ASSET_NOT_PREPARED.
 */
enum tfm_sst_err_t sst_object_handle(uint16_t object_uuid, uint32_t *hdl);

/**
 * \brief Creates a new object with given uuid.
 *
 * \param[in] object_uuid  Object UUID
 * \param[in] size         Object size
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_create(uint16_t object_uuid, uint32_t size);

/**
 * \brief Gets object's data referenced by object handle, and stores it
 *        in the data buffer.
 *
 * \param[in]  object_handle  Object handler
 * \param[out] data           Data buffer to store the object data
 * \param[in]  offset         Offset from where the read is going to start
 * \param[in]  size           Data buffer size
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_read(uint32_t object_handle, uint8_t *data,
                                   uint32_t offset, uint32_t size);
/**
 * \brief Writes data into the object referenced by object uuid.
 *
 * \param[in] object_handl  Object handle
 * \param[in] data          Data buffer to write into object object
 * \param[in] offset        Offset from where the write is going to start
 * \param[in] size          Data buffer size
 *
 * \return Returns number of bytes read or a relevant error \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_write(uint32_t object_handle, const uint8_t *data,
                                    uint32_t offset, uint32_t size);
/**
 * \brief Deletes the object referenced by object handler.
 *
 * \param[in] object_handle  Object handler
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_delete(uint32_t object_handle);

/**
 * \brief Gets the object attributes referenced by object handler.
 *
 * \param[in]  object_handle  Object handler
 * \param[out] attributes     Pointer to the attributes structure to store the
 *                            attributes values
 *
 * \return Returns error code specified in \ref tfm_sst_err_t
 */
enum tfm_sst_err_t sst_object_get_attributes(uint32_t object_handle,
                                          struct tfm_sst_attribs_t *attributes);

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
