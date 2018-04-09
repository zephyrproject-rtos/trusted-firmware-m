/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SPM_HAL_H__
#define __TFM_SPM_HAL_H__

#include <stdint.h>

/**
 * Holds the data necessary to do isolation for a specific peripheral.
 */
struct tfm_spm_partition_platform_data_t
{
    uint32_t periph_start;
    uint32_t periph_limit;
    uint16_t periph_ppc_bank;
    uint16_t periph_ppc_loc;
};

/**
 * \brief Initialise the platform related fields of a partition DB record.
 *
 * \param[in] partition_id     The id of the partition
 * \param[in] platform_data    The platform fields of the partition DB record to
 *                             init
 */
void tfm_spm_hal_init_platform_data(uint32_t partition_id,
        struct tfm_spm_partition_platform_data_t *platform_data);

#endif /* __TFM_SPM_HAL_H__ */
