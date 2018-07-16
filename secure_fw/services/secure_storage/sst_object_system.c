/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#include "cmsis_compiler.h"
#ifdef SST_ENCRYPTION
#include "sst_encrypted_object.h"
#endif
#include "sst_flash_fs.h"
#include "sst_object_defs.h"
#include "sst_object_system.h"
#include "sst_object_table.h"
#include "sst_utils.h"

#define SST_SYSTEM_READY     1
#define SST_SYSTEM_NOT_READY 0

/* Set to 1 once sst_system_prepare has been called */
static uint8_t sst_system_ready = SST_SYSTEM_NOT_READY;

/* Gets the size of object written to the object system below */
#define SST_OBJECT_SIZE(max_size) (SST_OBJECT_HEADER_SIZE + max_size)
#define SST_OBJECT_START_POSITION  0

/* Allocate static variables to process objects */
static struct sst_object_t g_sst_object;
static struct sst_obj_table_info_t g_obj_tbl_info;

/**
 * \brief Initialize g_sst_object based on the input parameters and empty data.
 *
 * \param[in]  type  Object type
 * \param[in]  size  Object size
 * \param[out] obj   Object to initialize
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
__attribute__ ((always_inline))
__STATIC_INLINE void sst_init_empty_object(uint32_t type, uint32_t size,
                                           struct sst_object_t *obj)
{
    /* Set all object data to 0 */
    sst_utils_memset(obj, SST_DEFAULT_EMPTY_BUFF_VAL, SST_MAX_OBJECT_SIZE);

#ifndef SST_ENCRYPTION
    /* Initialize object version */
    obj->header.version = 0;
#endif

    /* Set object header based on input parameters */
    obj->header.info.size_max = size;
    obj->header.info.type = type;
}

/**
 * \brief Removes the old object table and object from the file system.
 *
 * \param[in] old_fid  Old file ID to remove.
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_remove_old_data(uint32_t old_fid)
{
    enum psa_sst_err_t err;

    /* Delete old object table from the persistent area */
    err = sst_object_table_delete_old_table();
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* Delete old file from the persistent area */
    err = sst_flash_fs_file_delete(old_fid);

    return err;
}

#ifndef SST_ENCRYPTION
enum read_type_t {
  READ_HEADER_ONLY = 0,
  READ_ALL_OBJECT,
};

/**
 * \brief Reads and validates an object header based on its object table info
 *        stored in g_obj_tbl_info.
 *
 * \param[in] type  Read type as specified in \ref read_type_t
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_read_object(enum read_type_t type)
{
    enum psa_sst_err_t err;

    /* Read object header */
    err = sst_flash_fs_file_read(g_obj_tbl_info.fid, SST_OBJECT_HEADER_SIZE,
                                 SST_OBJECT_START_POSITION,
                                 (uint8_t *)&g_sst_object.header);
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    /* As SST encryption support is not enabled, check file ID and version to
     * detect inconsistency after read the object header from flash.
     */
    if (g_sst_object.header.fid != g_obj_tbl_info.fid ||
        g_sst_object.header.version != g_obj_tbl_info.version) {
        err = PSA_SST_ERR_SYSTEM_ERROR;
    }

    if (type == READ_ALL_OBJECT) {
        /* Read object data if any */
        if (g_sst_object.header.info.size_current > 0) {
            err = sst_flash_fs_file_read(g_obj_tbl_info.fid,
                                         g_sst_object.header.info.size_current,
                                         SST_OBJECT_HEADER_SIZE,
                                         g_sst_object.data);
            if (err != PSA_SST_ERR_SUCCESS) {
                return err;
            }
        }
    }

    return err;
}

/**
 * \brief Writes an object based on its object table info stored in
 *        g_obj_tbl_info and the input parameter.
 *
 * \param[in] wrt_size  Number of bytes to write
 *
 * \return Returns error code as specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t sst_write_object(uint32_t wrt_size)
{
    enum psa_sst_err_t err;
    uint32_t max_size = SST_OBJECT_SIZE(g_sst_object.header.info.size_max);

    /* Add object identification and increase object version */
    g_sst_object.header.fid = g_obj_tbl_info.fid;
    g_sst_object.header.version++;

    /* Save object version to be stored in the object table */
    g_obj_tbl_info.version = g_sst_object.header.version;

    err = sst_flash_fs_file_create(g_obj_tbl_info.fid, max_size, wrt_size,
                                   (const uint8_t *)&g_sst_object);
    return err;
}

