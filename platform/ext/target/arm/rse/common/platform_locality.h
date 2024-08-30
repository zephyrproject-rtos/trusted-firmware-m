/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_LOCALITY_H__
#define __PLATFORM_LOCALITY_H__

#include <stdint.h>

#define LOCALITY_NONE       -1
#define LOCALITY_RSE_S       0
#define LOCALITY_RSE_NS      1
#define LOCALITY_AP_S        2
#define LOCALITY_AP_NS       3
/* Note: Further localities can be added for platform with multiple AP worlds,
 * and communicating to RSE via different MHU.
 */

/**
 * \brief Gets the locality based on input client ID.
 *
 * \param[in] client_id    Input client ID
 *
 * \return Returns value corresponding to platform locality
 */
int32_t tfm_plat_get_mailbox_locality(int32_t client_id);

#endif /* __PLATFORM_LOCALITY_H__ */
