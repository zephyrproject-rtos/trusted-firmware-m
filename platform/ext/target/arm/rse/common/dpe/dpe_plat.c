/*
 * Copyright (c) 2023-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "dpe_plat.h"
#include "device_definition.h"
#include "rse_kmu_slot_ids.h"
#include "tfm_crypto_defs.h"
#include "tfm_plat_defs.h"

#include "sds.h"

#define SDS_DPE_CTX_HANDLE_STRUCT_ID    (0x0000000A)

int dpe_plat_get_rot_cdi(uint8_t *buf, size_t buf_len)
{
    enum kmu_error_t err;

    err = kmu_get_key(&KMU_DEV_S, RSE_KMU_SLOT_ROT_CDI, buf, buf_len);
    if (err != KMU_ERROR_NONE) {
        return -1;
    }

    return 0;
}

psa_key_id_t dpe_plat_get_root_attest_key_id(void)
{
    return TFM_BUILTIN_KEY_ID_IAK;
}

int dpe_plat_share_context_with_ap(int ctx_handle)
{
    enum tfm_plat_err_t ret;
    struct sds_structure_desc struct_desc = {
        .id = SDS_DPE_CTX_HANDLE_STRUCT_ID,
        .size = sizeof(ctx_handle),
        .payload = &ctx_handle,
        .finalize = true
    };

    ret = sds_struct_add(&struct_desc);
    if (ret != TFM_PLAT_ERR_SUCCESS) {
        return -1;
    }

    return 0;
}
