/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_initial_attestation_veneers.h"
#include "secure_fw/services/initial_attestation/attestation.h"
#include "tfm_secure_api.h"
#include "tfm_api.h"
#include "spm_partition_defs.h"
#include "psa_client.h"

__tfm_secure_gateway_attributes__
enum psa_attest_err_t
tfm_attest_veneer_get_token(const psa_invec  *in_vec,  uint32_t num_invec,
                                  psa_outvec *out_vec, uint32_t num_outvec)
{
    TFM_CORE_SFN_REQUEST(TFM_SP_INITIAL_ATTESTATION_ID,
                         initial_attest_get_token,
                         in_vec,  num_invec,
                         out_vec, num_outvec);
}
