/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_asset_management.h"

#include <stddef.h>

#include "assets/sst_asset_defs.h"
#include "sst_object_system.h"
#include "sst_utils.h"
#include "tfm_secure_api.h"
#include "tfm_sst_defs.h"

/******************************/
/* Asset management functions */
/******************************/

/* Policy database */
extern struct sst_asset_policy_t asset_perms[];
extern struct sst_asset_perm_t asset_perms_modes[];

/**
 * \brief Looks up for policy entry for give app and uuid
 *
 * \param[in] db_entry  Asset specific entry
 * \param[in] app_id    Identify of the application calling the service
 *
 * \return Returns the perms entry on successful lookup
 */
static struct sst_asset_perm_t *sst_am_lookup_app_perms(
                                      const struct sst_asset_policy_t *db_entry,
                                      uint32_t app_id)
{
    struct sst_asset_perm_t *perm_entry;
    uint32_t i;

    for (i = 0; i < db_entry->perms_count; i++) {
        perm_entry = &asset_perms_modes[db_entry->perms_modes_start_idx+i];
        if (perm_entry->app == app_id) {
            return perm_entry;
        }
    }

    return NULL;
}

/**
 * \brief Gets pointer to policy entry for an asset
 *
 * \param[in] uuid  Unique identifier of the object being accessed
 *
 * \return Returns the pointer for entry for specified asset
 */
static struct sst_asset_policy_t *sst_am_lookup_db_entry(uint32_t uuid)
{
    uint32_t i;

    /* Lookup in db for matching entry */
    for (i = 0; i < SST_NUM_ASSETS; i++) {
        if (asset_perms[i].asset_uuid == uuid) {
            return &asset_perms[i];
        }
    }

    return NULL;
}

/**
 * \brief Checks the compile time policy for secure/non-secure separation
 *
 * \param[in] app_id        caller's application ID
 * \param[in] request_type  requested action to perform
 *
 * \return Returns the sanitized request_type
 */
static uint16_t sst_am_check_s_ns_policy(uint32_t app_id, uint16_t request_type)
{
    enum psa_sst_err_t err;
    uint16_t access;

    /* FIXME: based on level 1 tfm isolation, any entity on the secure side
     * can have full access if it uses secure app ID to make the call.
     * When the secure caller passes on the app_id of non-secure entity,
     * the code only allows read by reference. I.e. if the app_id
     * has the reference permission, the secure caller will be allowed
     * to read the entry. This needs a revisit when for higher level
     * of isolation.
     *
     * FIXME: current code allows only a referenced read, however there
     * is a case for refereced create/write/delete as well, for example
     * a NS entity may ask another secure service to derive a key and securely
     * store it, and make references for encryption/decryption and later on
     * delete it.
     * For now it is for the other secure service to create/delete/write
     * resources with the secure app ID.
     */
    err = sst_utils_validate_secure_caller();

    if (err == PSA_SST_ERR_SUCCESS) {
        if (app_id != S_APP_ID) {
            if (request_type & SST_PERM_READ) {
                access = SST_PERM_REFERENCE;
            } else {
                /* Other permissions can not be delegated */
                access = SST_PERM_FORBIDDEN;
            }
        } else {
            /* a call from secure entity on it's own behalf.
             * In level 1 isolation, any secure entity has
             * full access to storage.
             */
            access = SST_PERM_BYPASS;
        }
    } else if (app_id == S_APP_ID) {
        /* non secure caller spoofing as secure caller */
        access = SST_PERM_FORBIDDEN;
    } else {
        access = request_type;
    }
    return access;
}

/**
 * \brief Gets asset's permissions if the application is allowed
 *        based on the request_type
 *
 * \param[in] app_id        Caller's application ID
 * \param[in] uuid          Asset's unique identifier
 * \param[in] request_type  Type of requested access
 *
 * \note If request_type contains multiple permissions, this function
 *       returns the entry pointer for specified asset if at least one
 *       of those permissions match.
 *
 * \return Returns the entry pointer for specified asset
 */
static struct sst_asset_policy_t *sst_am_get_db_entry(uint32_t app_id,
                                                      uint32_t uuid,
                                                      uint8_t request_type)
{
    struct sst_asset_perm_t   *perm_entry;
    struct sst_asset_policy_t *db_entry;

    request_type = sst_am_check_s_ns_policy(app_id, request_type);

    /* security access violation */
    if (request_type == SST_PERM_FORBIDDEN) {
        /* FIXME: this is prone to timing attacks. Ideally the time
         * spent in this function should always be constant irrespective
         * of success or failure of checks. Timing attacks will be
         * addressed in later version.
         */
        return NULL;
    }

    /* Find policy db entry for the the asset */
    db_entry = sst_am_lookup_db_entry(uuid);
    if (db_entry == NULL) {
        return NULL;
    }

    if (request_type == SST_PERM_BYPASS) {
         return db_entry;
     }

    /* Find the app ID entry in the database */
    perm_entry = sst_am_lookup_app_perms(db_entry, app_id);
    if (perm_entry == NULL) {
        return NULL;
    }

     /* Check if the db permission matches with at least one of the
      * requested permissions types.
      */
    if ((perm_entry->perm & request_type) != 0) {
        return db_entry;
    }
    return NULL;
}