#endif

enum psa_sst_err_t sst_system_prepare(void)
{
    enum psa_sst_err_t err;

    sst_global_lock();

    err = sst_flash_fs_prepare();
    if (err != PSA_SST_ERR_SUCCESS) {
        goto release_sst_lock_and_return;
    }

    /* Reuse the allocated g_sst_object.data to store a temporary object table
     * data to be validate inside the function.
     * The stored date will be cleaned up when the g_sst_object.data will
     * be used for the first time in the object system.
     */
    err = sst_object_table_init(g_sst_object.data);
    if (err == PSA_SST_ERR_SUCCESS) {
        sst_system_ready = SST_SYSTEM_READY;
    }

#ifdef SST_ENCRYPTION
    g_obj_tbl_info.tag = g_sst_object.header.crypto.ref.tag;
#endif

release_sst_lock_and_return:
    sst_global_unlock();

    return err;
}

enum psa_sst_err_t sst_object_read(uint32_t uuid,
                                   const struct tfm_sst_token_t *s_token,
                                   uint8_t *data, uint32_t offset,
                                   uint32_t size)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SYSTEM_ERROR;

#ifndef SST_ENCRYPTION
    (void)s_token;
#endif

#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    (void)offset;
#endif

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Retrieve the object information from the object table if
         * the object exist.
         */
        err = sst_object_table_get_obj_tbl_info(uuid, &g_obj_tbl_info);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Read object */
#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(g_obj_tbl_info.fid, s_token,
                                        &g_sst_object);
#else
        /* Read object header */
        err = sst_read_object(READ_ALL_OBJECT);
#endif
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Boundary check the incoming request */
        err = sst_utils_check_contained_in(SST_OBJECT_START_POSITION,
                                          g_sst_object.header.info.size_current,
                                          offset, size);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Copy the decrypted object data to the output buffer */
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
        sst_utils_memcpy(data, g_sst_object.data + offset, size);
#else
        sst_utils_memcpy(data, g_sst_object.data, size);
#endif

release_sst_lock_and_return:
        /* Remove data stored in the object before leaving the function */
        sst_utils_memset(&g_sst_object, SST_DEFAULT_EMPTY_BUFF_VAL,
                         SST_MAX_OBJECT_SIZE);

        sst_global_unlock();
    }

    return err;
}

enum psa_sst_err_t sst_object_create(uint32_t uuid,
                                     const struct tfm_sst_token_t *s_token,
                                     uint32_t type, uint32_t size)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SYSTEM_ERROR;

#ifndef SST_ENCRYPTION
    (void)s_token;
#endif

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Check if the object already exists */
        err = sst_object_table_obj_exist(uuid);
        if (err == PSA_SST_ERR_ASSET_NOT_FOUND) {

            /* Get internal ID */
            err = sst_object_table_get_free_fid(&g_obj_tbl_info.fid);
            if (err != PSA_SST_ERR_SUCCESS) {
                goto release_sst_lock_and_return;
            }

            /* Initialize object based on the input arguments and
             * empty content.
             */
            sst_init_empty_object(type, size, &g_sst_object);

#ifdef SST_ENCRYPTION
            err = sst_encrypted_object_write(g_obj_tbl_info.fid, s_token,
                                             &g_sst_object);
#else
            err = sst_write_object(SST_OBJECT_HEADER_SIZE);
#endif
            if (err != PSA_SST_ERR_SUCCESS) {
                goto release_sst_lock_and_return;
            }

            /* Create entry in the table */
            err = sst_object_table_set_obj_tbl_info(uuid, &g_obj_tbl_info);
            if (err != PSA_SST_ERR_SUCCESS) {
                goto release_sst_lock_and_return;
            }

            /* Delete old object table from the persistent area */
            err = sst_object_table_delete_old_table();
        }

release_sst_lock_and_return:
        /* Remove data stored in the object before leaving the function */
        sst_utils_memset(&g_sst_object, SST_DEFAULT_EMPTY_BUFF_VAL,
                         SST_MAX_OBJECT_SIZE);

        sst_global_unlock();
    }

    return err;
}

