/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_DCU_H__
#define __PLATFORM_DCU_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NUM_OF_DCU_REG 4
#define DCU_LOCK_REG0_VALUE 0xFFFFFFFF
#define DCU_LOCK_REG1_VALUE 0xFFFFFFFF
#define DCU_LOCK_REG2_VALUE 0xFFFFFFFF
#define DCU_LOCK_REG3_VALUE 0xFFFFFFFF

#define DCU_DEBUG_ENABLED_REG0_VALUE 0xFFFFE7FC
#define DCU_DEBUG_ENABLED_REG1_VALUE 0x800703FF
#define DCU_DEBUG_ENABLED_REG2_VALUE 0xFFFFFFFF
#define DCU_DEBUG_ENABLED_REG3_VALUE 0xFFFFFFFF

/**
 * \brief Applies corresponding zone related debug permissions by
 *        setting appropriate dcu registers
 *
 * \param[in] zone Platform specific selected zone
 *
 * \return 0 on success, non-zero on error.
 */
int32_t tfm_plat_apply_debug_permissions(uint16_t zone);

/**
 * \brief Locks the Debug Control Unit so that debug permissions cannot
 *        be altered anymore.
 *
 * \return 0 on success, non-zero on error.
 */
int32_t tfm_plat_lock_dcu(void);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_DCU_H__ */
