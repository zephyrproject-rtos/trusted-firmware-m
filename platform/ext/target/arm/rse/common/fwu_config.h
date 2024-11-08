/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __FWU_CONFIG_H__
#define __FWU_CONFIG_H__

/* Number of firmware banks (#banks) */
#define FWU_BANK_COUNT 2

/* Number of metadata replicas */
#define FWU_MD_REPLICAS     2u

#if (FWU_BANK_COUNT > 4)
/* DEN0118 Firmware update metadata: v2 restricts #banks to 4 */
#   error "Too many firmware banks!"
#endif

#endif /* __FWU_CONFIG_H__ */

