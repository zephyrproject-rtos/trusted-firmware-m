/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>

#ifdef SST_ENCRYPTION
#include "sst_encrypted_object.h"
#endif
#include "sst_object_defs.h"
#include "sst_object_system.h"
#include "sst_core.h"
#include "sst_utils.h"

#define SST_SYSTEM_READY     1
#define SST_SYSTEM_NOT_READY 0

/* Set to 1 once sst_system_prepare has been called */
static uint8_t sst_system_ready = SST_SYSTEM_NOT_READY;

/* Gets the size of object written to the object system below */
#define SST_OBJECT_SIZE(max_size) (SST_OBJECT_HEADER_SIZE + max_size)
#define SST_OBJECT_START_POSITION  0

#define SST_DEFAULT_EMPTY_BUFF_VAL 0

static struct sst_object_t g_sst_object;

/**
 * \brief Initialize an object based on the input parameters.
 *
 * \param[in]  uuid  Object UUID
 * \param[in]  type  Object type
 * \param[in]  size  Object size
 * \param[out] obj   Object to
 *
 * \return Returns error code as specified in \ref tfm_sst_err_t
 */
static void sst_object_init_object(uint32_t uuid, uint32_t type, uint32_t size,
                                   struct sst_object_t *obj)
{
    /* Set all object data to 0 */
    sst_utils_memset(obj, SST_DEFAULT_EMPTY_BUFF_VAL, SST_MAX_OBJECT_SIZE);

    /* Set object header properties based on input parameters */
    obj->header.uuid = uuid;
    obj->header.version = 0;
    obj->header.info.size_max = size;
    obj->header.info.type = type;
}

enum tfm_sst_err_t sst_system_prepare(void)
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

enum tfm_sst_err_t sst_object_read(uint32_t uuid, uint8_t *data,
                                   uint32_t offset, uint32_t size)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    (void)offset;
#endif

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(uuid, &g_sst_object);
#else
        /* Read object header */
        err = sst_core_object_read(uuid,
                                   (uint8_t *)&g_sst_object.header,
                                   SST_OBJECT_START_POSITION,
                                   SST_OBJECT_HEADER_SIZE);
        if (err != TFM_SST_ERR_SUCCESS) {
            return err;
        }

        /* Read object data if any */
        if (g_sst_object.header.info.size_current > 0) {
            err = sst_core_object_read(uuid, g_sst_object.data,
                                       SST_OBJECT_HEADER_SIZE,
                                       g_sst_object.header.info.size_current);
        }
#endif
        if (err != TFM_SST_ERR_SUCCESS) {
            return err;
        }

        /* Boundary check the incoming request */
        err = sst_utils_check_contained_in(SST_OBJECT_START_POSITION,
                                          g_sst_object.header.info.size_current,
                                          offset, size);
        if (err != TFM_SST_ERR_SUCCESS) {
            return err;
        }

        /* Copy the decrypted object data to the output buffer */
#ifdef SST_ENABLE_PARTIAL_ASSET_RW
        sst_utils_memcpy(data, g_sst_object.data + offset, size);
#else
        sst_utils_memcpy(data, g_sst_object.data, size);
#endif


        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_create(uint32_t uuid,
                                     uint32_t type, uint32_t size)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();
        /* Check if it already exists */
        err = sst_core_object_exist(uuid);
        if (err == TFM_SST_ERR_ASSET_NOT_FOUND) {
            /* Initialize object based on the input arguments */
            sst_object_init_object(uuid, type, size, &g_sst_object);

#ifdef SST_ENCRYPTION
            err = sst_encrypted_object_create(uuid, &g_sst_object);
#else
            /* FixMe: This is an inefficient way to write the object header.
             *        The create function should allow to write content
             *        in the object.
             */
            err = sst_core_object_create(uuid,
                                         SST_OBJECT_SIZE(size));
            if (err != TFM_SST_ERR_SUCCESS) {
                return err;
            }

            err = sst_core_object_write(uuid,
                                        (uint8_t *)&g_sst_object,
                                        SST_OBJECT_START_POSITION,
                                        SST_OBJECT_HEADER_SIZE);
#endif
        }
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_write(uint32_t uuid,
                                    const uint8_t *data, uint32_t offset,
                                    uint32_t size)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    (void)offset;
