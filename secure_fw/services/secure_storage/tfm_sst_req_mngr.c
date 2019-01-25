/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_sst_req_mngr.h"

#include "secure_fw/core/tfm_secure_api.h"
#include "tfm_api.h"
#include "tfm_protected_storage.h"

psa_status_t tfm_sst_set_req(struct psa_invec *in_vec, size_t in_len,
                             struct psa_outvec *out_vec, size_t out_len)
{
    psa_ps_uid_t uid;
    uint32_t data_length;
    int32_t client_id;
    const void *p_data;
    enum tfm_status_e status;
    psa_ps_create_flags_t create_flags;
    psa_ps_status_t *err;

    if ((in_len != 3) || (out_len != 1)) {
        /* The number of arguments are incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(psa_ps_uid_t)) {
        /* The input argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    uid = *((psa_ps_uid_t *)in_vec[0].base);

    p_data = (const void *)in_vec[1].base;
    data_length = in_vec[1].len;

    if (in_vec[2].len != sizeof(psa_ps_create_flags_t)) {
        /* The input argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    create_flags = *(psa_ps_create_flags_t *)in_vec[2].base;

    if (out_vec[0].len != sizeof(psa_ps_status_t)) {
        /* The output argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    err = (psa_ps_status_t *)out_vec[0].base;

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != TFM_SUCCESS) {
        return PSA_CONNECTION_REFUSED;
    }

    *err = tfm_sst_set(client_id, uid, data_length, p_data, create_flags);

    return PSA_SUCCESS;
}

psa_status_t tfm_sst_get_req(struct psa_invec *in_vec, size_t in_len,
                             struct psa_outvec *out_vec, size_t out_len)
{
    uint32_t data_offset;
    uint32_t data_length;
    int32_t client_id;
    psa_ps_uid_t uid;
    void *p_data;
    enum tfm_status_e status;
    psa_ps_status_t *err;

    if ((in_len != 2) || (out_len != 2)) {
        /* The number of arguments are incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(psa_ps_uid_t)) {
        /* The input argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    uid = *((psa_ps_uid_t *)in_vec[0].base);

    if (in_vec[1].len != sizeof(data_offset)) {
        /* The input argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    data_offset = *(uint32_t *)in_vec[1].base;

    if (out_vec[0].len != sizeof(psa_ps_status_t)) {
        /* The output argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    err = (psa_ps_status_t *)out_vec[0].base;

    p_data = (void *)out_vec[1].base;
    data_length = out_vec[1].len;

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != TFM_SUCCESS) {
        return PSA_CONNECTION_REFUSED;
    }

    *err = tfm_sst_get(client_id, uid, data_offset, data_length, p_data);

    return PSA_SUCCESS;

}

psa_status_t tfm_sst_get_info_req(struct psa_invec *in_vec, size_t in_len,
                                  struct psa_outvec *out_vec, size_t out_len)
{
    int32_t client_id;
    psa_ps_uid_t uid;
    struct psa_ps_info_t *p_info;
    enum tfm_status_e status;
    psa_ps_status_t *err;

    if ((in_len != 1) || (out_len != 2)) {
        /* The number of arguments are incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(psa_ps_uid_t)) {
        /* The input argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    uid = *((psa_ps_uid_t *)in_vec[0].base);

    if (out_vec[0].len != sizeof(psa_ps_status_t)) {
        /* The output argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    err = (psa_ps_status_t *)out_vec[0].base;

    if (out_vec[1].len != sizeof(struct psa_ps_info_t)) {
        /* The output argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    p_info = (struct psa_ps_info_t *)out_vec[1].base;

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != TFM_SUCCESS) {
        return PSA_CONNECTION_REFUSED;
    }

    *err = tfm_sst_get_info(client_id, uid, p_info);

    return PSA_SUCCESS;
}

psa_status_t tfm_sst_remove_req(struct psa_invec *in_vec, size_t in_len,
                                struct psa_outvec *out_vec, size_t out_len)
{
    int32_t client_id;
    psa_ps_uid_t uid;
    enum tfm_status_e status;
    psa_ps_status_t *err;

    if ((in_len != 1) || (out_len != 1)) {
        /* The number of arguments are incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(psa_ps_uid_t)) {
        /* The input argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    uid = *((psa_ps_uid_t *)in_vec[0].base);

    if (out_vec[0].len != sizeof(psa_ps_status_t)) {
        /* The output argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    err = (psa_ps_status_t *)out_vec[0].base;

    /* Get the caller's client ID */
    status = tfm_core_get_caller_client_id(&client_id);
    if (status != TFM_SUCCESS) {
        return PSA_CONNECTION_REFUSED;
    }

    *err = tfm_sst_remove(client_id, uid);

    return PSA_SUCCESS;
}

psa_status_t tfm_sst_get_support_req(struct psa_invec *in_vec, size_t in_len,
                                     struct psa_outvec *out_vec, size_t out_len)
{
    uint32_t *support_flags;

    if ((in_len != 0) || (out_len != 1)) {
        /* The number of arguments are incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    if (out_vec[0].len != sizeof(*support_flags)) {
        /* The output argument size is incorrect */
        return PSA_CONNECTION_REFUSED;
    }

    support_flags = (uint32_t *)out_vec[0].base;

    *support_flags = tfm_sst_get_support();

    return PSA_SUCCESS;
}
