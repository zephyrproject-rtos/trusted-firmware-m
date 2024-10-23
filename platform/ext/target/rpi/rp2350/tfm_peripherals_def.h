/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#ifndef __TFM_PERIPHERALS_DEF_H__
#define __TFM_PERIPHERALS_DEF_H__

#include "hardware/irq.h"
#include "common_target_cfg.h"
#include "tfm_hal_device_header.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Quantized default IRQ priority, the value is:
 * (Number of configurable priority) / 2: (1UL << __NVIC_PRIO_BITS) / 4
 */

#define DEFAULT_IRQ_PRIORITY     (1UL << (__NVIC_PRIO_BITS - 2))

#define TFM_TIMER0_IRQ           (TIMER0_IRQ_0_IRQn)
#define MAILBOX_IRQ               SIO_IRQ_FIFO_NS_IRQn

#define FF_TEST_UART_IRQ         (UART0_IRQ)
extern void FF_TEST_UART_IRQ_Handler(void);

extern struct platform_data_t tfm_peripheral_std_uart;
extern struct platform_data_t tfm_peripheral_timer0;

#define TFM_PERIPHERAL_STD_UART  (&tfm_peripheral_std_uart)
#define TFM_PERIPHERAL_TIMER0    (&tfm_peripheral_timer0)

#ifdef PSA_API_TEST_IPC
extern struct platform_data_t tfm_peripheral_FF_TEST_UART_REGION;
extern struct platform_data_t tfm_peripheral_FF_TEST_WATCHDOG_REGION;
extern struct platform_data_t tfm_peripheral_FF_TEST_NVMEM_REGION;
extern struct platform_data_t tfm_peripheral_FF_TEST_SERVER_PARTITION_MMIO;
extern struct platform_data_t tfm_peripheral_FF_TEST_DRIVER_PARTITION_MMIO;
#define FF_TEST_UART_REGION           (&tfm_peripheral_FF_TEST_UART_REGION)
#define FF_TEST_WATCHDOG_REGION       (&tfm_peripheral_FF_TEST_WATCHDOG_REGION)
#define FF_TEST_NVMEM_REGION          (&tfm_peripheral_FF_TEST_NVMEM_REGION)
#define FF_TEST_SERVER_PARTITION_MMIO (&tfm_peripheral_FF_TEST_SERVER_PARTITION_MMIO)
#define FF_TEST_DRIVER_PARTITION_MMIO (&tfm_peripheral_FF_TEST_DRIVER_PARTITION_MMIO)
#endif /* PSA_API_TEST_IPC */

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PERIPHERALS_DEF_H__ */