/**
 * \brief Validates the policy database's integrity
 *        Stub function.
 *
 * \return Returns value specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t validate_policy_db(void)
{
    /* Currently the policy database is inbuilt
     * in the code. It's sanity is assumed to be correct.
     * In the later revisions if access policy is
     * stored differently, it may require sanity check
     * as well.
     */
    return PSA_SST_ERR_SUCCESS;
}

enum psa_sst_err_t sst_am_prepare(void)
{
    enum psa_sst_err_t err;
    /* FIXME: outcome of this function should determine
     * state machine of asset manager. If this
     * step fails other APIs shouldn't entertain
     * any user calls. Not a major issue for now
     * as policy db check is a dummy function, and
     * sst core maintains it's own state machine.
     */

    /* Validate policy database */
    err = validate_policy_db();

    /* Initialize underlying storage system */
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_SYSTEM_ERROR;
    }

    err = sst_system_prepare();
#ifdef SST_RAM_FS
    /* in case of RAM based system there wouldn't be
     * any content in the boot time. Call the wipe API
     * to create a storage structure.
     */
    if (err != PSA_SST_ERR_SUCCESS) {
        sst_system_wipe_all();
        /* attempt to initialise again */
        err = sst_system_prepare();
    }
#endif /* SST_RAM_FS */

    return err;
}

/**
 * \brief Validate incoming iovec structure
 *
 * \param[in] src     Incoming iovec for the read/write request
 * \param[in] dest    Pointer to local copy of the iovec
 * \param[in] app_id  Application ID of the caller
 * \param[in] access  Access type to be permormed on the given dest->data
 *                    address
 *
 * \return Returns value specified in \ref psa_sst_err_t
 */
static enum psa_sst_err_t validate_copy_validate_iovec(
                                                const struct tfm_sst_buf_t *src,
                                                struct tfm_sst_buf_t *dest,
                                                uint32_t app_id,
                                                uint32_t access)
{
    /* iovec struct needs to be used as veneers do not allow
     * more than four params.
     * First validate the pointer for iovec itself, then copy
     * the iovec, then validate the local copy of iovec.
     */
    enum psa_sst_err_t bound_check;

    bound_check = sst_utils_bound_check_and_copy((uint8_t *) src,
                      (uint8_t *) dest, sizeof(struct tfm_sst_buf_t), app_id);
    if (bound_check == PSA_SST_ERR_SUCCESS) {
        bound_check = sst_utils_memory_bound_check(dest->data, dest->size,
                                                   app_id, access);
    }

    return bound_check;
}

enum psa_sst_err_t sst_am_get_info(uint32_t app_id, uint32_t asset_uuid,
                                   const struct tfm_sst_token_t *s_token,
                                   struct psa_sst_asset_info_t *info)
{
    enum psa_sst_err_t bound_check;
    struct sst_asset_policy_t *db_entry;
    struct psa_sst_asset_info_t tmp_info;
    enum psa_sst_err_t err;
    uint8_t all_perms = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE;

    bound_check = sst_utils_memory_bound_check(info,
                                               PSA_SST_ASSET_INFO_SIZE,
                                               app_id, TFM_MEMORY_ACCESS_RW);
    if (bound_check != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    db_entry = sst_am_get_db_entry(app_id, asset_uuid, all_perms);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_object_get_info(asset_uuid, s_token, &tmp_info);
    if (err == PSA_SST_ERR_SUCCESS) {
        /* Use tmp_info to not leak information in case the previous function
         * returns and error. It avoids to leak information in case of error.
         * So, copy the tmp_info content into the attrs only if that tmp_info
         * data is valid.
         */
        sst_utils_memcpy(info, &tmp_info, PSA_SST_ASSET_INFO_SIZE);
    }

    return err;
}

enum psa_sst_err_t sst_am_get_attributes(uint32_t app_id, uint32_t asset_uuid,
                                         const struct tfm_sst_token_t *s_token,
                                         struct psa_sst_asset_attrs_t *attrs)
{
    uint8_t all_perms = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE;
    enum psa_sst_err_t bound_check;
    struct sst_asset_policy_t *db_entry;
    enum psa_sst_err_t err;
    struct psa_sst_asset_attrs_t tmp_attrs;

