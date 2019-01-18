/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_protected_storage.h"

#include "secure_fw/core/tfm_secure_api.h"
#include "sst_object_system.h"
#include "sst_utils.h"
#include "tfm_api.h"

enum tfm_sst_err_t tfm_sst_init(void)
{
    enum tfm_sst_err_t err;

    err = sst_system_prepare();
#ifdef SST_CREATE_FLASH_LAYOUT
    /* If SST_CREATE_FLASH_LAYOUT is set, it indicates that it is required to
     * create a SST flash layout. SST service will generate an empty and valid
     * SST flash layout to store assets. It will erase all data located in the
     * assigned SST memory area before generating the SST layout.
     * This flag is required to be set if the SST memory area is located in
     * non-persistent memory.
     * This flag can be set if the SST memory area is located in persistent
     * memory without a previous valid SST flash layout in it. That is the case
     * when it is the first time in the device life that the SST service is
     * executed.
     */
    if (err != TFM_SST_ERR_SUCCESS) {
        /* Remove all data in the SST memory area and create a valid SST flash
         * layout in that area.
         */
        sst_system_wipe_all();

        /* Attempt to initialise again */
        err = sst_system_prepare();
    }
#endif /* SST_CREATE_FLASH_LAYOUT */

    return err;
}

enum tfm_sst_err_t tfm_sst_set(const psa_ps_uid_t *uid,
                               uint32_t data_length,
                               const void *p_data,
                               psa_ps_create_flags_t create_flags)
{
    enum tfm_status_e status;
    int32_t client_id;

    status = tfm_core_memory_permission_check((psa_ps_uid_t *)uid,
                                              sizeof(psa_ps_uid_t),
                                              TFM_MEMORY_ACCESS_RO);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    /* If the data length is zero, then just check the p_data pointer is not
     * NULL, otherwise perform a memory permission check on the region.
     */
    if (data_length == 0) {
        if (p_data == NULL) {
            return TFM_SST_ERR_INVALID_ARGUMENT;
        }
    } else {
        status = tfm_core_memory_permission_check((void *)p_data, data_length,
                                                  TFM_MEMORY_ACCESS_RO);
        if (status != TFM_SUCCESS) {
            return TFM_SST_ERR_INVALID_ARGUMENT;
        }
    }

    /* Check that the UID is valid */
    if (*uid == SST_INVALID_UID) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_OPERATION_FAILED;
    }

    /* Check that the create_flags does not contain any unsupported flags */
    if (create_flags & ~PSA_PS_FLAG_WRITE_ONCE) {
        return TFM_SST_ERR_FLAGS_NOT_SUPPORTED;
    }

    /* Create the object in the object system */
    return sst_object_create(*uid, client_id, create_flags, data_length,
                             p_data);
}

enum tfm_sst_err_t tfm_sst_get(const psa_ps_uid_t *uid,
                               uint32_t data_offset,
                               uint32_t data_length,
                               void *p_data)
{
    enum tfm_status_e status;
    int32_t client_id;

    status = tfm_core_memory_permission_check((psa_ps_uid_t *)uid,
                                              sizeof(psa_ps_uid_t),
                                              TFM_MEMORY_ACCESS_RO);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    /* If the data length is zero, then just check the p_data pointer is not
     * NULL, otherwise perform a memory permission check on the region.
     */
    if (data_length == 0) {
        if (p_data == NULL) {
            return TFM_SST_ERR_INVALID_ARGUMENT;
        }
    } else {
        status = tfm_core_memory_permission_check((void *)p_data, data_length,
                                                  TFM_MEMORY_ACCESS_RW);
        if (status != TFM_SUCCESS) {
            return TFM_SST_ERR_INVALID_ARGUMENT;
        }
    }

    /* Check that the UID is valid */
    if (*uid == SST_INVALID_UID) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_OPERATION_FAILED;
    }

    /* Read the object data from the object system */
    return sst_object_read(*uid, client_id, data_offset, data_length, p_data);
}

enum tfm_sst_err_t tfm_sst_get_info(const psa_ps_uid_t *uid,
                                    struct psa_ps_info_t *p_info)
{
    enum tfm_status_e status;
    int32_t client_id;

    status = tfm_core_memory_permission_check((psa_ps_uid_t *)uid,
                                              sizeof(psa_ps_uid_t),
                                              TFM_MEMORY_ACCESS_RO);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    status = tfm_core_memory_permission_check(p_info,
                                              sizeof(struct psa_ps_info_t),
                                              TFM_MEMORY_ACCESS_RW);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    /* Check that the UID is valid */
    if (*uid == SST_INVALID_UID) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_OPERATION_FAILED;
    }

    /* Get the info struct data from the object system */
    return sst_object_get_info(*uid, client_id, p_info);
}

enum tfm_sst_err_t tfm_sst_remove(const psa_ps_uid_t *uid)
{
    enum tfm_status_e status;
    int32_t client_id;

    status = tfm_core_memory_permission_check((psa_ps_uid_t *)uid,
                                              sizeof(psa_ps_uid_t),
                                              TFM_MEMORY_ACCESS_RO);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    /* Check that the UID is valid */
    if (*uid == SST_INVALID_UID) {
        return TFM_SST_ERR_INVALID_ARGUMENT;
    }

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != TFM_SUCCESS) {
        return TFM_SST_ERR_OPERATION_FAILED;
    }

    /* Delete the object from the object system */
    return sst_object_delete(*uid, client_id);
}

enum tfm_sst_err_t tfm_sst_get_support(uint32_t *support_flags)
{
    /*
     * This function returns a bitmask with flags set for all of the optional
     * features supported by the SST service implementation.
     *
     * SST service does not support the optional extended PSA PS API yet.
     */

    *support_flags = 0;

    return TFM_SST_ERR_SUCCESS;
}
