/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BL1_2_DEBUG_H__
#define __BL1_2_DEBUG_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                     Checks for valid conditions and applies
 *                            appropriate permissions DCU mask to open debug
 *
 * \return                    0 on success, non-zero on error.
 */
int32_t b1_2_platform_debug_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BL1_2_DEBUG_H__ */
