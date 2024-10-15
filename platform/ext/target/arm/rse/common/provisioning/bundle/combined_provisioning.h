/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __COMBINED_PROVISIONING_H__
#define __COMBINED_PROVISIONING_H__

#include "tfm_plat_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

enum tfm_plat_err_t do_cm_provision(void);
enum tfm_plat_err_t do_dm_provision(void);

#ifdef __cplusplus
}
#endif

#endif /* __COMBINED_PROVISIONING_H__ */
