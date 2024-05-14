/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "discovery/ni_tower_discovery_drv.h"
#include "ni_tower_apu_drv.h"
#include "ni_tower_apu_reg.h"
#include "util/ni_tower_util.h"

#include <stddef.h>

#define NI_TOWER_APU_ADDRESS_GRAN      (1ULL << 6)
#define NI_TOWER_APU_ADDRESS_MASK      (~(NI_TOWER_APU_ADDRESS_GRAN - 1))
#define NI_TOWER_APU_ADDRESS_H(addr)   ((addr) >> 32)
#define NI_TOWER_APU_ADDRESS_L(addr)   ((addr) & NI_TOWER_APU_ADDRESS_MASK)

#define NI_TOWER_APU_GET64_BASE_ADDRESS(addr, high, low)    \
    addr = (((uint64_t)(high) << 32) | (low)) & NI_TOWER_APU_ADDRESS_MASK

#define NI_TOWER_APU_GET64_END_ADDRESS(addr, high, low)     \
    addr = (((uint64_t)(high) << 32) | (low)) | (NI_TOWER_APU_ADDRESS_GRAN - 1)

static enum ni_tower_err ni_tower_apu_set_addr_range(
    const struct ni_tower_apu_dev *dev,
    const uint32_t region,
    uint64_t base_addr,
    uint64_t end_addr)
{
    struct ni_tower_apu_reg_map* reg;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    base_addr += dev->region_mapping_offset;
    end_addr += dev->region_mapping_offset;

    /* Check alignment of base and end addresses */
    if (((base_addr & (NI_TOWER_APU_ADDRESS_GRAN - 1)) != 0) ||
        ((~end_addr & (NI_TOWER_APU_ADDRESS_GRAN - 1)) != 0)) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    /* Set base address */
    reg->region[region].prbar_high = NI_TOWER_APU_ADDRESS_H(base_addr);
    reg->region[region].prbar_low = NI_TOWER_APU_ADDRESS_L(base_addr);
    /* Set end address */
    reg->region[region].prlar_high = NI_TOWER_APU_ADDRESS_H(end_addr);
    reg->region[region].prlar_low = NI_TOWER_APU_ADDRESS_L(end_addr);

    return NI_TOWER_SUCCESS;
}

static enum ni_tower_err ni_tower_apu_set_access_perms(
    const struct ni_tower_apu_dev *dev, uint32_t region,
    const enum ni_tower_apu_access_perm_type permission,
    const enum ni_tower_apu_entity_type id_select)
{
    struct ni_tower_apu_reg_map* reg;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    switch (id_select) {
    case NI_T_ID_0_SELECT:
        /* Clear permission */
        reg->region[region].prid_low &= ~NI_TOWER_APU_PERM_0_MSK;
        /* Set permission */
        reg->region[region].prid_low |=
            (permission << NI_TOWER_APU_PERM_0_POS) & NI_TOWER_APU_PERM_0_MSK;
        break;
    case NI_T_ID_1_SELECT:
        /* Clear permission */
        reg->region[region].prid_low &= ~NI_TOWER_APU_PERM_1_MSK;
        /* Set permission */
        reg->region[region].prid_low |=
            (permission << NI_TOWER_APU_PERM_1_POS) & NI_TOWER_APU_PERM_1_MSK;
        break;
    case NI_T_ID_2_SELECT:
        /* Clear permission */
        reg->region[region].prid_high &= ~NI_TOWER_APU_PERM_2_MSK;
        /* Set permission */
        reg->region[region].prid_high |=
            (permission << NI_TOWER_APU_PERM_2_POS) & NI_TOWER_APU_PERM_2_MSK;
        break;
    case NI_T_ID_3_SELECT:
        /* Clear permission */
        reg->region[region].prid_high &= ~NI_TOWER_APU_PERM_3_MSK;
        /* Set permission */
        reg->region[region].prid_high |=
            (permission << NI_TOWER_APU_PERM_3_POS) & NI_TOWER_APU_PERM_3_MSK;
        break;
    default:
        return NI_TOWER_ERR_INVALID_ARG;
    }

    return NI_TOWER_SUCCESS;
}

