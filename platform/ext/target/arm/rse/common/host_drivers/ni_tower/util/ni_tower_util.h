/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __NI_TOWER_UTIL_H__
#define __NI_TOWER_UTIL_H__

#include "ni_tower_drv.h"
#include <stdint.h>

/**
 * \brief Check if a region overlaps with another
 *
 * \param[in]   region_base_1   Region 1 base address.
 * \param[in]   region_end_1    Region 1 end address.
 * \param[in]   region_base_2   Region 2 base address.
 * \param[in]   region_end_2    Region 2 end address.
 *
 * \return Returns NI_TOWER_ERR_REGION_OVERLAPS is the region overlaps, else
 * NI_TOWER_SUCCESS as specified in \ref ni_tower_err
 */
enum ni_tower_err ni_tower_check_region_overlaps(uint64_t region_base_1,
                                                 uint64_t region_end_1,
                                                 uint64_t region_base_2,
                                                 uint64_t region_end_2);


#endif /* __NI_TOWER_UTIL_H__ */
