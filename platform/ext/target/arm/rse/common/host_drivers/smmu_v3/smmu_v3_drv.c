/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "smmu_v3_drv.h"

#include <stddef.h>
#include <stdint.h>

#include "smmu_v3_memory_map.h"
#include "tfm_hal_device_header.h"

/* Root Control Page (note: contains some 64-bit registers) */
__PACKED_STRUCT _smmu_root_ctrl_page_t {
    __IM  uint32_t idr0;         /* 0x000 */
    const uint32_t reserved0[1];
    __IM  uint32_t iidr;         /* 0x008 */
    const uint32_t reserved1[5];
    __IOM uint32_t cr0;          /* 0x020 */
    __IM  uint32_t cr0ack;       /* 0x024 */
    __IOM uint64_t gpt_base;     /* 0x028-0x02C */
    __IOM uint64_t gpt_base_cfg; /* 0x030-0x034 */
    __IOM uint64_t gpf_far;      /* 0x038-0x03C */
    __IOM uint64_t gpf_cfg_far;  /* 0x040-0x044 */
    const uint32_t reserved2[2];
    __IOM uint64_t tlbi;         /* 0x050-0x054 */
    __IOM uint32_t tlbi_ctrl;    /* 0x058 */
};

#define SMMU_ROOT_CR0_ACCESSEN_POS (0U)
#define SMMU_ROOT_CR0_ACCESSEN     (0x1UL << SMMU_ROOT_CR0_ACCESSEN_POS)
#define SMMU_ROOT_CR0_GPCEN_POS    (1U)
#define SMMU_ROOT_CR0_GPCEN        (0x1UL << SMMU_ROOT_CR0_GPCEN_POS)

/* Set bits of cr0 register and wait for signal from ack register */
static enum smmu_error_t smmu_cr0_setbits(struct smmu_dev_t *dev, uint32_t val)
{
    struct _smmu_root_ctrl_page_t *root_page;
    uint64_t timeout_counter;
    uint32_t write_val;

    if (dev == NULL || dev->smmu_base == NULL) {
        return SMMU_ERR_INVALID_PARAM;
    }

    /* Get the root control registers page on TCU */
    root_page = (struct _smmu_root_ctrl_page_t *)(dev->smmu_base +
            SMMU_TCU_BASE + SMMU_ROOT_CONTROL_REGISTERS_PAGE_BASE);

    write_val = root_page->cr0 | val;
    root_page->cr0 = write_val;

    /* Update is not immediate so wait for SMMU to acknowledge change */
    timeout_counter = 0;
    while (root_page->cr0ack != write_val) {
        if (timeout_counter++ > dev->ack_timeout) {
            return SMMU_ERR_TIMEOUT;
        }
    }

    return SMMU_ERR_NONE;
}

/* Clear bits of cr0 register and wait for signal from ack register */
static enum smmu_error_t smmu_cr0_clearbits(struct smmu_dev_t *dev,
                                            uint32_t val)
{
    struct _smmu_root_ctrl_page_t *root_page;
    uint64_t timeout_counter;
    uint32_t write_val;

    if (dev == NULL || dev->smmu_base == NULL) {
        return SMMU_ERR_INVALID_PARAM;
    }

    /* Get the root control registers page on TCU */
    root_page = (struct _smmu_root_ctrl_page_t *)(dev->smmu_base +
            SMMU_TCU_BASE + SMMU_ROOT_CONTROL_REGISTERS_PAGE_BASE);

    write_val = root_page->cr0 & ~val;
    root_page->cr0 = write_val;

    /* Update is not immediate so wait for SMMU to acknowledge change */
    timeout_counter = 0;
    while (root_page->cr0ack != write_val) {
        if (timeout_counter++ > dev->ack_timeout) {
            return SMMU_ERR_TIMEOUT;
        }
    }

    return SMMU_ERR_NONE;
}

enum smmu_error_t smmu_gpc_enable(struct smmu_dev_t *dev)
{
    return smmu_cr0_setbits(dev, SMMU_ROOT_CR0_GPCEN);
}

enum smmu_error_t smmu_gpc_disable(struct smmu_dev_t *dev)
{
    return smmu_cr0_clearbits(dev, SMMU_ROOT_CR0_GPCEN);
}

enum smmu_error_t smmu_access_enable(struct smmu_dev_t *dev)
{
    return smmu_cr0_setbits(dev, SMMU_ROOT_CR0_ACCESSEN);
}

enum smmu_error_t smmu_access_disable(struct smmu_dev_t *dev)
{
    return smmu_cr0_clearbits(dev, SMMU_ROOT_CR0_ACCESSEN);
}
