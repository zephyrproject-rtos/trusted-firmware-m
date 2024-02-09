/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sam_drv.h"
#include "sam_reg_map.h"
#include "tfm_hal_device_header.h"

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

/* Index in the sames, samecl, samem or samim register arrays for event_id */
#define SAMEx_IDX(event_id) ((uint32_t)(event_id) >> 0x5UL)

/* Mask for event_id in the sames, samecl, samem or samim register */
#define SAMEx_MASK(event_id) (0x1UL << ((uint32_t)(event_id) & 0x1FUL))

/* Index in the samrrls register array of response for event_id */
#define SAMRRLS_IDX(event_id) ((uint32_t)(event_id) >> 0x3UL)

/* Offset within the samrrls register of response for event_id */
#define SAMRRLS_OFF(event_id) (((uint32_t)(event_id) & 0x7UL) << 0x2UL)

/* Mask for response in the samrrls register for event_id */
#define SAMRRLS_MASK(event_id) (0xFUL << SAMRRLS_OFF(event_id))

static uint32_t zero_count(uint32_t val)
{
    uint32_t res = 32;

    while (val != 0) {
        val &= val - 1;
        res--;
    }

    return res;
}

static uint32_t log2(uint32_t val)
{
    uint32_t res = 0;

    while (val >>= 1) {
        res++;
    }

    return res;
}

enum sam_error_t sam_init(const struct sam_dev_t *dev)
{
    struct sam_reg_map_t *regs = (struct sam_reg_map_t *)dev->cfg->base;
    volatile uint32_t *sam_cfg_base = regs->samem;

    /* Write the default config */
    for (size_t i = 0; i < SAM_CONFIG_LEN; i++) {
        sam_cfg_base[i] = dev->cfg->default_config[i];
    }

    return SAM_ERROR_NONE;
}

enum sam_error_t sam_enable_event(const struct sam_dev_t *dev,
                                  enum sam_event_id_t event_id)
{
    struct sam_reg_map_t *regs = (struct sam_reg_map_t *)dev->cfg->base;

    if (event_id > SAM_MAX_EVENT_ID) {
        return SAM_ERROR_INVALID_ARGUMENT;
    }

    if (!(regs->samim[SAMEx_IDX(event_id)] & SAMEx_MASK(event_id))) {
        regs->samim[SAMEx_IDX(event_id)] |= SAMEx_MASK(event_id);
        /* Update integrity check value, one lower zero count */
        regs->samicv--;
    }

    return SAM_ERROR_NONE;
}

enum sam_error_t sam_disable_event(const struct sam_dev_t *dev,
                                   enum sam_event_id_t event_id)
{
    struct sam_reg_map_t *regs = (struct sam_reg_map_t *)dev->cfg->base;

    if (event_id > SAM_MAX_EVENT_ID) {
        return SAM_ERROR_INVALID_ARGUMENT;
    }

    if (regs->samim[SAMEx_IDX(event_id)] & SAMEx_MASK(event_id)) {
        regs->samim[SAMEx_IDX(event_id)] &= ~SAMEx_MASK(event_id);
        /* Update integrity check value, one higher zero count */
        regs->samicv++;
    }

    return SAM_ERROR_NONE;
}

enum sam_error_t sam_set_event_response(const struct sam_dev_t *dev,
                                        enum sam_event_id_t event_id,
                                        enum sam_response_t response,
                                        bool enable_response)
{
    struct sam_reg_map_t *regs = (struct sam_reg_map_t *)dev->cfg->base;
    uint32_t old_reg_val;
    uint32_t new_reg_val;
    uint32_t rrl_val;

    if (event_id > SAM_MAX_EVENT_ID || response > SAM_MAX_RESPONSE_ACTION ||
        response == SAM_RESPONSE_NONE) {
        return SAM_ERROR_INVALID_ARGUMENT;
    }

    old_reg_val = regs->samrrls[SAMRRLS_IDX(event_id)];

    rrl_val = ((uint32_t)enable_response << 3UL) | log2((uint32_t)response);

    new_reg_val = (old_reg_val & ~SAMRRLS_MASK(event_id)) |
                  ((rrl_val << SAMRRLS_OFF(event_id)) & SAMRRLS_MASK(event_id));

    regs->samrrls[SAMRRLS_IDX(event_id)] = new_reg_val;

    /* Update integrity check value with the difference in zero count */
    regs->samicv += zero_count(new_reg_val) - zero_count(old_reg_val);

    return SAM_ERROR_NONE;
}

void sam_set_watchdog_counter_initial_value(const struct sam_dev_t *dev,
                                            uint32_t count_value,
                                            enum sam_response_t responses)
{
    struct sam_reg_map_t *regs = (struct sam_reg_map_t *)dev->cfg->base;
    uint32_t prev_zero_count = zero_count(regs->samwdciv);

