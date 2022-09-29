/*
 * Copyright (c) 2019-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "boot_hal.h"

#include <string.h>

#include "bootutil/bootutil_log.h"
#include "device_definition.h"
#include "host_base_address.h"
#include "platform_regs.h"
#include "platform_base_address.h"
#ifdef CRYPTO_HW_ACCELERATOR
#include "crypto_hw.h"
#include "fih.h"
#endif /* CRYPTO_HW_ACCELERATOR */

int32_t boot_platform_post_init(void)
{
    enum mhu_v2_x_error_t status;
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

    return 0;
}

int boot_platform_pre_load(uint32_t image_id)
{
    enum atu_error_t err;

    if (image_id == 3) {
        /* Initialize SCP ATU region */
        err = atu_initialize_region(&ATU_DEV_S, 0, HOST_BOOT1_LOAD_BASE_S,
                                    SCP_BL1_SRAM_BASE, SCP_BL1_SIZE);
        if (err != ATU_ERR_NONE) {
            return 1;
        }

    } else if (image_id == 2) {
        /* Initialize AP ATU region */
        err = atu_initialize_region(&ATU_DEV_S, 0, HOST_BOOT0_LOAD_BASE_S,
                                    AP_BL1_SRAM_BASE, AP_BL1_SIZE);
        if (err != ATU_ERR_NONE) {
            return 1;
        }
    }

    return 0;
}

int boot_platform_post_load(uint32_t image_id)
{
    enum atu_error_t err;

    /* FIXME: provide enum for image_ids */
    if (image_id == 3) {
        uint32_t channel_stat = 0;
        struct rss_sysctrl_t *sysctrl =
                                     (struct rss_sysctrl_t *)RSS_SYSCTRL_BASE_S;

        /* Remove the image header and move the image to the start of SCP memory
         * FIXME: Would be better to set SCP VTOR, but not currently possible
         */
        memmove((void *)HOST_BOOT1_LOAD_BASE_S,
                (void *)(HOST_BOOT1_LOAD_BASE_S + BL2_HEADER_SIZE),
                SCP_BL1_SIZE - BL2_HEADER_SIZE);

        /* Release SCP CPU from wait */
        sysctrl->gretreg = 0x1;

        /* Close SCP ATU region */
        err = atu_uninitialize_region(&ATU_DEV_S, 0);
        if (err != ATU_ERR_NONE) {
            return 1;
        }

        /* Wait for SCP to finish its startup */
        BOOT_LOG_INF("Waiting for SCP BL1 started event");
        while (channel_stat == 0) {
            mhu_v2_x_channel_receive(&MHU_SCP_TO_RSS_DEV, 0, &channel_stat);
        }
        BOOT_LOG_INF("Got SCP BL1 started event");

    } else if (image_id == 2) {
        /* Close AP ATU region */
        err = atu_uninitialize_region(&ATU_DEV_S, 0);
        if (err != ATU_ERR_NONE) {
            return 1;
        }

        BOOT_LOG_INF("Telling SCP to start AP cores");
        mhu_v2_x_initiate_transfer(&MHU_RSS_TO_SCP_DEV);
        /* Slot 0 is used in the SCP protocol */
        mhu_v2_x_channel_send(&MHU_RSS_TO_SCP_DEV, 0, 1);
    }

    return 0;
}