static enum ni_tower_err ni_tower_apu_set_entity_id(
    const struct ni_tower_apu_dev *dev,
    const uint32_t region,
    const uint32_t id_value,
    const enum ni_tower_apu_entity_type id_select)
{
    struct ni_tower_apu_reg_map* reg;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    switch (id_select) {
    case NI_T_ID_0_SELECT:
        /* Clear apu id */
        reg->region[region].prid_low &= ~NI_TOWER_APU_ID_0_MSK;
        /* Set apu id */
        reg->region[region].prid_low |=
            (id_value << NI_TOWER_APU_ID_0_POS) & NI_TOWER_APU_ID_0_MSK;
        break;
    case NI_T_ID_1_SELECT:
        /* Clear apu id */
        reg->region[region].prid_low &= ~NI_TOWER_APU_ID_1_MSK;
        /* Set apu id */
        reg->region[region].prid_low |=
            (id_value << NI_TOWER_APU_ID_1_POS) & NI_TOWER_APU_ID_1_MSK;
        break;
    case NI_T_ID_2_SELECT:
        /* Clear apu id */
        reg->region[region].prid_high &= ~NI_TOWER_APU_ID_2_MSK;
        /* Set apu id */
        reg->region[region].prid_high |=
            (id_value << NI_TOWER_APU_ID_2_POS) & NI_TOWER_APU_ID_2_MSK;
        break;
    case NI_T_ID_3_SELECT:
        /* Clear apu id */
        reg->region[region].prid_high &= ~NI_TOWER_APU_ID_3_MSK;
        /* Set apu id */
        reg->region[region].prid_high |=
            (id_value << NI_TOWER_APU_ID_3_POS) & NI_TOWER_APU_ID_3_MSK;
        break;
    default:
        return NI_TOWER_ERR_INVALID_ARG;
    }

    return NI_TOWER_SUCCESS;
}

static enum ni_tower_err ni_tower_apu_set_lock(
    const struct ni_tower_apu_dev *dev,
    const uint32_t region,
    const enum ni_tower_apu_lock_type lock)
{
    struct ni_tower_apu_reg_map* reg;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    /* Once locked, the region cannot be unlocked unless APU is reset again. */
    reg->region[region].prbar_low |= (lock << NI_TOWER_APU_LOCK_POS) &
                                   NI_TOWER_APU_LOCK_MSK;

    return NI_TOWER_SUCCESS;
}

static enum ni_tower_err ni_tower_apu_set_br(
    const struct ni_tower_apu_dev *dev,
    const uint32_t region,
    const enum ni_tower_apu_br_type background)
{
    struct ni_tower_apu_reg_map* reg;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    /* Clear background bit */
    reg->region[region].prbar_low &= ~NI_TOWER_APU_BR_MSK;
    /* Set background bit */
    reg->region[region].prbar_low |= (background << NI_TOWER_APU_BR_POS) &
                                   NI_TOWER_APU_BR_MSK;

    return NI_TOWER_SUCCESS;
}

