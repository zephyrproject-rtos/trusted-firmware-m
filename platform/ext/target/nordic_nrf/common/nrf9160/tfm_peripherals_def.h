/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 * Copyright (c) 2020, Nordic Semiconductor ASA. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PERIPHERALS_DEF_H__
#define __TFM_PERIPHERALS_DEF_H__

#include <nrf.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TFM_TIMER0_IRQ    (TIMER0_IRQn)
#define TFM_TIMER1_IRQ    (TIMER1_IRQn)

struct platform_data_t;

extern struct platform_data_t tfm_peripheral_regulators;
extern struct platform_data_t tfm_peripheral_clock;
extern struct platform_data_t tfm_peripheral_power;
extern struct platform_data_t tfm_peripheral_spim0;
extern struct platform_data_t tfm_peripheral_spis0;
extern struct platform_data_t tfm_peripheral_twim0;
extern struct platform_data_t tfm_peripheral_twis0;
extern struct platform_data_t tfm_peripheral_uarte0;
extern struct platform_data_t tfm_peripheral_spim1;
extern struct platform_data_t tfm_peripheral_spis1;
extern struct platform_data_t tfm_peripheral_twim1;
extern struct platform_data_t tfm_peripheral_twis1;
extern struct platform_data_t tfm_peripheral_uarte1;
extern struct platform_data_t tfm_peripheral_spim2;
extern struct platform_data_t tfm_peripheral_spis2;
extern struct platform_data_t tfm_peripheral_twim2;
extern struct platform_data_t tfm_peripheral_twis2;
extern struct platform_data_t tfm_peripheral_uarte2;
extern struct platform_data_t tfm_peripheral_spim3;
extern struct platform_data_t tfm_peripheral_spis3;
extern struct platform_data_t tfm_peripheral_twim3;
extern struct platform_data_t tfm_peripheral_twis3;
extern struct platform_data_t tfm_peripheral_uarte3;
extern struct platform_data_t tfm_peripheral_saadc;
extern struct platform_data_t tfm_peripheral_timer0;
extern struct platform_data_t tfm_peripheral_timer1;
extern struct platform_data_t tfm_peripheral_timer2;
extern struct platform_data_t tfm_peripheral_rtc0;
extern struct platform_data_t tfm_peripheral_rtc1;
extern struct platform_data_t tfm_peripheral_dppic;
extern struct platform_data_t tfm_peripheral_wdt;
extern struct platform_data_t tfm_peripheral_egu0;
extern struct platform_data_t tfm_peripheral_egu1;
extern struct platform_data_t tfm_peripheral_egu2;
extern struct platform_data_t tfm_peripheral_egu3;
extern struct platform_data_t tfm_peripheral_egu4;
extern struct platform_data_t tfm_peripheral_egu5;
extern struct platform_data_t tfm_peripheral_pwm0;
extern struct platform_data_t tfm_peripheral_pwm1;
extern struct platform_data_t tfm_peripheral_pwm2;
extern struct platform_data_t tfm_peripheral_pwm3;
extern struct platform_data_t tfm_peripheral_pdm;
extern struct platform_data_t tfm_peripheral_i2s;
extern struct platform_data_t tfm_peripheral_ipc;
extern struct platform_data_t tfm_peripheral_fpu;
extern struct platform_data_t tfm_peripheral_nvmc;
extern struct platform_data_t tfm_peripheral_vmc;
extern struct platform_data_t tfm_peripheral_p0;


