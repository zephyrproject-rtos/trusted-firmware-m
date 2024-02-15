/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __SMMU_V3_MEMORY_MAP_H__
#define __SMMU_V3_MEMORY_MAP_H__

/* SMMU Section Map relative to SMMU base */

/* SMMU TCU base */
#define SMMU_TCU_BASE        0x0000000ULL
/* SMMU TCU limit */
#define SMMU_TCU_LIMIT       0x00AFFFCULL

/* SMMU TCU Page Bases relative to TCU base */

/* TCU SMMU root control registers page base */
#define SMMU_ROOT_CONTROL_REGISTERS_PAGE_BASE    0xA0000U
/* TCU SMMU root control registers page base */
#define SMMU_ROOT_CONTROL_REGISTERS_PAGE_LIMIT   0xAFFFFU

#endif /* __SMMU_V3_MEMORY_MAP_H__ */