static enum ni_tower_err ni_tower_apu_set_region_enable(
    const struct ni_tower_apu_dev *dev,
    const uint32_t region)
{
    enum ni_tower_apu_br_type temp_br_type, curr_br_type;
    struct ni_tower_apu_reg_map* reg;
    uint64_t temp_base_addr, temp_end_addr, curr_base_addr, curr_end_addr;
    uint32_t r_idx;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    /* Clear apu region enable bit */
    reg->region[region].prbar_low &= ~NI_TOWER_APU_REGION_ENABLE_MSK;

    /*
     * Check whether two foreground or two background region overlaps.
     * Foreground region can overlap two different background region, APU
     * prioritises the foreground access permissions.
     */
    curr_br_type = reg->region[region].prbar_low & NI_TOWER_APU_BR_MSK ?
                                        NI_T_BACKGROUND : NI_T_FOREGROUND;

    NI_TOWER_APU_GET64_BASE_ADDRESS(curr_base_addr,
                                    reg->region[region].prbar_high,
                                    reg->region[region].prbar_low);

    NI_TOWER_APU_GET64_END_ADDRESS(curr_end_addr,
                                   reg->region[region].prlar_high,
                                   reg->region[region].prlar_low);

    for (r_idx = 0; r_idx < NI_TOWER_MAX_APU_REGIONS; ++r_idx) {
        temp_br_type = reg->region[r_idx].prbar_low & NI_TOWER_APU_BR_MSK ?
                                        NI_T_BACKGROUND : NI_T_FOREGROUND;
        if ((reg->region[r_idx].prbar_low & NI_TOWER_APU_REGION_ENABLE) &&
            (temp_br_type == curr_br_type))
        {
            NI_TOWER_APU_GET64_BASE_ADDRESS(temp_base_addr,
                                            reg->region[r_idx].prbar_high,
                                            reg->region[r_idx].prbar_low);

            NI_TOWER_APU_GET64_END_ADDRESS(temp_end_addr,
                                           reg->region[r_idx].prlar_high,
                                           reg->region[r_idx].prlar_low);

            if (ni_tower_check_region_overlaps(curr_base_addr, curr_end_addr,
                    temp_base_addr, temp_end_addr) !=
                NI_TOWER_SUCCESS) {
                return NI_TOWER_ERR_REGION_OVERLAPS;
            }
        }
    }

    /* Set apu region enable bit */
    reg->region[region].prbar_low |= NI_TOWER_APU_REGION_ENABLE;

    return NI_TOWER_SUCCESS;
}

static enum ni_tower_err ni_tower_apu_set_id_valid(
    const struct ni_tower_apu_dev *dev,
    const uint32_t region,
    const enum ni_tower_apu_entity_valid_type valid)
{
    struct ni_tower_apu_reg_map* reg;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    /* Clear id valid */
    reg->region[region].prbar_low &= ~NI_TOWER_APU_ID_VALID_MSK;
    /* Set id valid */
    reg->region[region].prlar_low |= (valid << NI_TOWER_APU_ID_VALID_POS) &
                                   NI_TOWER_APU_ID_VALID_MSK;

    return NI_TOWER_SUCCESS;
}

enum ni_tower_err ni_tower_apu_enable(const struct ni_tower_apu_dev *dev)
{
    struct ni_tower_apu_reg_map* reg;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    /*
     * Cannot disable this field once enabled. This can only cleared by
     * APU reset.
     */
    reg->apu_ctlr |= NI_TOWER_APU_CTLR_APU_ENABLE;

    return NI_TOWER_SUCCESS;
}

enum ni_tower_err ni_tower_apu_sync_err_enable(
    const struct ni_tower_apu_dev *dev)
{
    struct ni_tower_apu_reg_map* reg;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    /* Clear sync_err_en */
    reg->apu_ctlr &= ~NI_TOWER_APU_CTLR_SYNC_ERROR_EN_MSK;
    /* Set sync_err_en */
    reg->apu_ctlr |= NI_TOWER_APU_CTLR_SYNC_ERROR_EN;

    return NI_TOWER_SUCCESS;
}

enum ni_tower_err ni_tower_apu_dev_init(
    const struct ni_tower_dev *ni_tower_dev,
    const struct ni_tower_component_node *component,
    const uint64_t region_mapping_offset,
    struct ni_tower_apu_dev *dev)
{
    enum ni_tower_err err;
    uint32_t off_addr;
    struct ni_tower_discovery_node root = {
        .node_type = NI_TOWER_CFGNI,
        .node_id = 0,
        .node_off_addr = 0x0
    };

