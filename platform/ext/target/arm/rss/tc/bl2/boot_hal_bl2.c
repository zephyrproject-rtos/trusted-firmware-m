/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_hal.h"

#include <string.h>

#ifdef PLATFORM_HAS_BOOT_DMA
#include "boot_dma.h"
#endif /* PLATFORM_HAS_BOOT_DMA */
#include "flash_layout.h"
#include "bootutil/bootutil_log.h"
#include "bootutil/bootutil.h"
#include "device_definition.h"
#include "host_base_address.h"
#include "platform_base_address.h"
#include "platform_regs.h"

#ifdef CRYPTO_HW_ACCELERATOR
#include "crypto_hw.h"
#include "fih.h"
#endif /* CRYPTO_HW_ACCELERATOR */
#include "bl2_image_id.h"
#include "Driver_Flash.h"
#include "host_flash_atu.h"
#include "sic_boot.h"

#ifdef FLASH_DEV_NAME
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;
#endif /* FLASH_DEV_NAME */

extern struct boot_rsp rsp;

int32_t boot_platform_post_init(void)
{
    enum mhu_v2_x_error_t status;
#ifdef PLATFORM_HAS_BOOT_DMA
    enum tfm_plat_err_t plat_err;
#endif /* PLATFORM_HAS_BOOT_DMA */

#ifdef CRYPTO_HW_ACCELERATOR
    int32_t result;

    result = crypto_hw_accelerator_init();
    if (result) {
        return 1;
    }

    (void)fih_delay_init();
#endif /* CRYPTO_HW_ACCELERATOR */

    status = mhu_v2_x_driver_init(&MHU_SCP_TO_RSS_DEV, MHU_REV_READ_FROM_HW);
    if (status != MHU_V_2_X_ERR_NONE) {
        BOOT_LOG_ERR("SCP->RSS MHU driver initialization failed");
        return 1;
    }
    BOOT_LOG_INF("SCP->RSS MHU driver initialized successfully");

    status = mhu_v2_x_driver_init(&MHU_RSS_TO_SCP_DEV, MHU_REV_READ_FROM_HW);
    if (status != MHU_V_2_X_ERR_NONE) {
        BOOT_LOG_ERR("RSS->SCP MHU driver initialization failed");
        return 1;
    }
    BOOT_LOG_INF("RSS->SCP MHU driver initialized successfully");

#ifdef PLATFORM_HAS_BOOT_DMA
    plat_err = boot_dma_init_cfg();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        BOOT_LOG_ERR("DMA driver initialization failed: ", plat_err);
        return 1;
    }
    BOOT_LOG_INF("DMA350 driver initialized successfully.");
#endif /* PLATFORM_HAS_BOOT_DMA */

#ifdef RSS_XIP
    result = sic_boot_init();
    if (result) {
        return result;
    }
#endif /* RSS_XIP */

    return 0;
}

int boot_platform_pre_load(uint32_t image_id)
{
    return host_flash_atu_pre_load(image_id);
}

int boot_platform_post_load(uint32_t image_id)
{
    int err;

#ifdef RSS_XIP
    err = sic_boot_post_load(image_id, rsp.br_image_off);
    if (err) {
        return err;
    }
#endif /* RSS_XIP */

    if (image_id == RSS_BL2_IMAGE_SCP) {
        uint32_t channel_stat = 0;
        struct rss_sysctrl_t *sysctrl =
                                     (struct rss_sysctrl_t *)RSS_SYSCTRL_BASE_S;

        /* Remove the image header and move the image to the start of SCP memory
         * FIXME: Would be better to set SCP VTOR, but not currently possible
         */
        memmove((void *)HOST_BOOT_IMAGE1_LOAD_BASE_S,
                (void *)(HOST_BOOT_IMAGE1_LOAD_BASE_S + 0x1000),
                SCP_BL1_MAX_SIZE - 0x1000);

        /* Release SCP CPU from wait */
        sysctrl->gretreg = 0x1;

        /* Wait for SCP to finish its startup */
        BOOT_LOG_INF("Waiting for SCP BL1 started event");
        while (channel_stat == 0) {
            mhu_v2_x_channel_receive(&MHU_SCP_TO_RSS_DEV, 0, &channel_stat);
        }
        BOOT_LOG_INF("Got SCP BL1 started event");

    } else if (image_id == RSS_BL2_IMAGE_AP) {
        BOOT_LOG_INF("Telling SCP to start AP cores");
        mhu_v2_x_initiate_transfer(&MHU_RSS_TO_SCP_DEV);
        /* Slot 0 is used in the SCP protocol */
        mhu_v2_x_channel_send(&MHU_RSS_TO_SCP_DEV, 0, 1);
    }

    err = host_flash_atu_post_load(image_id);
    if (err) {
        return err;
    }

    return 0;
}

void boot_platform_quit(struct boot_arm_vector_table *vt)
{
    /* Clang at O0, stores variables on the stack with SP relative addressing.
     * When manually set the SP then the place of reset vector is lost.
     * Static variables are stored in 'data' or 'bss' section, change of SP has
     * no effect on them.
     */
    static struct boot_arm_vector_table *vt_cpy;
    int32_t result;

    vt_cpy = vt;

#ifdef RSS_XIP
    result = sic_boot_pre_quit(&vt_cpy);
    if (result) {
        while(1){}
    }
#endif /* RSS_XIP */

#ifdef CRYPTO_HW_ACCELERATOR
    result = crypto_hw_accelerator_finish();
    if (result) {
        while(1){}
    }
#endif /* CRYPTO_HW_ACCELERATOR */

#ifdef FLASH_DEV_NAME
    result = FLASH_DEV_NAME.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1){}
    }
#endif /* FLASH_DEV_NAME */
#ifdef FLASH_DEV_NAME_2
    result = FLASH_DEV_NAME_2.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1){}
    }
#endif /* FLASH_DEV_NAME_2 */
#ifdef FLASH_DEV_NAME_3
    result = FLASH_DEV_NAME_3.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1){}
    }
#endif /* FLASH_DEV_NAME_3 */
#ifdef FLASH_DEV_NAME_SCRATCH
    result = FLASH_DEV_NAME_SCRATCH.Uninitialize();
    if (result != ARM_DRIVER_OK) {
        while(1){}
    }
#endif /* FLASH_DEV_NAME_SCRATCH */

#if defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) \
 || defined(__ARM_ARCH_8_1M_MAIN__)
    /* Restore the Main Stack Pointer Limit register's reset value
     * before passing execution to runtime firmware to make the
     * bootloader transparent to it.
     */
    __set_MSPLIM(0);
#endif /* defined(__ARM_ARCH_8M_MAIN__) || defined(__ARM_ARCH_8M_BASE__) \
       || defined(__ARM_ARCH_8_1M_MAIN__) */

    __set_MSP(vt_cpy->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}
