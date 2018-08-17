/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ATTESTATION_H__
#define __ATTESTATION_H__

#include "psa_initial_attestation_api.h"
#include "psa_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Extension of shared data TLVs defined in bl2/include/tfm_boot_status.h */
#define TLV_MINOR_IAS_BOOT_SEED       0x0f
#define TLV_MINOR_IAS_DEVICE_ID       0x10
#define TLV_MINOR_IAS_CHALLENGE       0x11
#define TLV_MINOR_IAS_CALLER_ID       0x12

/*!
 * \brief Initialise the initial attestation service during the TF-M boot up
 *        process.
 *
 * \return Returns PSA_ATTEST_ERR_SUCCESS if init has been completed,
 *         otherwise error as specified in \ref psa_attest_err_t
 */
enum psa_attest_err_t attest_init(void);

/*!
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
initial_attest_get_token(const psa_invec  *in_vec,  uint32_t num_invec,
                               psa_outvec *out_vec, uint32_t num_outvec);


#ifdef __cplusplus
}
#endif

#endif /* __ATTESTATION_H__ */