    if (ni_tower_dev == NULL || ni_tower_dev->periphbase == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    if (component == NULL || dev == NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    /* Discover offset address for the APU */
    err = ni_tower_discover_offset(
            ni_tower_dev, &root,
            component->type,
            component->id,
            NI_TOWER_APU, &off_addr);
    if (err != NI_TOWER_SUCCESS) {
        return err;
    }

    dev->base = ni_tower_dev->periphbase + off_addr;
    dev->region_mapping_offset = region_mapping_offset;

    return NI_TOWER_SUCCESS;
}

enum ni_tower_err ni_tower_apu_configure_region(
    const struct ni_tower_apu_dev *dev,
    const struct ni_tower_apu_reg_cfg_info *cfg_info,
    const uint32_t region)
{
    struct ni_tower_apu_reg_map *reg;
    enum ni_tower_err err;
    uint32_t id_idx;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map *)dev->base;

    /*
     * If this region is locked, disallow re-configuration of this APU region
     */
    if (reg->region[region].prbar_low & NI_TOWER_APU_LOCK) {
        return NI_TOWER_ERR_NOT_PERMITTED;
    }

    err = ni_tower_apu_set_addr_range(dev, region,
                                      cfg_info->base_addr,
                                      cfg_info->end_addr);
    if (err != NI_TOWER_SUCCESS) {
        return err;
    }

    err = ni_tower_apu_set_br(dev, region, cfg_info->background);
    if (err != NI_TOWER_SUCCESS) {
        return err;
    }

    err = ni_tower_apu_set_id_valid(dev, region, cfg_info->id_valid);
    if (err != NI_TOWER_SUCCESS) {
        return err;
    }

    for (id_idx = 0; id_idx < NI_TOWER_APU_NUM_ENTITIES; ++id_idx) {
        err = ni_tower_apu_set_access_perms(dev, region,
                                            cfg_info->permissions[id_idx],
                                            (1 << id_idx));
        if (err != NI_TOWER_SUCCESS) {
            return err;
        }

        err = ni_tower_apu_set_entity_id(dev, region,
                                         cfg_info->entity_ids[id_idx],
                                         (1 << id_idx));
        if (err != NI_TOWER_SUCCESS) {
            return err;
        }
    }

    if (cfg_info->region_enable == NI_T_REGION_ENABLE) {
        err = ni_tower_apu_set_region_enable(dev, region);
        if (err != NI_TOWER_SUCCESS) {
            return err;
        }
    }

    err = ni_tower_apu_set_lock(dev, region, cfg_info->lock);
    if (err != NI_TOWER_SUCCESS) {
        return err;
    }

    return NI_TOWER_SUCCESS;
}

static enum ni_tower_err get_next_available_region(
    const struct ni_tower_apu_dev *dev,
    uint32_t *region)
{
    struct ni_tower_apu_reg_map* reg;
    uint32_t r_idx;

    if (dev == NULL || dev->base == (uintptr_t)NULL) {
        return NI_TOWER_ERR_INVALID_ARG;
    }

    reg = (struct ni_tower_apu_reg_map*)dev->base;

    for (r_idx = 0; r_idx < NI_TOWER_MAX_APU_REGIONS; ++r_idx) {
        if (!(reg->region[r_idx].prbar_low & NI_TOWER_APU_REGION_ENABLE)) {
            *region = r_idx;
            return NI_TOWER_SUCCESS;
        }
    }

    return NI_TOWER_ERR;
}

enum ni_tower_err ni_tower_apu_configure_next_available_region(
    const struct ni_tower_apu_dev *dev,
    const struct ni_tower_apu_reg_cfg_info *cfg_info)
{
    enum ni_tower_err err;
    uint32_t next_available_region;

    err = get_next_available_region(dev, &next_available_region);
    if (err != NI_TOWER_SUCCESS) {
        return err;
    }

    return ni_tower_apu_configure_region(dev, cfg_info, next_available_region);
}
