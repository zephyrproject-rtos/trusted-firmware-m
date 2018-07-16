/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_flash_fs.h"

#include "sst_core.h"

#define SST_FILE_START_POSITION  0

/* FIXME: This implementation relies in the current sst core implementation.
 *        In a different patch, SST core will be refactored to implement this
 *        abstraction layer directly rather than call the current implemented
 *        functions.
 */

enum psa_sst_err_t sst_flash_fs_prepare(void)
{
    /* Initialize metadata block with the valid/active metablock */
    return sst_core_prepare();
}

enum psa_sst_err_t sst_flash_fs_wipe_all(void)
{
    /* Clean and initialize the metadata block */
    return sst_core_wipe_all();
}

enum psa_sst_err_t sst_flash_fs_file_exist(uint32_t file_fid)
{
    return sst_core_object_exist(file_fid);
}

enum psa_sst_err_t sst_flash_fs_file_create(uint32_t fid,
                                            uint32_t max_size,
                                            uint32_t data_size,
                                            const uint8_t *data)
{
    enum psa_sst_err_t err;

    err = sst_core_object_create(fid, max_size);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    if (data != NULL && data_size != 0) {
        err = sst_core_object_write(fid, data, SST_FILE_START_POSITION,
                                    data_size);
    }

    return err;
}

enum psa_sst_err_t sst_flash_fs_file_get_info(uint32_t fid,
                                              struct sst_file_info_t *info)
{
    enum psa_sst_err_t err;
    struct sst_core_obj_info_t tmp_metadata;

    err = sst_core_object_get_info(fid, &tmp_metadata);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    info->size_max = tmp_metadata.size_max;
    info->size_current = tmp_metadata.size_current;

    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_flash_fs_file_write(uint32_t fid, uint32_t size,
                                           uint32_t offset,
                                           const uint8_t *data)
{
    return sst_core_object_write(fid, data, offset, size);
}

enum psa_sst_err_t sst_flash_fs_file_delete(uint32_t fid)
{
    return sst_core_object_delete(fid);
}

enum psa_sst_err_t sst_flash_fs_file_read(uint32_t fid, uint32_t size,
                                          uint32_t offset, uint8_t *data)
{
    return sst_core_object_read(fid, data, offset, size);
}
