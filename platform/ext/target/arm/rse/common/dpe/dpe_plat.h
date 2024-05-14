/*
 * Copyright (c) 2023-2024 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __DPE_PLAT_H__
#define __DPE_PLAT_H__

#include <stddef.h>
#include <stdint.h>

#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Get the RoT CDI key ID.
 *
 * \return Returns key ID.
 */
psa_key_id_t dpe_plat_get_rot_cdi_key_id(void);

/**
 * \brief Get the key ID of the root attestation key.
 *
 * \return Returns key ID.
 */
psa_key_id_t dpe_plat_get_root_attest_key_id(void);

/**
 * \brief Share derived context handle with AP monitor.
 *
 * \param[in] ctx_handle    New context handle to share with AP
 *
 * \return Returns 0 on success or a negative integer on failure.
 */
int dpe_plat_share_context_with_ap(int ctx_handle);

#ifdef __cplusplus
}
#endif

#endif /* __DPE_PLAT_H__ */
