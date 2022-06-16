/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "tfm_platform_api.h"
#include "psa_manifest/sid.h"

enum tfm_platform_err_t tfm_platform_system_reset(void)
{
    psa_status_t status = PSA_ERROR_CONNECTION_REFUSED;
    psa_handle_t handle = PSA_NULL_HANDLE;

    handle = psa_connect(TFM_SP_PLATFORM_SYSTEM_RESET_SID,
                         TFM_SP_PLATFORM_SYSTEM_RESET_VERSION);
    if (handle <= 0) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    status = psa_call(handle, PSA_IPC_CALL,
                      NULL, 0, NULL, 0);
    psa_close(handle);

    if (status < PSA_SUCCESS) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    } else {
        return (enum tfm_platform_err_t)status;
    }

}

enum tfm_platform_err_t
tfm_platform_ioctl(tfm_platform_ioctl_req_t request,
                   psa_invec *input, psa_outvec *output)
{
    tfm_platform_ioctl_req_t req = request;
    struct psa_invec in_vec[2] = { {0} };
    size_t inlen, outlen;
    psa_status_t status = PSA_ERROR_CONNECTION_REFUSED;
    psa_handle_t handle = PSA_NULL_HANDLE;

    in_vec[0].base = &req;
    in_vec[0].len = sizeof(req);
    if (input != NULL) {
        in_vec[1].base = input->base;
        in_vec[1].len = input->len;
        inlen = 2;
    } else {
        inlen = 1;
    }

    if (output != NULL) {
        outlen = 1;
    } else {
        outlen = 0;
    }

    handle = psa_connect(TFM_SP_PLATFORM_IOCTL_SID,
                         TFM_SP_PLATFORM_IOCTL_VERSION);
    if (handle <= 0) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    status = psa_call(handle, PSA_IPC_CALL,
                      in_vec, inlen,
                      output, outlen);
    psa_close(handle);

    if (status < PSA_SUCCESS) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    } else {
        return (enum tfm_platform_err_t)status;
    }
}

enum tfm_platform_err_t
tfm_platform_nv_counter_increment(uint32_t counter_id)
{
    psa_status_t status = PSA_ERROR_CONNECTION_REFUSED;
    psa_handle_t handle = PSA_NULL_HANDLE;
    struct psa_invec in_vec[1];

    in_vec[0].base = &counter_id;
    in_vec[0].len = sizeof(counter_id);

    handle = psa_connect(TFM_SP_PLATFORM_NV_COUNTER_SID,
                         TFM_SP_PLATFORM_NV_COUNTER_VERSION);
    if (handle <= 0) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    status = psa_call(handle, TFM_PLATFORM_API_ID_NV_INCREMENT,
                      in_vec, 1, (psa_outvec *)NULL, 0);

    psa_close(handle);

    if (status < PSA_SUCCESS) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    } else {
        return (enum tfm_platform_err_t)status;
    }
}

enum tfm_platform_err_t
tfm_platform_nv_counter_read(uint32_t counter_id,
                             uint32_t size, uint8_t *val)
{
    psa_status_t status = PSA_ERROR_CONNECTION_REFUSED;
    psa_handle_t handle = PSA_NULL_HANDLE;
    struct psa_invec in_vec[1];
    struct psa_outvec out_vec[1];

    in_vec[0].base = &counter_id;
    in_vec[0].len = sizeof(counter_id);

    out_vec[0].base = val;
    out_vec[0].len = size;

    handle = psa_connect(TFM_SP_PLATFORM_NV_COUNTER_SID,
                         TFM_SP_PLATFORM_NV_COUNTER_VERSION);
    if (handle <= 0) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    status = psa_call(handle, TFM_PLATFORM_API_ID_NV_READ,
                      in_vec, 1, out_vec, 1);

    psa_close(handle);

    if (status < PSA_SUCCESS) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    } else {
        return (enum tfm_platform_err_t)status;
    }
}
