/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/client.h"
#include "psa/update.h"
#include "psa_manifest/sid.h"
#include "tfm_api.h"

psa_status_t psa_fwu_write(const psa_image_id_t image_id,
                           size_t block_offset,
                           const void *block,
                           size_t block_size)
{
    psa_invec in_vec[] = {
        { .base = &image_id, .len = sizeof(image_id) },
        { .base = &block_offset, .len = sizeof(block_offset) },
        { .base = block, .len = block_size }
    };

    return psa_call(TFM_FIRMWARE_UPDATE_SERVICE_HANDLE, TFM_FWU_WRITE,
                    in_vec, IOVEC_LEN(in_vec), NULL, 0);
}

psa_status_t psa_fwu_install(const psa_image_id_t image_id,
                             psa_image_id_t *dependency_uuid,
                             psa_image_version_t *dependency_version)
{
    psa_invec in_vec[] = {
        { .base = &image_id, .len = sizeof(image_id) }
    };

    psa_outvec out_vec[] = {
        { .base = dependency_uuid, .len = sizeof(*dependency_uuid) },
        { .base = dependency_version, .len = sizeof(*dependency_version)}
    };

    if ((dependency_uuid == NULL) || (dependency_version == NULL)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return psa_call(TFM_FIRMWARE_UPDATE_SERVICE_HANDLE, TFM_FWU_INSTALL,
                    in_vec, IOVEC_LEN(in_vec), out_vec, IOVEC_LEN(out_vec));
}

psa_status_t psa_fwu_abort(const psa_image_id_t image_id)
{
    psa_invec in_vec[] = {
        { .base = &image_id, .len = sizeof(image_id) }
    };

    return psa_call(TFM_FIRMWARE_UPDATE_SERVICE_HANDLE, TFM_FWU_ABORT,
                    in_vec, IOVEC_LEN(in_vec), NULL, 0);
}

psa_status_t psa_fwu_query(const psa_image_id_t image_id, psa_image_info_t *info)
{
    psa_invec in_vec[] = {
        { .base = &image_id, .len = sizeof(image_id) }
    };
    psa_outvec out_vec[] = {
        { .base = info, .len = sizeof(*info)}
    };

    return psa_call(TFM_FIRMWARE_UPDATE_SERVICE_HANDLE, TFM_FWU_QUERY,
                    in_vec, IOVEC_LEN(in_vec), out_vec, IOVEC_LEN(out_vec));
}

psa_status_t psa_fwu_request_reboot(void)
{
    return psa_call(TFM_FIRMWARE_UPDATE_SERVICE_HANDLE, TFM_FWU_REQUEST_REBOOT,
                    NULL, 0, NULL, 0);
}

psa_status_t psa_fwu_accept(psa_image_id_t image_id)
{
    psa_invec in_vec[] = {
        { .base = &image_id, .len = sizeof(image_id) }
    };

    return psa_call(TFM_FIRMWARE_UPDATE_SERVICE_HANDLE, TFM_FWU_ACCEPT,
                    in_vec, IOVEC_LEN(in_vec), NULL, 0);
}

psa_status_t psa_fwu_set_manifest(psa_image_id_t image_id,
                                  const void *manifest,
                                  size_t manifest_size,
                                  psa_hash_t *manifest_dependency)
{
    psa_status_t status;

    status = PSA_ERROR_NOT_SUPPORTED;

    return status;
}
