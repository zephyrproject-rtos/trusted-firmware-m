/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __STAGED_BOOT_H__
#define __STAGED_BOOT_H__

#include <stdint.h>

/**
 * \brief               Staged boot function
 *
 * \return              0 if success, non-zero otherwise
 */
int32_t run_staged_boot(void);

#endif /* __STAGED_BOOT_H__ */