#endif


    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Read the object from the object system */
#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(uuid, &g_sst_object);
#else
        /* Read object header */
        err = sst_core_object_read(uuid,
                                   (uint8_t *)&g_sst_object.header,
                                   SST_OBJECT_START_POSITION,
                                   SST_OBJECT_HEADER_SIZE);
        if (err != TFM_SST_ERR_SUCCESS) {
            return err;
        }

        /* Read object data if any */
        if (g_sst_object.header.info.size_current > 0) {
            err = sst_core_object_read(uuid, g_sst_object.data,
                                       SST_OBJECT_HEADER_SIZE,
                                       g_sst_object.header.info.size_current);
        }
#endif
        if (err != TFM_SST_ERR_SUCCESS) {
            return err;
        }

        /* Offset must not be larger than the object's current size to
         * prevent gaps being created in the object data.
         */
        if (offset > g_sst_object.header.info.size_current) {
            return TFM_SST_ERR_PARAM_ERROR;
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

#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_write(uuid, &g_sst_object);
#else
        err = sst_core_object_write(uuid, (uint8_t *)&g_sst_object,
                                    SST_OBJECT_START_POSITION,
                                    SST_OBJECT_SIZE(
                                        g_sst_object.header.info.size_current));
#endif
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_get_info(uint32_t uuid,
                                       struct tfm_sst_asset_info_t *info)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Read the object from the object system */
#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(uuid, &g_sst_object);
#else
        err = sst_core_object_read(uuid, (uint8_t *)&g_sst_object,
                                   SST_OBJECT_START_POSITION,
                                   SST_OBJECT_HEADER_SIZE);
#endif
        if (err != TFM_SST_ERR_SUCCESS) {
            return err;
        }

        sst_utils_memcpy(info, &g_sst_object.header.info,
                         TFM_SST_ASSET_INFO_SIZE);

        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_get_attributes(uint32_t uuid,
                                        struct tfm_sst_asset_attrs_t *attrs)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Read the object from the object system */
#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(uuid, &g_sst_object);
#else
        err = sst_core_object_read(uuid, (uint8_t *)&g_sst_object,
                                   SST_OBJECT_START_POSITION,
                                   SST_OBJECT_HEADER_SIZE);
#endif
        if (err != TFM_SST_ERR_SUCCESS) {
            return err;
        }

        sst_utils_memcpy(attrs, &g_sst_object.header.attr,
                         TFM_SST_ASSET_ATTR_SIZE);

        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_set_attributes(uint32_t uuid,
                                      const struct tfm_sst_asset_attrs_t *attrs)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();

        /* Read the object from the object system */
#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_read(uuid, &g_sst_object);
#else
        err = sst_core_object_read(uuid, (uint8_t *)&g_sst_object,
                                   SST_OBJECT_START_POSITION,
                                   SST_OBJECT_HEADER_SIZE);
#endif
        if (err != TFM_SST_ERR_SUCCESS) {
            return err;
        }

        /* Set new attributes */
        sst_utils_memcpy(&g_sst_object.header.attr,
                         attrs, TFM_SST_ASSET_ATTR_SIZE);

#ifdef SST_ENCRYPTION
        err = sst_encrypted_object_write(uuid, &g_sst_object);
#else
        err = sst_core_object_write(uuid, (uint8_t *)&g_sst_object,
                                   SST_OBJECT_START_POSITION,
                                   SST_OBJECT_HEADER_SIZE);
#endif

        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_object_delete(uint32_t uuid)
{
    enum tfm_sst_err_t err = TFM_SST_ERR_SYSTEM_ERROR;

    /* FIXME: Authenticate object data to validate token before delete it. */

    if (sst_system_ready == SST_SYSTEM_READY) {
        sst_global_lock();
        err = sst_core_object_delete(uuid);
        sst_global_unlock();
    }

    return err;
}

enum tfm_sst_err_t sst_system_wipe_all(void)
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
