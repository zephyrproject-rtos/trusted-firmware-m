/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa_protected_storage.h"

#include "tfm_ns_lock.h"
#include "tfm_sst_defs.h"
#include "tfm_sst_veneers.h"

psa_ps_status_t psa_ps_set(psa_ps_uid_t uid,
                           uint32_t data_length,
                           const void *p_data,
                           psa_ps_create_flags_t create_flags)
{
    enum tfm_sst_err_t err;

    err = tfm_ns_lock_dispatch((veneer_fn)tfm_sst_veneer_set,
                               (uint32_t)&uid,
                               (uint32_t)data_length,
                               (uint32_t)p_data,
                               (uint32_t)create_flags);

    return TFM_SST_PSA_RETURN(err);
}

psa_ps_status_t psa_ps_get(psa_ps_uid_t uid,
                           uint32_t data_offset,
                           uint32_t data_length,
                           void *p_data)
{
    enum tfm_sst_err_t err;

    err = tfm_ns_lock_dispatch((veneer_fn)tfm_sst_veneer_get,
                               (uint32_t)&uid,
                               (uint32_t)data_offset,
                               (uint32_t)data_length,
                               (uint32_t)p_data);

    return TFM_SST_PSA_RETURN(err);
}

psa_ps_status_t psa_ps_get_info(psa_ps_uid_t uid, struct psa_ps_info_t *p_info)
{
    enum tfm_sst_err_t err;

    err = tfm_ns_lock_dispatch((veneer_fn)tfm_sst_veneer_get_info,
                               (uint32_t)&uid,
                               (uint32_t)p_info,
                               (uint32_t)0,
                               (uint32_t)0);

    return TFM_SST_PSA_RETURN(err);
}

psa_ps_status_t psa_ps_remove(psa_ps_uid_t uid)
{
    enum tfm_sst_err_t err;

    err = tfm_ns_lock_dispatch((veneer_fn)tfm_sst_veneer_remove,
                               (uint32_t)&uid,
                               (uint32_t)0,
                               (uint32_t)0,
                               (uint32_t)0);

    return TFM_SST_PSA_RETURN(err);
}

psa_ps_status_t psa_ps_create(psa_ps_uid_t uid, uint32_t size,
                              psa_ps_create_flags_t create_flags)
{
    (void)uid, (void)size, (void)create_flags;
    return PSA_PS_ERROR_NOT_SUPPORTED;
}

psa_ps_status_t psa_ps_set_extended(psa_ps_uid_t uid, uint32_t data_offset,
                                    uint32_t data_length, const void *p_data)
{
    (void)uid, (void)data_offset, (void)data_length, (void)p_data;
    return PSA_PS_ERROR_NOT_SUPPORTED;
}

uint32_t psa_ps_get_support(void)
{
    uint32_t support_flags;

    /* Initialise support_flags to a sensible default, to avoid returning an
     * uninitialised value in case the secure function fails.
     */
    support_flags = 0;

    /* The PSA API does not return an error, so any error from TF-M is
     * ignored.
     */
    (void)tfm_ns_lock_dispatch((veneer_fn)tfm_sst_veneer_get_support,
                               (uint32_t)&support_flags,
                               (uint32_t)0,
                               (uint32_t)0,
                               (uint32_t)0);

    return support_flags;
}