enum psa_sst_err_t sst_object_write(uint32_t uuid,
                                    const struct tfm_sst_token_t *s_token,
                                    const uint8_t *data, uint32_t offset,
                                    uint32_t size)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SYSTEM_ERROR;
    uint32_t old_fid;

#ifndef SST_ENCRYPTION
    uint32_t wrt_size;

    (void)s_token;
#endif

#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    (void)offset;
#endif

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Retrieve the object information from the object table if
         * the object exists.
         */
        err = sst_object_table_get_obj_tbl_info(uuid, &g_obj_tbl_info);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Read the object */
#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(g_obj_tbl_info.fid, s_token,
                                        &g_sst_object);
#else
        /* Read object header */
        err = sst_read_object(READ_ALL_OBJECT);
#endif
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Offset must not be larger than the object's current size to
         * prevent gaps being created in the object data.
         */
        if (offset > g_sst_object.header.info.size_current) {
            err = PSA_SST_ERR_PARAM_ERROR;
            goto release_sst_lock_and_return;
        }

#ifdef SST_ENABLE_PARTIAL_ASSET_RW
        /* Update the object data */
        sst_utils_memcpy(g_sst_object.data + offset, data, size);

        /* Update the current object size if necessary */
        if ((offset + size) > g_sst_object.header.info.size_current) {
            g_sst_object.header.info.size_current = offset + size;
        }
#else
        /* Update the object data */
        sst_utils_memcpy(g_sst_object.data, data, size);

        /* Update the current object size if necessary */
        if (size > g_sst_object.header.info.size_current) {
            g_sst_object.header.info.size_current = size;
        }
#endif

        /* Save old file ID */
        old_fid = g_obj_tbl_info.fid;

        /* Get new file ID */
        err = sst_object_table_get_free_fid(&g_obj_tbl_info.fid);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_write(g_obj_tbl_info.fid, s_token,
                                         &g_sst_object);
#else
        wrt_size = SST_OBJECT_SIZE(g_sst_object.header.info.size_current);

        /* Write g_sst_object */
        err = sst_write_object(wrt_size);
#endif
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Update the table with the new internal ID and version for
         * the object, and store it in the persistent area.
         */
        err = sst_object_table_set_obj_tbl_info(uuid, &g_obj_tbl_info);
        if (err != PSA_SST_ERR_SUCCESS) {
            /* Remove new object as object table is not persistent
             * and propagate object table manipulation error.
             */
            (void)sst_flash_fs_file_delete(g_obj_tbl_info.fid);

            goto release_sst_lock_and_return;
        }

        /* Remove old object table and object */
        err = sst_remove_old_data(old_fid);

release_sst_lock_and_return:
        /* Remove data stored in the object before leaving the function */
        sst_utils_memset(&g_sst_object, SST_DEFAULT_EMPTY_BUFF_VAL,
                         SST_MAX_OBJECT_SIZE);

        sst_global_unlock();
    }

    return err;
}

enum psa_sst_err_t sst_object_get_info(uint32_t uuid,
                                       const struct tfm_sst_token_t *s_token,
                                       struct psa_sst_asset_info_t *info)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SYSTEM_ERROR;

#ifndef SST_ENCRYPTION
    (void)s_token;
#endif

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Retrieve the object information from the object table if
         * the object exist.
         */
        err = sst_object_table_get_obj_tbl_info(uuid, &g_obj_tbl_info);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(g_obj_tbl_info.fid, s_token,
                                        &g_sst_object);
#else
        err = sst_read_object(READ_HEADER_ONLY);
#endif
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        sst_utils_memcpy(info, &g_sst_object.header.info,
                         PSA_SST_ASSET_INFO_SIZE);

release_sst_lock_and_return:
        /* Remove data stored in the object before leaving the function */
        sst_utils_memset(&g_sst_object, SST_DEFAULT_EMPTY_BUFF_VAL,
                         SST_MAX_OBJECT_SIZE);

        sst_global_unlock();
    }

    return err;
}

enum psa_sst_err_t sst_object_get_attributes(uint32_t uuid,
                                          const struct tfm_sst_token_t *s_token,
                                          struct psa_sst_asset_attrs_t *attrs)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SYSTEM_ERROR;

#ifndef SST_ENCRYPTION
    (void)s_token;
