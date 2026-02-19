/*
 * Copyright (c) 2026 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef PLATFORM_SVC_PRIVATE_H
#define PLATFORM_SVC_PRIVATE_H

#include "svc_num.h"

/* SVC request to write message to SPM UART. Is used to "share" UART with non-secure image. */
#define IFX_SVC_PLATFORM_UART_LOG           TFM_SVC_NUM_PLATFORM_THREAD(0x0U)

/* SVC request to Enable/Disable SysTick */
#define IFX_SVC_PLATFORM_ENABLE_SYSTICK     TFM_SVC_NUM_PLATFORM_THREAD(0x1U)

/* SVC request for system reset to reset the MCU */
#define IFX_SVC_PLATFORM_SYSTEM_RESET       TFM_SVC_NUM_PLATFORM_THREAD(0x2U)

#endif /* PLATFORM_SVC_PRIVATE_H */
