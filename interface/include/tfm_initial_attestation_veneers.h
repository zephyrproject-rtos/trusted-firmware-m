/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_INITIAL_ATTESTATION_VENEERS_H__
#define __TFM_INITIAL_ATTESTATION_VENEERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "psa_client.h"

/**
 * \brief Get initial attestation token
 *
 * \param[in]     in_vec     Pointer to in_vec array, which contains input data
 *                           to attestation service
 * \param[in]     num_invec  Number of elements in in_vec array
 * \param[in/out] out_vec    Pointer out_vec array, which contains output data
 *                           to attestation service
 * \param[in]     num_outvec Number of elements in out_vec array
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
enum psa_attest_err_t
tfm_attest_veneer_get_token(const psa_invec  *in_vec,  uint32_t num_invec,
                                  psa_outvec *out_vec, uint32_t num_outvec);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_INITIAL_ATTESTATION_VENEERS_H__ */
