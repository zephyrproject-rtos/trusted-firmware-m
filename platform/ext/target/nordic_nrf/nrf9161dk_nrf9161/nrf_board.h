/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef NRF_BOARD_H__
#define NRF_BOARD_H__

#include <hal/nrf_gpio.h>

#define BUTTON1_PIN          (8UL)
#define BUTTON1_ACTIVE_LEVEL (0UL)
#define BUTTON1_PULL         (NRF_GPIO_PIN_PULLUP)
#define LED1_PIN             (2UL)
#define LED1_ACTIVE_LEVEL    (1UL)

#endif // NRF_BOARD_H__
