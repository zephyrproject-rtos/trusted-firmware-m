/*
 * Copyright (c) 2018-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_NSPM_H__
#define __TFM_NSPM_H__

#include <stdint.h>

#define TFM_NS_CLIENT_INVALID_ID            ((int32_t)0)

#ifdef CONFIG_TFM_USE_TRUSTZONE

#endif /* CONFIG_TFM_USE_TRUSTZONE */

/**
 * \brief initialise the NS context database
 */
void tfm_nspm_ctx_init(void);

/**
 * \brief Get the client ID of the current NS client
 *
 * \return The client id of the current NS client. 0 (invalid client id) is
 *         returned in case of error.
 */
int32_t tfm_nspm_get_current_client_id(void);

/**
 * \brief Register a non-secure client ID range.
 *
 * \param[in] client_id_base  The minimum client ID for this client.
 * \param[in] client_id_limit The maximum client ID for this client.
 */
void tz_ns_agent_register_client_id_range(int32_t client_id_base,
                                          int32_t client_id_limit);

#endif /* __TFM_NSPM_H__ */
