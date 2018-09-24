/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 * Copyright (c) 2017-2018 Arm Limited.
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

#include <assert.h>
#include "bl2_util.h"
#include "target.h"
#include "cmsis.h"
#include "uart_stdout.h"
#include "Driver_Flash.h"
#include "mbedtls/memory_buffer_alloc.h"
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#include "bootutil/bootutil_log.h"
#include "bootutil/image.h"
#include "bootutil/bootutil.h"
#include "flash_map/flash_map.h"

/* Avoids the semihosting issue */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

#define BL2_MBEDTLS_MEM_BUF_LEN 0x2000
/* Static buffer to be used by mbedtls for memory allocation */
static uint8_t mbedtls_mem_buf[BL2_MBEDTLS_MEM_BUF_LEN];

struct arm_vector_table {
    uint32_t msp;
    uint32_t reset;
};

static void do_boot(struct boot_rsp *rsp)
{
    /* Clang at O0, stores variables on the stack with SP relative addressing.
     * When manually set the SP then the place of reset vector is lost.
     * Static variables are stored in 'data' or 'bss' section, change of SP has
     * no effect on them.
     */
    static struct arm_vector_table *vt;
    uintptr_t flash_base;
    int rc;

    /* The beginning of the image is the ARM vector table, containing
     * the initial stack pointer address and the reset vector
     * consecutively. Manually set the stack pointer and jump into the
     * reset vector
     */
    rc = flash_device_base(rsp->br_flash_dev_id, &flash_base);
    assert(rc == 0);

    if (rsp->br_hdr->ih_flags & IMAGE_F_RAM_LOAD) {
       /* The image has been copied to SRAM, find the vector table
        * at the load address instead of image's address in flash
        */
        vt = (struct arm_vector_table *)(rsp->br_hdr->ih_load_addr +
                                         rsp->br_hdr->ih_hdr_size);
    } else {
        /* Using the flash address as not executing in SRAM */
        vt = (struct arm_vector_table *)(flash_base +
                                         rsp->br_image_off +
                                         rsp->br_hdr->ih_hdr_size);
    }

    stdio_uninit();

    __disable_irq();
    __set_MSP(vt->msp);
    __DSB();
    __ISB();

    ((void (*)(void))vt->reset)();
}

int main(void)
{
    struct boot_rsp rsp;
    int rc;

    stdio_init();

    BOOT_LOG_INF("Starting bootloader");

    /* Initialise the mbedtls static memory allocator so that mbedtls allocates
     * memory from the provided static buffer instead of from the heap.
     */
    mbedtls_memory_buffer_alloc_init(mbedtls_mem_buf, BL2_MBEDTLS_MEM_BUF_LEN);

    /* Initialize Flash driver */
    FLASH_DEV_NAME.Initialize(NULL);

    rc = boot_go(&rsp);
    if (rc != 0) {
        BOOT_LOG_ERR("Unable to find bootable image");
        while (1)
            ;
    }

    BOOT_LOG_INF("Bootloader chainload address offset: 0x%x",
                 rsp.br_image_off);
    flash_area_warn_on_open();
    BOOT_LOG_INF("Jumping to the first image slot");
    do_boot(&rsp);

    BOOT_LOG_ERR("Never should get here");
    while (1)
        ;
}