    bound_check = sst_utils_memory_bound_check(attrs,
                                               PSA_SST_ASSET_ATTR_SIZE,
                                               app_id, TFM_MEMORY_ACCESS_RW);
    if (bound_check != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    db_entry = sst_am_get_db_entry(app_id, asset_uuid, all_perms);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_object_get_attributes(asset_uuid, s_token, &tmp_attrs);
    if (err == PSA_SST_ERR_SUCCESS) {
        /* Use tmp_attrs to not leak information incase the previous function
         * returns and error. It avoids to leak information in case of error.
         * So, copy the tmp_attrs content into the attrs only if that tmp_attrs
         * data is valid.
         */
        sst_utils_memcpy(attrs, &tmp_attrs, PSA_SST_ASSET_ATTR_SIZE);
    }

    return err;
}

enum psa_sst_err_t sst_am_set_attributes(uint32_t app_id, uint32_t asset_uuid,
                                      const struct tfm_sst_token_t *s_token,
                                      const struct psa_sst_asset_attrs_t *attrs)
{
    uint8_t all_perms = SST_PERM_REFERENCE | SST_PERM_READ | SST_PERM_WRITE;
    enum psa_sst_err_t bound_check;
    struct sst_asset_policy_t *db_entry;
    enum psa_sst_err_t err;

    bound_check = sst_utils_memory_bound_check((uint8_t *)attrs,
                                               PSA_SST_ASSET_ATTR_SIZE,
                                               app_id, TFM_MEMORY_ACCESS_RO);
    if (bound_check != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    db_entry = sst_am_get_db_entry(app_id, asset_uuid, all_perms);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* FIXME: Validity attributes are not supported in the current service
     *        implementation. It is mandatory to set start and end subattributes
     *        to 0.
     */
    if (attrs->validity.start != 0 || attrs->validity.end != 0) {
        return PSA_SST_ERR_PARAM_ERROR;
    }

    /* FIXME: Check which bit attributes have been changed and check if those
     *        can be modified or not.
     */
    err = sst_object_set_attributes(asset_uuid, s_token, attrs);

    return err;
}

enum psa_sst_err_t sst_am_create(uint32_t app_id, uint32_t asset_uuid,
                                 const struct tfm_sst_token_t *s_token)
{
    enum psa_sst_err_t err;
    struct sst_asset_policy_t *db_entry;

    db_entry = sst_am_get_db_entry(app_id, asset_uuid, SST_PERM_WRITE);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_object_create(asset_uuid, s_token, db_entry->type,
                            db_entry->max_size);

    return err;
}

enum psa_sst_err_t sst_am_read(uint32_t app_id, uint32_t asset_uuid,
                               const struct tfm_sst_token_t *s_token,
                               struct tfm_sst_buf_t *data)
{
    struct tfm_sst_buf_t local_data;
    enum psa_sst_err_t err;
    struct sst_asset_policy_t *db_entry;

    /* Check application ID permissions */
    db_entry = sst_am_get_db_entry(app_id, asset_uuid, SST_PERM_READ);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Make a local copy of the iovec data structure */
    err = validate_copy_validate_iovec(data, &local_data,
                                       app_id, TFM_MEMORY_ACCESS_RW);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    if (data->offset != 0) {
        return PSA_SST_ERR_PARAM_ERROR;
    }
#endif

    err = sst_object_read(asset_uuid, s_token, local_data.data,
                          local_data.offset, local_data.size);

    return err;
}

enum psa_sst_err_t sst_am_write(uint32_t app_id, uint32_t asset_uuid,
                                const struct tfm_sst_token_t *s_token,
                                const struct tfm_sst_buf_t *data)
{
    struct tfm_sst_buf_t local_data;
    enum psa_sst_err_t err;
    struct sst_asset_policy_t *db_entry;

    /* Check application ID permissions */
    db_entry = sst_am_get_db_entry(app_id, asset_uuid, SST_PERM_WRITE);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Make a local copy of the iovec data structure */
    err = validate_copy_validate_iovec(data, &local_data,
                                       app_id, TFM_MEMORY_ACCESS_RO);
    if (err != PSA_SST_ERR_SUCCESS) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    /* Boundary check the incoming request */
    err = sst_utils_check_contained_in(0, db_entry->max_size,
                                       local_data.offset, local_data.size);

    if (err != PSA_SST_ERR_SUCCESS) {
        return err;
    }

#ifndef SST_ENABLE_PARTIAL_ASSET_RW
    if (data->offset != 0) {
        return PSA_SST_ERR_PARAM_ERROR;
    }
#endif

    err = sst_object_write(asset_uuid, s_token, local_data.data,
                           local_data.offset, local_data.size);

    return err;
}

enum psa_sst_err_t sst_am_delete(uint32_t app_id, uint32_t asset_uuid,
                                 const struct tfm_sst_token_t *s_token)
{
    enum psa_sst_err_t err;
    struct sst_asset_policy_t *db_entry;

    db_entry = sst_am_get_db_entry(app_id, asset_uuid, SST_PERM_WRITE);
    if (db_entry == NULL) {
        return PSA_SST_ERR_ASSET_NOT_FOUND;
    }

    err = sst_object_delete(asset_uuid, s_token);

    return err;
}