#endif

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Retrieve the object information from the object table if
         * the object exist.
         */
        err = sst_object_table_get_obj_tbl_info(uuid, &g_obj_tbl_info);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

#ifdef SST_ENCRYPTION
        /* Read the object from the the persistent area */
        err = sst_encrypted_object_read(g_obj_tbl_info.fid, s_token,
                                        &g_sst_object);
#else
        /* Read object header */
        err = sst_read_object(READ_HEADER_ONLY);
#endif
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        sst_utils_memcpy(attrs, &g_sst_object.header.attr,
                         PSA_SST_ASSET_ATTR_SIZE);

release_sst_lock_and_return:
        /* Remove data stored in the object before leaving the function */
        sst_utils_memset(&g_sst_object, SST_DEFAULT_EMPTY_BUFF_VAL,
                         SST_MAX_OBJECT_SIZE);

        sst_global_unlock();
    }

    return err;
}

enum psa_sst_err_t sst_object_set_attributes(uint32_t uuid,
                                      const struct tfm_sst_token_t *s_token,
                                      const struct psa_sst_asset_attrs_t *attrs)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SYSTEM_ERROR;
    uint32_t old_fid;

#ifndef SST_ENCRYPTION
    (void)s_token;
#endif

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Retrieve the object information from the object table if
         * the object exist.
         */
        err = sst_object_table_get_obj_tbl_info(uuid, &g_obj_tbl_info);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Read the object from the object system */
#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(g_obj_tbl_info.fid, s_token,
                                        &g_sst_object);
#else
        /* Read object header */
        err = sst_read_object(READ_HEADER_ONLY);
#endif
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Set new attributes */
        sst_utils_memcpy(&g_sst_object.header.attr,
                         attrs, PSA_SST_ASSET_ATTR_SIZE);

        /* Save old file ID */
        old_fid = g_obj_tbl_info.fid;

        /* Get new file ID */
        err = sst_object_table_get_free_fid(&g_obj_tbl_info.fid);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_write(g_obj_tbl_info.fid, s_token,
                                         &g_sst_object);
#else
        /* Write g_sst_object */
        err = sst_write_object(SST_OBJECT_HEADER_SIZE);
#endif
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Update the table with the new internal ID and version for
         * the object, and store it in the persistent area.
         */
        err = sst_object_table_set_obj_tbl_info(uuid, &g_obj_tbl_info);
        if (err != PSA_SST_ERR_SUCCESS) {
            /* Remove new object as object table is not persistent
             * and propagate object table manipulation error.
             */
            (void)sst_flash_fs_file_delete(g_obj_tbl_info.fid);

            goto release_sst_lock_and_return;
        }

        /* Remove old object table and file */
        err = sst_remove_old_data(old_fid);

release_sst_lock_and_return:
        /* Remove data stored in the object before leaving the function */
        sst_utils_memset(&g_sst_object, SST_DEFAULT_EMPTY_BUFF_VAL,
                         SST_MAX_OBJECT_SIZE);

        sst_global_unlock();
    }

    return err;
}

enum psa_sst_err_t sst_object_delete(uint32_t uuid,
                                     const struct tfm_sst_token_t *s_token)
{
    enum psa_sst_err_t err = PSA_SST_ERR_SYSTEM_ERROR;

    /* FIXME: Authenticate object data to validate token before delete it. */
    (void)s_token;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Check if the object exist */
        err = sst_object_table_get_obj_tbl_info(uuid, &g_obj_tbl_info);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Delete object from the table and stores the table in the persistent
         * area.
         */
        err = sst_object_table_delete_object(uuid);
        if (err != PSA_SST_ERR_SUCCESS) {
            goto release_sst_lock_and_return;
        }

        /* Remove old object table and file */
        err = sst_remove_old_data(g_obj_tbl_info.fid);

release_sst_lock_and_return:
        sst_global_unlock();
    }

    return err;
}

enum psa_sst_err_t sst_system_wipe_all(void)
{
    enum psa_sst_err_t err;

    /* This function may get called as a corrective action
     * if a system level security violation is detected.
     * This could be asynchronous to normal system operation
     * and state of the sst system lock is unknown. Hence
     * this function doesn't block on the lock and directly
     * moves to erasing the flash instead.
     */
    err = sst_flash_fs_wipe_all();
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    err = sst_flash_fs_prepare();
    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

    return sst_object_table_create();
}
