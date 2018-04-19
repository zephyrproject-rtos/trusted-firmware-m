/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_NS_PSA_SVC_H__
#define __TFM_NS_PSA_SVC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "interface/include/psa_client.h"

uint32_t tfm_psa_ns_version(uint32_t sid);

psa_handle_t tfm_psa_ns_connect(uint32_t sid, uint32_t minor_version);

psa_error_t tfm_psa_ns_call(psa_handle_t handle,
                            const psa_invec *in_vecs,
                            const psa_invec *out_vecs);

psa_error_t tfm_psa_ns_close(psa_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_NS_PSA_SVC_H__ */