    uint32_t wdciv_val = (count_value & 0x3FFFFFFUL) |
                         ((((uint32_t)responses >> 2UL) & 0x3FUL) << 26UL);

    regs->samwdciv = wdciv_val;

    /* Update integrity check value with the difference in zero count */
    regs->samicv += zero_count(wdciv_val) - prev_zero_count;
}

enum sam_error_t sam_register_event_handler(struct sam_dev_t *dev,
                                            enum sam_event_id_t event_id,
                                            sam_event_handler_t event_handler)
{
    if (event_id > SAM_MAX_EVENT_ID) {
        return SAM_ERROR_INVALID_ARGUMENT;
    }

    dev->event_handlers[event_id] = event_handler;

    return SAM_ERROR_NONE;
}

void sam_handle_event(const struct sam_dev_t *dev)
{
    struct sam_reg_map_t *regs = (struct sam_reg_map_t *)dev->cfg->base;
    uint32_t reg_idx;
    uint32_t event_id;
    uint32_t mask;
    uint32_t sames_val;
    uint32_t samecl_val;

    /* Iterate over each bit position in each of the SAMES registers to check if
     * the corresponding event ID is pending.
     */
    for (reg_idx = 0; reg_idx < ARRAY_LEN(regs->sames); reg_idx++) {
        sames_val = regs->sames[reg_idx];
        samecl_val = 0;

        /* Skip ECC events, which have their own separate interrupt handlers */
        if (reg_idx == SAMEx_IDX(SAM_EVENT_SRAM_PARTIAL_WRITE)) {
            sames_val &= ~(SAMEx_MASK(SAM_EVENT_SRAM_PARTIAL_WRITE) |
                           SAMEx_MASK(SAM_EVENT_VM0_SINGLE_ECC_ERROR) |
                           SAMEx_MASK(SAM_EVENT_VM1_SINGLE_ECC_ERROR) |
                           SAMEx_MASK(SAM_EVENT_VM2_SINGLE_ECC_ERROR) |
                           SAMEx_MASK(SAM_EVENT_VM3_SINGLE_ECC_ERROR));
        }

        /* Check each bit position until all pending event have been handled
         * (when the clear value equals the status value). In most cases there
         * will only be one pending event.
         */
        for (event_id = reg_idx << 5UL, mask = 1;
             event_id <= SAM_MAX_EVENT_ID && samecl_val != sames_val;
             event_id++, mask <<= 1) {
            if (sames_val & mask) {
                if (dev->event_handlers[event_id]) {
                    dev->event_handlers[event_id]();
                }
                samecl_val |= mask;
            }
        }

        if (samecl_val != 0) {
            regs->samecl[reg_idx] = samecl_val;
        }
    }
}

void sam_handle_partial_write(const struct sam_dev_t *dev)
{
    struct sam_reg_map_t *regs = (struct sam_reg_map_t *)dev->cfg->base;
    volatile uint64_t *vm_ptr;
    size_t i;

    /* Handle any partial writes by reading & writing-back the affected memory
     * address.
     */
    for (i = 0; i < ARRAY_LEN(regs->vmpwca); i++) {
        vm_ptr = (volatile uint64_t *)regs->vmpwca[i];
        if (vm_ptr) {
            *vm_ptr = *vm_ptr;
            regs->vmpwca[i] = 0;
        }
    }

    /* Clear partial write error event */
    regs->samecl[SAMEx_IDX(SAM_EVENT_SRAM_PARTIAL_WRITE)] =
        SAMEx_MASK(SAM_EVENT_SRAM_PARTIAL_WRITE);
}

void sam_handle_single_ecc_error(const struct sam_dev_t *dev)
{
    struct sam_reg_map_t *regs = (struct sam_reg_map_t *)dev->cfg->base;
    volatile uint64_t *vm_ptr;
    size_t i;

    /* Handle any single ECC error events by reading & writing-back the affected
     * memory address.
     */
    for (i = 0; i < ARRAY_LEN(regs->vmsceeca); i++) {
        if (regs->sames[0] & SAMEx_MASK(SAM_EVENT_VM0_SINGLE_ECC_ERROR + i)) {
            vm_ptr = (volatile uint64_t *)regs->vmsceeca[i];
            if (vm_ptr) {
                *vm_ptr = *vm_ptr;
                regs->vmsceeca[i] = 0;
            }
        }
    }

    /* Clear single ECC error events */
    regs->samecl[SAMEx_IDX(SAM_EVENT_VM0_SINGLE_ECC_ERROR)] =
        SAMEx_MASK(SAM_EVENT_VM0_SINGLE_ECC_ERROR) |
        SAMEx_MASK(SAM_EVENT_VM1_SINGLE_ECC_ERROR) |
        SAMEx_MASK(SAM_EVENT_VM2_SINGLE_ECC_ERROR) |
        SAMEx_MASK(SAM_EVENT_VM3_SINGLE_ECC_ERROR);
}