#define TFM_PERIPHERAL_REGULATORS   (&tfm_peripheral_regulators)
#define TFM_PERIPHERAL_CLOCK        (&tfm_peripheral_clock)
#define TFM_PERIPHERAL_POWER        (&tfm_peripheral_power)
#define TFM_PERIPHERAL_SPIM0        (&tfm_peripheral_spim0)
#define TFM_PERIPHERAL_SPIS0        (&tfm_peripheral_spis0)
#define TFM_PERIPHERAL_TWIM0        (&tfm_peripheral_twim0)
#define TFM_PERIPHERAL_TWIS0        (&tfm_peripheral_twis0)
#define TFM_PERIPHERAL_UARTE0       (&tfm_peripheral_uarte0)
#define TFM_PERIPHERAL_SPIM1        (&tfm_peripheral_spim1)
#define TFM_PERIPHERAL_SPIS1        (&tfm_peripheral_spis1)
#define TFM_PERIPHERAL_TWIM1        (&tfm_peripheral_twim1)
#define TFM_PERIPHERAL_TWIS1        (&tfm_peripheral_twis1)
#define TFM_PERIPHERAL_UARTE1       (&tfm_peripheral_uarte1)
#define TFM_PERIPHERAL_SPIM2        (&tfm_peripheral_spim2)
#define TFM_PERIPHERAL_SPIS2        (&tfm_peripheral_spis2)
#define TFM_PERIPHERAL_TWIM2        (&tfm_peripheral_twim2)
#define TFM_PERIPHERAL_TWIS2        (&tfm_peripheral_twis2)
#define TFM_PERIPHERAL_UARTE2       (&tfm_peripheral_uarte2)
#define TFM_PERIPHERAL_SPIM3        (&tfm_peripheral_spim3)
#define TFM_PERIPHERAL_SPIS3        (&tfm_peripheral_spis3)
#define TFM_PERIPHERAL_TWIM3        (&tfm_peripheral_twim3)
#define TFM_PERIPHERAL_TWIS3        (&tfm_peripheral_twis3)
#define TFM_PERIPHERAL_UARTE3       (&tfm_peripheral_uarte3)
#define TFM_PERIPHERAL_SAADC        (&tfm_peripheral_saadc)
#define TFM_PERIPHERAL_TIMER0       (&tfm_peripheral_timer0)
#define TFM_PERIPHERAL_TIMER1       (&tfm_peripheral_timer1)
#define TFM_PERIPHERAL_TIMER2       (&tfm_peripheral_timer2)
#define TFM_PERIPHERAL_RTC0         (&tfm_peripheral_rtc0)
#define TFM_PERIPHERAL_RTC1         (&tfm_peripheral_rtc1)
#define TFM_PERIPHERAL_DPPIC        (&tfm_peripheral_dppic)
#define TFM_PERIPHERAL_WDT          (&tfm_peripheral_wdt)
#define TFM_PERIPHERAL_EGU0         (&tfm_peripheral_egu0)
#define TFM_PERIPHERAL_EGU1         (&tfm_peripheral_egu1)
#define TFM_PERIPHERAL_EGU2         (&tfm_peripheral_egu2)
#define TFM_PERIPHERAL_EGU3         (&tfm_peripheral_egu3)
#define TFM_PERIPHERAL_EGU4         (&tfm_peripheral_egu4)
#define TFM_PERIPHERAL_EGU5         (&tfm_peripheral_egu5)
#define TFM_PERIPHERAL_PWM0         (&tfm_peripheral_pwm0)
#define TFM_PERIPHERAL_PWM1         (&tfm_peripheral_pwm1)
#define TFM_PERIPHERAL_PWM2         (&tfm_peripheral_pwm2)
#define TFM_PERIPHERAL_PWM3         (&tfm_peripheral_pwm3)
#define TFM_PERIPHERAL_PDM          (&tfm_peripheral_pdm)
#define TFM_PERIPHERAL_I2S          (&tfm_peripheral_i2s)
#define TFM_PERIPHERAL_IPC          (&tfm_peripheral_ipc)
#define TFM_PERIPHERAL_FPU          (&tfm_peripheral_fpu)
#define TFM_PERIPHERAL_NVMC         (&tfm_peripheral_nvmc)
#define TFM_PERIPHERAL_VMC          (&tfm_peripheral_vmc)
#define TFM_PERIPHERAL_P0           (&tfm_peripheral_p0)

#define TFM_PERIPHERAL_STD_UART     TFM_PERIPHERAL_UARTE1

#ifdef PSA_API_TEST_IPC
#define FF_TEST_UART_IRQ         (EGU5_IRQn)
#define FF_TEST_UART_IRQ_Handler (pal_interrupt_handler)

extern struct platform_data_t tfm_peripheral_FF_TEST_NVMEM_REGION;
extern struct platform_data_t tfm_peripheral_FF_TEST_SERVER_PARTITION_MMIO;
extern struct platform_data_t tfm_peripheral_FF_TEST_DRIVER_PARTITION_MMIO;

#define FF_TEST_UART_REGION           (&tfm_peripheral_std_uart)
#define FF_TEST_WATCHDOG_REGION       (&tfm_peripheral_timer0)
#define FF_TEST_NVMEM_REGION          (&tfm_peripheral_FF_TEST_NVMEM_REGION)
#define FF_TEST_SERVER_PARTITION_MMIO (&tfm_peripheral_FF_TEST_SERVER_PARTITION_MMIO)
#define FF_TEST_DRIVER_PARTITION_MMIO (&tfm_peripheral_FF_TEST_DRIVER_PARTITION_MMIO)
#endif /* PSA_API_TEST_IPC */


#ifdef __cplusplus
}
#endif

#endif /* __TFM_PERIPHERALS_DEF_H__ */
