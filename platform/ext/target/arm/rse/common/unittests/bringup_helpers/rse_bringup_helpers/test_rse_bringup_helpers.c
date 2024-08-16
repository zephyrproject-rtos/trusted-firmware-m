/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdlib.h>
#include <string.h>

#include "rse_bringup_helpers.h"

#include "mock_rse_bringup_helpers_hal.h"
#include "mock_gpio_pl061_drv.h"
#include "mock_lcm_drv.h"
#include "mock_boot_hal.h"

#include "unity.h"

static pl061_regblk_t pl061_regblk;
pl061_regblk_t *const GPIO0_DEV_S = (pl061_regblk_t *) &pl061_regblk;

static struct lcm_dev_cfg_t lcm_dev_cfg;
struct lcm_dev_t LCM_DEV_S = {
    .cfg = (struct lcm_dev_cfg_t *) &lcm_dev_cfg
};

struct boot_arm_vector_table vector_tables[3] = {
    {0x00000000, VM0_VECTOR_TABLE_RESET},  /* VM0 Vector Table */
    {0x00000000, QSPI_VECTOR_TABLE_RESET}, /* QSPI Vector Table */
    {0x00000000, SBAND_VECTOR_TABLE_RESET} /* Side Band Vector Table */
};

TEST_CASE(0, 0, DEFAULT_VECTOR_TABLE_RESET, DEFAULT_VECTOR_TABLE_RESET)
TEST_CASE(0, 1, DEFAULT_VECTOR_TABLE_RESET, VM0_VECTOR_TABLE_RESET)
TEST_CASE(0, 2, DEFAULT_VECTOR_TABLE_RESET, QSPI_VECTOR_TABLE_RESET)
TEST_CASE(0, 3, DEFAULT_VECTOR_TABLE_RESET, SBAND_VECTOR_TABLE_RESET)
TEST_CASE(1, 0, VM0_VECTOR_TABLE_RESET, DEFAULT_VECTOR_TABLE_RESET)
TEST_CASE(1, 1, VM0_VECTOR_TABLE_RESET, VM0_VECTOR_TABLE_RESET)
TEST_CASE(1, 2, VM0_VECTOR_TABLE_RESET, QSPI_VECTOR_TABLE_RESET)
TEST_CASE(1, 3, VM0_VECTOR_TABLE_RESET, SBAND_VECTOR_TABLE_RESET)
TEST_CASE(2, 0, QSPI_VECTOR_TABLE_RESET, DEFAULT_VECTOR_TABLE_RESET)
TEST_CASE(2, 1, QSPI_VECTOR_TABLE_RESET, VM0_VECTOR_TABLE_RESET)
TEST_CASE(2, 2, QSPI_VECTOR_TABLE_RESET, QSPI_VECTOR_TABLE_RESET)
TEST_CASE(2, 3, QSPI_VECTOR_TABLE_RESET, SBAND_VECTOR_TABLE_RESET)
TEST_CASE(3, 0, SBAND_VECTOR_TABLE_RESET, DEFAULT_VECTOR_TABLE_RESET)
TEST_CASE(3, 1, SBAND_VECTOR_TABLE_RESET, VM0_VECTOR_TABLE_RESET)
TEST_CASE(3, 2, SBAND_VECTOR_TABLE_RESET, QSPI_VECTOR_TABLE_RESET)
TEST_CASE(3, 3, SBAND_VECTOR_TABLE_RESET, SBAND_VECTOR_TABLE_RESET)
void test_rse_run_bringup_helpers_if_requested(uint32_t gppc_val,
                                               uint8_t gpio_pins_value,
                                               uintptr_t vector_table_gppc,
                                               uintptr_t vector_table_gpio)
{
    uint32_t pins_offset = 0;

    /* Prepare */
    rse_bringup_helpers_hal_get_vm0_exec_address_IgnoreAndReturn(&vector_tables[0]);
    rse_bringup_helpers_hal_get_qpsi_exec_address_IgnoreAndReturn(&vector_tables[1]);
    rse_bringup_helpers_hal_get_side_band_exec_address_IgnoreAndReturn(&vector_tables[2]);

    lcm_get_gppc_Expect(&LCM_DEV_S, NULL);
    lcm_get_gppc_IgnoreArg_gppc();
    lcm_get_gppc_ReturnMemThruPtr_gppc((uint32_t *)&gppc_val, sizeof(gppc_val));

    rse_bringup_helpers_hal_get_pins_offset_IgnoreAndReturn(pins_offset);
    GPIO0_DEV_S->gpiodata[UINT8_MAX] = gpio_pins_value;

    if(gppc_val) {
        boot_jump_to_next_image_Expect(vector_table_gppc);
    }

    if(gpio_pins_value) {
        boot_jump_to_next_image_Expect(vector_table_gpio);
    }

    /* Act */
    rse_run_bringup_helpers_if_requested();
}
