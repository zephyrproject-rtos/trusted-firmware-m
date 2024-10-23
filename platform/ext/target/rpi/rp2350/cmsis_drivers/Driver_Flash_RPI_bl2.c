/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include "tfm_hal_device_header.h"
#include "Driver_Flash_RPI.h"
#include "RTE_Device.h"

#if (RTE_FLASH0)

#define RP2350_FLASH_PAGE_SIZE        0x100        /* 256B */
#define RP2350_FLASH_SECTOR_SIZE      0x1000       /* 4KB */
#define RP2350_FLASH_SIZE             PICO_FLASH_SIZE_BYTES
#define RP2350_FLASH_ERASE_VALUE      0xFF

static ARM_FLASH_INFO RP2350_FLASH_DEV_DATA = {
    .sector_info    = NULL,     /* Uniform sector layout */
    .sector_count   = RP2350_FLASH_SIZE/ RP2350_FLASH_SECTOR_SIZE,
    .sector_size    = RP2350_FLASH_SECTOR_SIZE,
    .page_size      = RP2350_FLASH_PAGE_SIZE,
    .program_unit   = RP2350_FLASH_PAGE_SIZE, /* page aligned, page multipled */
    .erased_value   = RP2350_FLASH_ERASE_VALUE
};

static uint32_t irq_state = 0;

static inline uint32_t __save_disable_irq(void)
{
    uint32_t result = 0;

    __ASM volatile ("mrs %0, primask \n cpsid i" : "=r" (result) :: "memory");
    return result;
}

static inline void __restore_irq(uint32_t status)
{
    __ASM volatile ("msr primask, %0" :: "r" (status) : "memory");
}

/* No need to save and restore MPU configuration for bl2 */
static void dummy_save(struct rp2350_flash_dev_t* flash_dev){
    ARG_UNUSED(flash_dev);
    irq_state = __save_disable_irq();
}
static void dummy_restore(void){
    __restore_irq(irq_state);
}

static rp2350_flash_dev_t RP2350_FLASH_DEV = {
    .data = &RP2350_FLASH_DEV_DATA,
    .base = XIP_BASE,
    .size = RP2350_FLASH_SIZE,
    .save_mpu_state = dummy_save,
    .restore_mpu_state = dummy_restore
};


RPI_RP2350_FLASH(RP2350_FLASH_DEV, RP2350_FLASH);
#endif /* RTE_FLASH0 */
