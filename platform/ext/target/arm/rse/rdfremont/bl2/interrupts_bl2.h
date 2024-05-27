/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __INTERRUPTS_BL2_H__
#define __INTERRUPTS_BL2_H__

#include <stdint.h>

/* Setup all BL2 interrupt handlers in the IRQ vector table */
int32_t interrupts_bl2_init(void);

#endif /* __INTERRUPTS_BL2_H__ */
