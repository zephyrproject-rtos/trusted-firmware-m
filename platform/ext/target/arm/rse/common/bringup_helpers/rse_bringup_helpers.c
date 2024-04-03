/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "rse_bringup_helpers.h"

#include "rse_bringup_helpers_hal.h"
#include "platform_base_address.h"
#include "boot_hal.h"
#include "flash_layout.h"
#include "device_definition.h"
#include "gpio_pl061_drv.h"

#include <stdbool.h>
#include <stdint.h>

static uint8_t check_gpio_pins(void)
{
    uint32_t pins_offset = rse_bringup_helpers_hal_get_pins_offset();

    return (pl061_get_gpio(GPIO0_DEV_S, UINT8_MAX) >> pins_offset) & 0b11;
}

static uint16_t get_gppc_val(void)
{
    struct lcm_otp_layout_t *lcm = ((struct lcm_otp_layout_t *)(LCM_BASE_S
                                                                + LCM_OTP_OFFSET));
    return (lcm->cm_config_2 >> 8) & 0xFFFF;
}

static void boot_address(struct boot_arm_vector_table *vt)
{
    /* Clang at O0, stores variables on the stack with SP relative addressing.
     * When manually set the SP th)en the place of reset vector is lost.
     * Static variables are stored in 'data' or 'bss' section, change of SP has
     * no effect on them.
     */
    static struct boot_arm_vector_table *vt_cpy;

    vt_cpy = vt;

    /* Restore the Main Stack Pointer Limit register's reset value
     * before passing execution to runtime firmware to make the
     * bootloader transparent to it.
     */
    __set_MSPLIM(0);

    __set_MSP(vt_cpy->msp);
    __DSB();
    __ISB();

    boot_jump_to_next_image(vt_cpy->reset);
}

void rse_run_bringup_helpers_if_requested(void)
{
    uint8_t gpio_pins_value = check_gpio_pins();
    uint8_t gppc_val = get_gppc_val();

    switch (gppc_val & 0b11) {
        case 0:
            break;
        case 1:
            boot_address(rse_bringup_helpers_hal_get_vm0_exec_address());
            break;
        case 2:
            boot_address(rse_bringup_helpers_hal_get_qpsi_exec_address());
            break;
        case 3:
            boot_address(rse_bringup_helpers_hal_get_side_band_exec_address());
            break;
    }

    switch (gpio_pins_value) {
        case 0:
            break;
        case 1:
            boot_address(rse_bringup_helpers_hal_get_vm0_exec_address());
            break;
        case 2:
            boot_address(rse_bringup_helpers_hal_get_qpsi_exec_address());
            break;
        case 3:
            boot_address(rse_bringup_helpers_hal_get_side_band_exec_address());
            break;
    }
}
