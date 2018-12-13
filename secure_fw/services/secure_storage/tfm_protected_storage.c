/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Note: A full implementation will be added in another patch. */

#include "tfm_protected_storage.h"

enum tfm_sst_err_t tfm_sst_init(void)
{
    return TFM_SST_ERR_SUCCESS;
}

enum tfm_sst_err_t tfm_sst_set(const psa_ps_uid_t *uid,
                               uint32_t data_length,
                               const void *p_data,
                               psa_ps_create_flags_t create_flags)
{
    (void)uid, (void)data_length, (void)p_data, (void)create_flags;
    return TFM_SST_ERR_NOT_SUPPORTED;
}

enum tfm_sst_err_t tfm_sst_get(const psa_ps_uid_t *uid,
                               uint32_t data_offset,
                               uint32_t data_length,
                               void *p_data)
{
    (void)uid, (void)data_offset, (void)data_length, (void)p_data;
    return TFM_SST_ERR_NOT_SUPPORTED;
}

enum tfm_sst_err_t tfm_sst_get_info(const psa_ps_uid_t *uid,
                                    struct psa_ps_info_t *p_info)
{
    (void)uid, (void)p_info;
    return TFM_SST_ERR_NOT_SUPPORTED;
}

enum tfm_sst_err_t tfm_sst_remove(const psa_ps_uid_t *uid)
{
    (void)uid, (void)uid;
    return TFM_SST_ERR_NOT_SUPPORTED;
}

enum tfm_sst_err_t tfm_sst_get_support(uint32_t *support_flags)
{
    (void)support_flags;
    return TFM_SST_ERR_NOT_SUPPORTED;
}
