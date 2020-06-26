/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 * Copyright (c) 2017-2020 Arm Limited.
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

#include "mcuboot_config/mcuboot_config.h"
#include <assert.h>
#include "bl2_util.h"
#include "target.h"
#include "tfm_hal_device_header.h"
#include "Driver_Flash.h"
#include "mbedtls/memory_buffer_alloc.h"
#include "bootutil/bootutil_log.h"
#include "bootutil/image.h"
#include "bootutil/bootutil.h"
#include "flash_map_backend/flash_map_backend.h"
#include "boot_record.h"
#include "security_cnt.h"
#include "boot_hal.h"
#include "region.h"
#if MCUBOOT_LOG_LEVEL > MCUBOOT_LOG_LEVEL_OFF
#include "uart_stdout.h"
#endif
#if defined(CRYPTO_HW_ACCELERATOR) || \
    defined(CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING)
#include "crypto_hw.h"
#endif

/* Avoids the semihosting issue */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif

#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
REGION_DECLARE(Image$$, ARM_LIB_STACK, $$ZI$$Base);
#endif

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

#define BL2_MBEDTLS_MEM_BUF_LEN 0x2000
/* Static buffer to be used by mbedtls for memory allocation */
static uint8_t mbedtls_mem_buf[BL2_MBEDTLS_MEM_BUF_LEN];

static void do_boot(struct boot_rsp *rsp)
{
    struct boot_arm_vector_table *vt;
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
        vt = (struct boot_arm_vector_table *)(rsp->br_hdr->ih_load_addr +
                                         rsp->br_hdr->ih_hdr_size);
    } else {
        /* Using the flash address as not executing in SRAM */
        vt = (struct boot_arm_vector_table *)(flash_base +
                                         rsp->br_image_off +
                                         rsp->br_hdr->ih_hdr_size);
    }
    rc = FLASH_DEV_NAME.Uninitialize();
    if(rc != ARM_DRIVER_OK) {
        BOOT_LOG_ERR("Error while uninitializing Flash Interface");
    }

#if MCUBOOT_LOG_LEVEL > MCUBOOT_LOG_LEVEL_OFF
    stdio_uninit();
#endif
    /* This function never returns, because it calls the secure application
     * Reset_Handler()
     */
    boot_platform_quit(vt);
}

int main(void)
{
#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
    uint32_t msp_stack_bottom =
            (uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK, $$ZI$$Base);
#endif
    struct boot_rsp rsp;
    int rc;

#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__)
    __set_MSPLIM(msp_stack_bottom);
#endif

    /* Perform platform specific initialization */
    if (boot_platform_init() != 0) {
        while (1)
            ;
    }

#if MCUBOOT_LOG_LEVEL > MCUBOOT_LOG_LEVEL_OFF
    stdio_init();
#endif

    BOOT_LOG_INF("Starting bootloader");

    /* Initialise the mbedtls static memory allocator so that mbedtls allocates
     * memory from the provided static buffer instead of from the heap.
     */
    mbedtls_memory_buffer_alloc_init(mbedtls_mem_buf, BL2_MBEDTLS_MEM_BUF_LEN);

#ifdef CRYPTO_HW_ACCELERATOR
    rc = crypto_hw_accelerator_init();
    if (rc) {
        BOOT_LOG_ERR("Error while initializing cryptographic accelerator.");
        while (1);
    }
#endif /* CRYPTO_HW_ACCELERATOR */

    rc = boot_nv_security_counter_init();
    if (rc != 0) {
        BOOT_LOG_ERR("Error while initializing the security counter");
        while (1)
            ;
    }

    rc = boot_go(&rsp);
    if (rc != 0) {
        BOOT_LOG_ERR("Unable to find bootable image");
        while (1)
            ;
    }

#ifdef CRYPTO_HW_ACCELERATOR
    rc = crypto_hw_accelerator_finish();
    if (rc) {
        BOOT_LOG_ERR("Error while uninitializing cryptographic accelerator.");
        while (1);
    }
#endif /* CRYPTO_HW_ACCELERATOR */

/* This is a workaround to program the TF-M related cryptographic keys
 * to CC312 OTP memory. This functionality is independent from secure boot,
 * this is usually done in the factory floor during chip manufacturing.
 */
#ifdef CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING
    BOOT_LOG_INF("OTP provisioning started.");
    rc = crypto_hw_accelerator_otp_provisioning();
    if (rc) {
        BOOT_LOG_ERR("OTP provisioning FAILED: 0x%X", rc);
        while (1);
    } else {
        BOOT_LOG_INF("OTP provisioning succeeded. TF-M won't be loaded.");

        /* We don't need to boot - the only aim is provisioning. */
        while (1);
    }
#endif /* CRYPTO_HW_ACCELERATOR_OTP_PROVISIONING */

    BOOT_LOG_INF("Bootloader chainload address offset: 0x%x",
                 rsp.br_image_off);
    BOOT_LOG_INF("Jumping to the first image slot");
    do_boot(&rsp);

    BOOT_LOG_ERR("Never should get here");
    while (1)
        ;
}
