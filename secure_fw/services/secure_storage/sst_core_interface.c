/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#include "sst_core_interface.h"
#include "assets/sst_asset_defs.h"
#include "sst_core.h"
#include "sst_utils.h"

#define ASSET_ID_ERROR  0xFFFF

#define SST_SYSTEM_READY 1
#define SST_SYSTEM_NOT_READY 0
/* Set to 1 once sst_object_prepare has been called */
static uint8_t sst_system_ready = SST_SYSTEM_NOT_READY;

enum tfm_sst_err_t sst_object_prepare(void)
{
    enum tfm_sst_err_t err;

    sst_global_lock();
    err = sst_core_prepare();
    if (err == TFM_SST_ERR_SUCCESS) {
        sst_system_ready = SST_SYSTEM_READY;
    }
    sst_global_unlock();

    return err;
}

enum tfm_sst_err_t sst_object_handle(uint16_t asset_uuid, uint32_t *handle)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();
        err = sst_core_object_handle(asset_uuid, handle);
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_read(uint32_t asset_handle, uint8_t *data,
                                   uint32_t offset, uint32_t size)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();
        err = sst_core_object_read(asset_handle, data, offset, size);
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_create(uint16_t uuid, uint32_t size)
{
    /* Check if it already exists */
    uint32_t hdl = 0;
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();
        err = sst_core_object_handle(uuid, &hdl);
        if (err == TFM_SST_ERR_ASSET_NOT_FOUND) {
            /* Find free space */
            err = sst_core_object_create(uuid, size);

        }
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_write(uint32_t asset_handle, const uint8_t *data,
                                    uint32_t offset, uint32_t size)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();
        err = sst_core_object_write(asset_handle, data, offset, size);
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_get_attributes(uint32_t asset_handle,
                                           struct tfm_sst_attribs_t *attributes)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;
    struct sst_assetmeta tmp_metadata;
    uint32_t object_index;
    uint16_t uuid;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();
        /* Get the meta data index */
        object_index = sst_utils_extract_index_from_handle(asset_handle);
        /* Read object metadata */
        err = sst_meta_read_object_meta(object_index, &tmp_metadata);
        if (err == 0) {
            /* Check if index is still referring to same asset */
            uuid = sst_utils_extract_uuid_from_handle(asset_handle);
            if (uuid != tmp_metadata.unique_id) {
                /* Likely the asset has been deleted in another context
                 * this handle isn't valid anymore.
                 */
                err = TFM_SST_ERR_INVALID_HANDLE;
            } else {
                attributes->size_max = tmp_metadata.max_size;
                attributes->size_current = tmp_metadata.cur_size;
            }
        }
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_delete(uint32_t asset_handle)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();
        err = sst_core_object_delete(asset_handle);
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_wipe_all(void)
{
    enum tfm_sst_err_t err;
    /* This function may get called as a corrective action
     * if a system level security violation is detected.
     * This could be asynchronous to normal system operation
     * and state of the sst system lock is unknown. Hence
     * this function doesn't block on the lock and directly
     * moves to erasing the flash instead.
     */
    err = sst_core_wipe_all();

    return err;
}
