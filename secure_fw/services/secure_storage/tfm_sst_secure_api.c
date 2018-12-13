/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa_protected_storage.h"
#include "tfm_sst_veneers.h"

__attribute__(( section("SFN")))
psa_ps_status_t psa_ps_set(psa_ps_uid_t uid,
                           uint32_t data_length,
                           const void *p_data,
                           psa_ps_create_flags_t create_flags)
{
    enum tfm_sst_err_t err;

    err = tfm_sst_veneer_set(&uid, data_length, p_data, create_flags);

    return TFM_SST_PSA_RETURN(err);
}

__attribute__(( section("SFN")))
psa_ps_status_t psa_ps_get(psa_ps_uid_t uid,
                           uint32_t data_offset,
                           uint32_t data_length,
                           void *p_data)
{
    enum tfm_sst_err_t err;

    err = tfm_sst_veneer_get(&uid, data_offset, data_length, p_data);

    return TFM_SST_PSA_RETURN(err);
}

__attribute__(( section("SFN")))
psa_ps_status_t psa_ps_get_info(psa_ps_uid_t uid, struct psa_ps_info_t *p_info)
{
    enum tfm_sst_err_t err;

    err = tfm_sst_veneer_get_info(&uid, p_info);

    return TFM_SST_PSA_RETURN(err);
}

__attribute__(( section("SFN")))
psa_ps_status_t psa_ps_remove(psa_ps_uid_t uid)
{
    enum tfm_sst_err_t err;

    err = tfm_sst_veneer_remove(&uid);

    return TFM_SST_PSA_RETURN(err);
}

__attribute__(( section("SFN")))
psa_ps_status_t psa_ps_create(psa_ps_uid_t uid, uint32_t size,
                              psa_ps_create_flags_t create_flags)
{
    (void)uid, (void)size, (void)create_flags;
    return PSA_PS_ERROR_NOT_SUPPORTED;
}

__attribute__(( section("SFN")))
psa_ps_status_t psa_ps_set_extended(psa_ps_uid_t uid, uint32_t data_offset,
                                    uint32_t data_length, const void *p_data)
{
    (void)uid, (void)data_offset, (void)data_length, (void)p_data;
    return PSA_PS_ERROR_NOT_SUPPORTED;
}

__attribute__(( section("SFN")))
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
    (void)tfm_sst_veneer_get_support(&support_flags);

    return support_flags;
}
