/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cc3xx_dma.h"

#include <string.h>

#include "cc3xx_reg_defs.h"
#include "cc3xx_engine_state.h"

static cc3xx_dma_remap_region_t remap_regions[CC3XX_DMA_REMAP_REGION_AM] = {0};

static uint32_t remap_cpusel = 0;

cc3xx_err_t cc3xx_dma_remap_region_init(uint32_t remap_region_idx,
                                        cc3xx_dma_remap_region_t *region)
{
    if (remap_region_idx >= CC3XX_DMA_REMAP_REGION_AM) {
        return CC3XX_ERR_GENERIC_ERROR;
    }

    memcpy(&remap_regions[remap_region_idx], region, sizeof(*region));

    return CC3XX_ERR_SUCCESS;
}

cc3xx_err_t cc3xx_dma_remap_region_clear(uint32_t remap_region_idx)
{
    if (remap_region_idx >= CC3XX_DMA_REMAP_REGION_AM) {
        return CC3XX_ERR_GENERIC_ERROR;
    }

    memset(&remap_regions[remap_region_idx], 0, sizeof(cc3xx_dma_remap_region_t));

    return CC3XX_ERR_SUCCESS;
}

cc3xx_err_t cc3xx_dma_tcm_cpusel(uint32_t cpuid)
{
    remap_cpusel = cpuid;
}

static uintptr_t remap_addr(uintptr_t addr)
{
    uint32_t idx;
    cc3xx_dma_remap_region_t *region;

    for (idx = 0; idx < CC3XX_DMA_REMAP_REGION_AM; idx++) {
        region = &remap_regions[idx];
        if (addr >= region->region_base
            && addr < region->region_base + region->region_size) {
            return (addr - region->region_base) + region->remap_base
                    + (region->remap_cpusel_offset * remap_cpusel);
        }
    }

    return addr;
}

cc3xx_err_t cc3xx_dma_copy_data(void* dest, const void* src, size_t length)
{
    cc3xx_err_t err;

    if (cc3xx_engine_in_use) {
        /* Since the copy operation isn't restartable, just check that the
         * engine isn't in use when we begin.
         */
        return CC3XX_ERR_ENGINE_IN_USE;
    }

    /* Enable the DMA clock */
    *CC3XX_REG_MISC_DMA_CLK_ENABLE = 0x1U;

    /* Wait for the crypto engine to be ready */
    while (*CC3XX_REG_CC_CTL_CRYPTO_BUSY) {}

    /* Set the crypto engine to the PASSTHROUGH engine */
    *CC3XX_REG_CC_CTL_CRYPTO_CTL = 0b00000U;

    /* Set output target */
    err = cc3xx_dma_set_output(dest, length);
    if (err != CC3XX_ERR_SUCCESS) {
        goto out;
    }

    /* This starts the copy */
    err = cc3xx_dma_input_data(src, length);

out:
    /* Disable the DMA clock */
    *CC3XX_REG_MISC_DMA_CLK_ENABLE = 0x0U;

    return err;
}

cc3xx_err_t cc3xx_dma_input_data(const void* buf, size_t length)
{
    uintptr_t remapped_buf;

    /* There's a max size of data the DMA can deal with */
    if (length >= 0x10000) {
        return CC3XX_ERR_INVALID_DATA;
    }

    /* Mask a sensible set of the host interrupts */
    *CC3XX_REG_HOST_RGF_HOST_RGF_IMR = 0xFF0U;

    /* Reset the AXI_ERROR and SYM_DMA_COMPLETED interrupts */
    *CC3XX_REG_HOST_RGF_HOST_RGF_ICR |= 0x900U;

    /* remap the address, particularly for TCMs */
    remapped_buf = remap_addr((uintptr_t)buf);

    /* Set the data source */
    *CC3XX_REG_DIN_SRC_LLI_WORD0 = remapped_buf;
    /* Writing the length triggers the DMA */
    *CC3XX_REG_DIN_SRC_LLI_WORD1 = length;

    /* Wait for the DMA to complete (The SYM_DMA_COMPLETED interrupt to be
     * asserted)
     */
    while (!(*CC3XX_REG_HOST_RGF_HOST_RGF_IRR & 0x800U)){
        if (*CC3XX_REG_HOST_RGF_HOST_RGF_IRR & 0x100U) {
            return CC3XX_ERR_GENERIC_ERROR;
        }
    }

    /* Reset the SYM_DMA_COMPLETED interrupt */
    *CC3XX_REG_HOST_RGF_HOST_RGF_ICR = 0x800U;

    return CC3XX_ERR_SUCCESS;
}

cc3xx_err_t cc3xx_dma_set_output(void* buf, size_t length)
{
    uintptr_t remapped_buf;

    /* remap the address, particularly for TCMs */
    remapped_buf = remap_addr((uintptr_t)buf);

    /* Set the data target */
    *CC3XX_REG_DOUT_DST_LLI_WORD0 = remapped_buf;
    /* And the length */
    *CC3XX_REG_DOUT_DST_LLI_WORD1 = length;

    return CC3XX_ERR_SUCCESS;
}
