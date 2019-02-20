/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLAT_TEST_H__
#define __TFM_PLAT_TEST_H__

#include "tfm_plat_defs.h"

/**
 * \brief Busy wait until the user presses a specific button
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_CORE_TEST")
void tfm_plat_test_wait_user_button_pressed(void);

/**
 * \brief Busy wait until the user releases a specific button
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_CORE_TEST")
void tfm_plat_test_wait_user_button_released(void);

/**
 * \brief Get the status of the LEDs used by tests
 *
 * \return Returns the current status of LEDs
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_CORE_TEST")
uint32_t tfm_plat_test_get_led_status(void);

/**
 * \brief Sets the status of the LEDs used by tests
 *
 * \param[in]  status  The status to be set
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_CORE_TEST")
void tfm_plat_test_set_led_status(uint32_t status);

/**
 * \brief Get the mask of the LEDs used for testing
 *
 * \return Returns the mask of the LEDs used for testing
 */
TFM_LINK_SET_RO_IN_PARTITION_SECTION("TFM_SP_CORE_TEST")
uint32_t tfm_plat_test_get_userled_mask(void);

#endif /* __TFM_PLAT_TEST_H__ */
