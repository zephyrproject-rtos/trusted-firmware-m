/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_sst_veneers.h"

#include "secure_fw/core/tfm_secure_api.h"
#include "secure_fw/services/secure_storage/tfm_protected_storage.h"
#include "secure_fw/spm/spm_partition_defs.h"

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_set(const psa_ps_uid_t *uid,
                                      uint32_t data_length,
                                      const void *p_data,
                                      psa_ps_create_flags_t create_flags)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, tfm_sst_set,
                         uid, data_length, p_data, create_flags);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_get(const psa_ps_uid_t *uid,
                                      uint32_t data_offset,
                                      uint32_t data_length,
                                      void *p_data)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, tfm_sst_get,
                         uid, data_offset, data_length, p_data);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_get_info(const psa_ps_uid_t *uid,
                                           struct psa_ps_info_t *p_info)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, tfm_sst_get_info,
                         uid, p_info, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_remove(const psa_ps_uid_t *uid)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, tfm_sst_remove,
                         uid, 0, 0, 0);
}

__tfm_secure_gateway_attributes__
enum tfm_sst_err_t tfm_sst_veneer_get_support(uint32_t *support_flags)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_STORAGE_ID, tfm_sst_get_support,
                         support_flags, 0, 0, 0);
}
