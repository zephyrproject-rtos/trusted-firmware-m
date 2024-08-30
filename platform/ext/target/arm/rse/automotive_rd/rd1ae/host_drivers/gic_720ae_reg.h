/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __GIC_720AE_REG_H__
#define __GIC_720AE_REG_H__

#include <stdint.h>

/* GIC components offset in each view */
#define SI_GICD_OFFSET      0x0
#define SI_GICR_OFFSET      0x40000
#define SI_GICR_FRAME_SZ_v3 0x20000
#define SI_GICR_FRAME_SZ_v4 0x40000

/* Register offset of GICD */
#define GICD_CTLR           0x0
#define GICD_CFGID          0xF000
#define GICD_IVIEWnR        0xF600
#define GICD_IVIEWnRE       0xF800
#define GICD_PIDR2          0xFFE8

/* GICD_PIDR2 to check the GIC version */
#define GICD_PIDR2_ARCH_MASK  0xF0
#define GICD_PIDR2_ARCH_GICv3 0x3
#define GICD_PIDR2_ARCH_GICv4 0x4

/*
 * GICD_CFGID MV bit [53] indicates Multiple View support.
 * Define a bit-21 macro for high 32-bit to check this feature.
 */
#define GICD_CFGID_HI_MV     (1U << 21)

/* Register offset of GICR */
#define GICR_TYPER          0x0008
#define GICR_VIEWR          0x002C
#define GICR_PIDR2          GICD_PIDR2

#define GICR_VIEWR_MASK     0x3

/* Bit to check the last Redistrubitor on this socket */
#define GICR_TYPER_LAST     (1U << 4)

#endif /* __GIC_720AE_REG_H__ */
