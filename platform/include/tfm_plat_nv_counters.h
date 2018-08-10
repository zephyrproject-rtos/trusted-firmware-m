/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLAT_NV_COUNTERS_H__
#define __TFM_PLAT_NV_COUNTERS_H__

/**
 * \file tfm_plat_nv_counters.h
 *
 * \note The interfaces defined in this file must be implemented for each
 *       SoC.
 */

#include <stdint.h>
#include "tfm_plat_defs.h"

enum tfm_nv_counter_t {
    TFM_SST_NV_COUNTER_1 = 0,
    TFM_SST_NV_COUNTER_2,
    TFM_SST_NV_COUNTER_3,
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initialises all non-volatile (NV) counters.
 *
 * \return  TFM_PLAT_ERR_SUCCESS if the initialization succeeds, otherwise
 *          TFM_PLAT_ERR_SYSTEM_ERR
 */
enum tfm_plat_err_t tfm_plat_init_nv_counter(void);

/**
 * \brief Reads the given non-volatile (NV) counter.
 *
 * \param[in]  counter_id  NV counter ID.
 * \param[in]  size        Size of the buffer to store NV counter value
 *                         in bytes.
 * \param[out] val         Pointer to store the current NV counter value.
 *
 * \return  TFM_PLAT_ERR_SUCCESS if the value is read correctly. Otherwise,
 *          it returns TFM_PLAT_ERR_SYSTEM_ERR.
 */
enum tfm_plat_err_t tfm_plat_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                             uint32_t size, uint8_t *val);

/**
 * \brief Increments the given non-volatile (NV) counter.
 *
 * \param[in] counter_id  NV counter ID.
 *
 * \return  When the NV counter reaches its maximum value, the
 *          TFM_PLAT_ERR_MAX_VALUE error is returned to indicate the value
 *          cannot be incremented. Otherwise, it returns TFM_PLAT_ERR_SUCCESS.
 */
enum tfm_plat_err_t tfm_plat_increment_nv_counter(
                                              enum tfm_nv_counter_t counter_id);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLAT_NV_COUNTERS_H__ */
