/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file  its_flash_fs.h
 *
 * \brief Internal Trusted Storage service filesystem abstraction APIs.
 *        The purpose of this abstraction is to have the ability to plug-in
 *        other filesystems or filesystem proxies (supplicant).
 */

#ifndef __ITS_FLASH_FS_H__
#define __ITS_FLASH_FS_H__

#include <stddef.h>
#include <stdint.h>

#include "its_flash_fs_mblock.h"
#include "psa/error.h"
#include "flash/its_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Space reserved for user-defined flags. These are stored in the file metadata
 * and can be retrieved by getting the file info.
 */
#define ITS_FLASH_FS_USER_FLAGS_MASK   ((1U << 16) - 1)

/* Filesystem flags that can be provided when calling the write function */
#define ITS_FLASH_FS_WRITE_FLAGS_MASK  ((1U << 24) - (1U << 16))
/* Create the file if it does not already exist */
#define ITS_FLASH_FS_FLAG_CREATE       (1U << 16)
/* Remove existing file data if it exists */
#define ITS_FLASH_FS_FLAG_TRUNCATE     (1U << 17)

/**
 * \brief ITS flash filesystem context type, used to maintain state across FS
 *        operations.
 *
 * \details The user should allocate a variable of this type, initialised to
 *          zero, before calling its_flash_fs_prepare, and then pass it to each
 *          subsequent FS operation. The contents are internal to the
 *          filesystem.
 */
typedef struct its_flash_fs_ctx_t its_flash_fs_ctx_t;

/*!
 * \struct its_file_info_t
 *
 * \brief Structure to store the file information.
 */
struct its_file_info_t {
    size_t size_current; /*!< The current size of the flash file data */
    size_t size_max;     /*!< The maximum size of the flash file data in
                          * bytes.
                          */
    uint32_t flags;      /*!< Flags set when the file was created */
};

/**
 * \brief Prepares the filesystem to accept operations on the files.
 *
 * \param[in,out] fs_ctx      Filesystem context to prepare. Must have been
 *                            initialised by the caller.
 * \param[in]     flash_info  Struct containing information about the flash
 *                            device to associate with the context.
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_prepare(its_flash_fs_ctx_t *fs_ctx,
                                  const struct its_flash_info_t *flash_info);

/**
 * \brief Wipes all files from the filesystem.
 *
 * \param[in,out] fs_ctx  Filesystem context to wipe. Must be prepared again
 *                        before further use.
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_wipe_all(its_flash_fs_ctx_t *fs_ctx);

/**
 * \brief Checks if a file exists in the filesystem.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 *
 * \return Returns PSA_SUCCESS if the file exists. If file does not exist, it
 *         returns PSA_ERROR_DOES_NOT_EXIST. Otherwise, it returns error code as
 *         specified in \ref psa_status_t.
 */
psa_status_t its_flash_fs_file_exist(its_flash_fs_ctx_t *fs_ctx,
                                     const uint8_t *fid);

/**
 * \brief Gets the file information referenced by the file ID.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 * \param[out]    info    Pointer to the information structure to store the
 *                        file information values \ref its_file_info_t
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_file_get_info(its_flash_fs_ctx_t *fs_ctx,
                                        const uint8_t *fid,
                                        struct its_file_info_t *info);

/**
 * \brief Writes data to a file.
 *
 * \param[in,out] fs_ctx     Filesystem context
 * \param[in]     fid        File ID
 * \param[in]     flags      Flags of the file
 * \param[in]     max_size   Maximum size of the file to be created. Ignored if
 *                           the file is not being created.
 * \param[in]     data_size  Size of the incoming write data.
 * \param[in]     offset     Offset in the file to write. Must be less than or
 *                           equal to the current file size.
 * \param[in]     data       Pointer to buffer containing data to be written
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_file_write(its_flash_fs_ctx_t *fs_ctx,
                                     const uint8_t *fid,
                                     uint32_t flags,
                                     size_t max_size,
                                     size_t data_size,
                                     size_t offset,
                                     const uint8_t *data);

/**
 * \brief Reads data from an existing file.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 * \param[in]     size    Size to be read
 * \param[in]     offset  Offset in the file
 * \param[out]    data    Pointer to buffer to store the data
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_file_read(its_flash_fs_ctx_t *fs_ctx,
                                    const uint8_t *fid,
                                    size_t size,
                                    size_t offset,
                                    uint8_t *data);

/**
 * \brief Deletes file referenced by the file ID.
 *
 * \param[in,out] fs_ctx  Filesystem context
 * \param[in]     fid     File ID
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_file_delete(its_flash_fs_ctx_t *fs_ctx,
                                      const uint8_t *fid);

/**
 * \brief Validates the configuration of the flash filesystem.
 *
 * This function checks that the flash block provided is compatible with the
 * flash_fs described by the info parameter
 *
 * \param[in] info        Filesystem information
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t its_flash_fs_validate_params(const struct its_flash_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* __ITS_FLASH_FS_H__ */
