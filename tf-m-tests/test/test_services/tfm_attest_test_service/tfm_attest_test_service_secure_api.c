/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/crypto.h"
#include "tfm_attest_test_service_api.h"
#ifdef TFM_PSA_API
#include "psa/client.h"
#include "tfm_api.h"
#include "psa_manifest/sid.h"
#else /* TFM_PSA_API */
#include "tfm_veneers.h"
#endif /* TFM_PSA_API */

psa_status_t tfm_initial_attest_get_public_key(uint8_t *public_key,
                                               size_t  public_key_buf_size,
                                               size_t  *public_key_len,
                                               psa_ecc_family_t *curve_type)
{
    psa_status_t status;

    psa_outvec out_vec[] = {
        {.base = public_key,     .len = public_key_buf_size},
        {.base = curve_type,     .len = sizeof(*curve_type)},
        {.base = public_key_len, .len = sizeof(*public_key_len)}
    };

#ifdef TFM_PSA_API
    psa_handle_t handle = PSA_NULL_HANDLE;

    handle = psa_connect(TFM_ATTEST_TEST_GET_PUBLIC_KEY_SID,
                         TFM_ATTEST_TEST_GET_PUBLIC_KEY_VERSION);
    if (!PSA_HANDLE_IS_VALID(handle)) {
        return PSA_HANDLE_TO_ERROR(handle);
    }

    status = psa_call(handle, PSA_IPC_CALL, NULL, 0,
                      out_vec, IOVEC_LEN(out_vec));
    psa_close(handle);
#else
    status = tfm_attest_test_get_public_key_veneer(NULL, 0,
                                                   out_vec, IOVEC_LEN(out_vec));
#endif

    return status;
}
