/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "ni_tower_util.h"

enum ni_tower_err ni_tower_check_region_overlaps(uint64_t region_base_1,
                                                 uint64_t region_end_1,
                                                 uint64_t region_base_2,
                                                 uint64_t region_end_2)
{
    if (region_base_2 > region_end_1) {
        return NI_TOWER_SUCCESS;
    } else if (region_base_2 >= region_base_1) {
        return NI_TOWER_ERR_REGION_OVERLAPS;
    } else if (region_end_2 >= region_base_1) {
        return NI_TOWER_ERR_REGION_OVERLAPS;
    }

    return NI_TOWER_SUCCESS;
}
