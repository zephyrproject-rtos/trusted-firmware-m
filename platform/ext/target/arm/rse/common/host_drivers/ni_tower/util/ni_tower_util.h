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

/* Pretty print log for NI-Tower */
#ifdef NI_TOWER_PRETTY_PRINT_LOG_ENABLED
#include <stdio.h>
#include <stdbool.h>

/* Using stderr (unbuffered) instead of stdout (buffered) */
// TODO: Replace fprintf with common TF-M logging function
#define NI_TOWER_DRV_LOG(fmt, ...)  fprintf(stderr, fmt, ##__VA_ARGS__)

#define NI_TOWER_DRV_LOG_HEADING(fmt, ...)                                  \
        do {                                                                \
            NI_TOWER_DRV_LOG("\r\n");                                       \
            NI_TOWER_DRV_LOG(fmt, ##__VA_ARGS__);                          \
            NI_TOWER_DRV_LOG("\r\n");                                       \
            for (int l = 0; l < sprintf(NULL, fmt,  ##__VA_ARGS__); ++l)    \
                NI_TOWER_DRV_LOG("-");                                     \
        } while(0);

/* NI-Tower sorted region data structure */
struct ni_tower_sorted_region_data {
    /* Region Index */
    uint32_t region_idx;
    /* Address used to for sorting */
    uint64_t address;
};

/* NI-Tower sorted region list structure */
struct ni_tower_sorted_region_list {
    /* List of region data in ascending order of base address */
    struct ni_tower_sorted_region_data *sorted_regions;
    /* Number of regions in the sorted list */
    uint32_t sorted_region_count;
};

/**
 * \brief Adds region index to the sorted list in ascending order sorted by
 *        the base address.
 *
 * \param[in] list     Pointer to the sorted region list
 *                     \ref ni_tower_sorted_region_list.
 * \param[in] region   Region Index to be added.
 * \param[in] address  Address used for sorting the list.
 *
 * \return Returns error code as specified in \ref ni_tower_err.
 */
enum ni_tower_err ni_tower_sorted_add_entry(
        struct ni_tower_sorted_region_list *list,
        uint32_t region,
        uint64_t address);

/**
 * \brief Check if a node type value is xSNI node.
 *
 * \param[in]   type   NI-Tower node type value enum
 *                     \ref ni_tower_node_type_value.
 *
 * \return Returns true if node type is a xSNI node, else false.
 */
static inline bool
ni_tower_type_is_xSNI(const enum ni_tower_node_type_value type)
{
    switch (type) {
    case NI_TOWER_ASNI:
    case NI_TOWER_HSNI:
        return true;
    default:
        return false;
    }
}

/**
 * \brief Check if a node type value is xMNI node.
 *
 * \param[in]   type   NI-Tower node type value enum
 *                     \ref ni_tower_node_type_value.
 *
 * \return Returns true if node type is a xMNI node, else false.
 */
static inline bool
ni_tower_type_is_xMNI(const enum ni_tower_node_type_value type)
{
    switch (type) {
    case NI_TOWER_AMNI:
    case NI_TOWER_HMNI:
    case NI_TOWER_PMNI:
        return true;
    default:
        return false;
    }
}
#endif

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
