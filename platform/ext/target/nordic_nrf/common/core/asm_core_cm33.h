/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef ASM_CORE_CM33_H__
#define ASM_CORE_CM33_H__

/* This header file is used by assembly code because the core_cm33.h CMSIS file
 * cannot be included by assembly
 * Static assert in hw_init.c to verify that this matches the original defines.
 */
#define _SCS_BASE_ADDR 0xE000E000
#define _SCS_MPU_CTRL (_SCS_BASE_ADDR + 0xd94)

#endif /* ASM_CORE_CM33_H__*/
