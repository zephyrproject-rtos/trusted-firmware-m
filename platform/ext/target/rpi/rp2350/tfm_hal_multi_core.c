/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include "tfm_hal_multi_core.h"
#include "platform_multicore.h"
#include "target_cfg.h"
#include "region_defs.h"

#include "interrupt.h"
#include "pico/multicore.h"
#include "hardware/structs/sio.h"

/* Entrypoint function declaration */
extern void ns_agent_tz_main(uint32_t c_entry);

volatile uint32_t CORE1_RUNNING;

bool multicore_ns_fifo_rvalid(void) {
    return !!(sio_ns_hw->fifo_st & SIO_FIFO_ST_VLD_BITS);
}

bool multicore_ns_fifo_wready(void) {
    return !!(sio_ns_hw->fifo_st & SIO_FIFO_ST_RDY_BITS);
}

void multicore_ns_fifo_push_blocking_inline(uint32_t data) {
    /* We wait for the fifo to have some space */
    while (!multicore_ns_fifo_wready())
        tight_loop_contents();

    sio_ns_hw->fifo_wr = data;

    /* Fire off an event to the other core */
    __sev();
}

uint32_t multicore_ns_fifo_pop_blocking_inline(void) {
    /* If nothing there yet, we wait for an event first,
       to try and avoid too much busy waiting */
    while (!multicore_ns_fifo_rvalid())
        __wfe();

    return sio_ns_hw->fifo_rd;
}

static void wait_for_core1_ready(void)
{
    uint32_t stage;
    while (1) {
        stage = multicore_fifo_pop_blocking();
        if  (stage == CORE1_S_READY) {
            break;
        }
    }
}

/* If Core0 wants to write Flash, Core1 must not use it.
 * As Core1 partly runs from Flash, it must be stopped while Core0 is writing.
 * The simplest solution is for Core0 to ring Core1's doorbell where we wait out
 * the flash operation, using spinlock. */
static void __not_in_flash_func(Core1Doorbell_Handler)() {
    uint32_t status = 0;
    /* Prevent IRQs to fire, as their handlers might be in Flash */
    __ASM volatile ("mrs %0, primask \n cpsid i" : "=r" (status) :: "memory");
    /* Check if this is the "flash-in-use" doorbell */
    if (sio_hw->doorbell_in_set & FLASH_DOORBELL_MASK)
    {
        /* Clear doorbell */
        sio_hw->doorbell_in_clr = FLASH_DOORBELL_MASK;
        /* Wait for Flash to be available, then release it immediately */
        while(!*FLASH_SPINLOCK);
        *FLASH_SPINLOCK = 0x1;
    /* Check if this is the "halt" doorbell */
    } else if (sio_hw->doorbell_in_set & HALT_DOORBELL_MASK)
    {
        /* Clear doorbell */
        sio_hw->doorbell_in_clr = HALT_DOORBELL_MASK;
        while (1) {
            __WFE();
        }
    }
    /* Restore IRQ status */
    __ASM volatile ("msr primask, %0" :: "r" (status) : "memory");
}

static void core1_entry(void)
{
    __TZ_set_STACKSEAL_S((uint32_t *)__get_MSP());
    /* Set up isolation boundaries between SPE and NSPE */
    sau_and_idau_cfg();

    NVIC_SetVector(SIO_IRQ_BELL_IRQn, (uint32_t) Core1Doorbell_Handler);
    /* Set it to highest priority */
    NVIC_SetPriority(SIO_IRQ_BELL_IRQn, 0x0);
    NVIC_EnableIRQ(SIO_IRQ_BELL_IRQn);
    __enable_irq();

    NVIC_SetTargetState(SIO_IRQ_FIFO_NS_IRQn);
    multicore_fifo_push_blocking(CORE1_S_READY);

    ns_agent_tz_main(NS_CODE_CORE1_START);
}

static void boot_s_core(void)
{
    CORE1_RUNNING = 0x1;
    multicore_launch_core1(core1_entry);
    wait_for_core1_ready();
}

void tfm_hal_boot_ns_cpu(uintptr_t start_addr)
{
    boot_s_core();
    return;
}

void tfm_hal_wait_for_ns_cpu_ready(void)
{
    uint32_t stage;
    while (1) {
        stage = multicore_ns_fifo_pop_blocking_inline();
        if  (stage == CORE1_NS_READY) {
            break;
        }
    }
    return;
}

void tfm_hal_get_secure_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr)
{
    /* Check static memory layout to get memory attributes */
    tfm_get_secure_mem_region_attr(p, s, p_attr);
#if TFM_ISOLATION_LEVEL >= 2
    p_attr->is_mpu_enabled = true;
#endif
}

void tfm_hal_get_ns_access_attr(const void *p, size_t s,
                                struct mem_attr_info_t *p_attr)
{
    /* Check static memory layout to get memory attributes */
    tfm_get_ns_mem_region_attr(p, s, p_attr);
}
