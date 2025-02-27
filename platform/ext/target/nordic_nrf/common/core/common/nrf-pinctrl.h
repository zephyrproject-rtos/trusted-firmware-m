/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef NRF_INCLUDE_NRF_PINCTRL_H
#define NRF_INCLUDE_NRF_PINCTRL_H

/*
 * The whole nRF pin configuration information is encoded in a 32-bit bitfield
 * organized as follows:
 *
 * - 31..16: Pin function.
 * - 15:     Reserved.
 * - 14:     Pin inversion mode.
 * - 13:     Pin low power mode.
 * - 12..9:  Pin output drive configuration.
 * - 8..7:   Pin pull configuration.
 * - 6..0:   Pin number (combination of port and pin).
 */

/* NOTE: Keep in sync with Zephyr's nrf-pinctrl.h */

/**
 * @name nRF pin configuration bit field positions and masks.
 * @{
 */

/** Position of the function field. */
#define NRF_FUN_POS 17U
/** Mask for the function field. */
#define NRF_FUN_MSK 0x7FFFU
/** Position of the pin field. */
#define NRF_PIN_POS 0U
/** Mask for the pin field. */
#define NRF_PIN_MSK 0x1FFU

/** @} */

/**
 * @name nRF pinctrl pin functions.
 * @{
 */

/** UART TX */
#define NRF_FUN_UART_TX 0U
/** UART RX */
#define NRF_FUN_UART_RX 1U
/** UART RTS */
#define NRF_FUN_UART_RTS 2U
/** UART CTS */
#define NRF_FUN_UART_CTS 3U

/** Indicates that a pin is disconnected */
#define NRF_PIN_DISCONNECTED NRF_PIN_MSK

/** @} */

/**
 * @brief Utility macro to build nRF psels property entry.
 *
 * @param fun Pin function configuration (see NRF_FUNC_{name} macros).
 * @param port Port (0 or 1).
 * @param pin Pin (0..31).
 */
#define NRF_PSEL(fun, port, pin)						       \
	((((((port) * 32U) + (pin)) & NRF_PIN_MSK) << NRF_PIN_POS) |		       \
	 ((NRF_FUN_ ## fun & NRF_FUN_MSK) << NRF_FUN_POS))

/**
 * @brief Utility macro to build nRF psels property entry when a pin is disconnected.
 *
 * This can be useful in situations where code running before Zephyr, e.g. a bootloader
 * configures pins that later needs to be disconnected.
 *
 * @param fun Pin function configuration (see NRF_FUN_{name} macros).
 */
#define NRF_PSEL_DISCONNECTED(fun)						       \
	(NRF_PIN_DISCONNECTED |							       \
	 ((NRF_FUN_ ## fun & NRF_FUN_MSK) << NRF_FUN_POS))

/**
 * @brief Utility macro to obtain pin function.
 *
 * @param pincfg Pin configuration bit field.
 */
#define NRF_GET_FUN(pincfg) (((pincfg) >> NRF_FUN_POS) & NRF_FUN_MSK)


/**
 * @brief Utility macro to obtain port and pin combination.
 *
 * @param pincfg Pin configuration bit field.
 */
#define NRF_GET_PIN(pincfg) (((pincfg) >> NRF_PIN_POS) & NRF_PIN_MSK)

#endif /* NRF_INCLUDE_NRF_PINCTRL_H */
