/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include "Driver_USART_RPI.h"
#include "RTE_Device.h"


#if (defined (RTE_USART0) && (RTE_USART0 == 1))
#define UART_TX_PIN 0
#define UART_RX_PIN 1

ARM_DRIVER_USART_RP2350((uart_inst_t *)UART0_BASE, driver_usart0, UART_RX_PIN, UART_TX_PIN);
#endif /* RTE_USART0 */
