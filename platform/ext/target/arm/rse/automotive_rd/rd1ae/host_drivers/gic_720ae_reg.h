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
#define GICR_IIDR           0x0004
#define GICR_TYPER          0x0008
#define GICR_PWRR           0x0024
#define GICR_VIEWR          0x002C
#define GICR_PIDR2          GICD_PIDR2

/* GICR_IIDR GIC model encoding */
#define GICR_IIDR_ARM_GIC_720AE  0x0200043b

/* GICR_PWRR fields */
#define GICR_PWRR_RDPD_SHIFT     0
#define GICR_PWRR_RDGPD_SHIFT    2
#define GICR_PWRR_RDGPO_SHIFT    3
#define GICR_PWRR_RDPD           (1U << GICR_PWRR_RDPD_SHIFT)
#define GICR_PWRR_RDGPD          (1U << GICR_PWRR_RDGPD_SHIFT)
#define GICR_PWRR_RDGPO          (1U << GICR_PWRR_RDGPO_SHIFT)
#define GICR_PWRR_ON             (0U << GICR_PWRR_RDPD_SHIFT)

#define GICR_VIEWR_MASK     0x3

#define GICR_CLUS0_ID       0x0
#define GICR_CLUS1_ID       0x1
#define GICR_CLUS2_ID       0x3

/* Bit to check the last Redistrubitor on this socket */
#define GICR_TYPER_LAST     (1U << 4)

#endif /* __GIC_720AE_REG_H__ */
