/*
 * Copyright (c) 2023-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "dpe_plat.h"
#include "device_definition.h"
#include "load/ns_client_id_tz.h"
#include "platform_locality.h"
#include "platform_svc_numbers.h"
#include "rse_kmu_slot_ids.h"
#include "sds.h"
#include "tfm_crypto_defs.h"
#include "tfm_plat_defs.h"

#define SDS_DPE_CTX_HANDLE_STRUCT_ID    (0x0000000A)

TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_DPE", "APP-ROT")
psa_key_id_t dpe_plat_get_rot_cdi_key_id(void)
{
    return TFM_BUILTIN_KEY_ID_ROT_CDI;
}

TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_DPE", "APP-ROT")
psa_key_id_t dpe_plat_get_root_attest_key_id(void)
{
    return TFM_BUILTIN_KEY_ID_IAK;
}

TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_DPE", "APP-ROT")
int dpe_plat_share_context_with_ap(int ctx_handle)
{
    enum tfm_plat_err_t ret;
    struct sds_structure_desc struct_desc = {
        .id = SDS_DPE_CTX_HANDLE_STRUCT_ID,
        .size = sizeof(ctx_handle),
        .payload = &ctx_handle,
        .finalize = true
    };

    ret = request_sds_struct_add(&struct_desc);
    if (ret != TFM_PLAT_ERR_SUCCESS) {
        return -1;
    }

    return 0;
}


TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_DPE", "APP-ROT") __attribute__((naked))
static int32_t dpe_plat_get_mailbox_locality(int32_t client_id)
{
    /* Platform SVC handlers can be added for secure world in TF-M */
    __asm("SVC %0\n"
          "BX LR\n"
          : : "I" (TFM_SVC_PLATFORM_GET_MAILBOX_LOCALITY));
}

TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_DPE", "APP-ROT")
int32_t dpe_plat_get_client_locality(int32_t client_id)
{
    if (client_id >= 0) {
        /* Secure partitions on RSE have positive value of client_id */
        return LOCALITY_RSE_S;
    }

#ifdef CONFIG_TFM_USE_TRUSTZONE
    if ((client_id >= TZ_NS_CLIENT_ID_BASE) &&
        (client_id <= TZ_NS_CLIENT_ID_LIMIT)) {
        return LOCALITY_RSE_NS;
    }
#endif /* CONFIG_TFM_USE_TRUSTZONE */

#ifdef TFM_MULTI_CORE_TOPOLOGY
    return dpe_plat_get_mailbox_locality(client_id);
#endif /* TFM_MULTI_CORE_TOPOLOGY */
}
