/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __GIC_720AE_LIB_H__
#define __GIC_720AE_LIB_H__

#include <stdint.h>

int gic_multiple_view_probe(uint32_t view0_base);

int gic_multiple_view_programming(void);

#endif /* __GIC_720AE_LIB_H__ */
