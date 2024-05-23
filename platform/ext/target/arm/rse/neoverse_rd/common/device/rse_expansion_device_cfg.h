/*
 * Copyright (c) 2023-2024 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __RSE_EXPANSION_DEVICE_CFG_H__
#define __RSE_EXPANSION_DEVICE_CFG_H__

/**
 * \file  rse_expansion_device_cfg.h
 * \brief This is the device configuration file with only used peripherals
 *        defined and configured via the secure and/or non-secure base address.
 */

/* ARM UART PL011 */
#define DEFAULT_UART_CONTROL 0
#define DEFAULT_UART_BAUDRATE 38400
#define UART0_PL011_S

/* Intel Strata Flash Device */
#define SPI_STRATAFLASHJ3_S

/* CFI Controller */
#define CFI_S

#if RSE_AMOUNT > 1
/* One MHU for each other RSE in system */
#define MHU_RSE_TO_RSE_SIDEBAND_RECEIVER_COUNT (RSE_AMOUNT - 1)
#define MHU_RSE_TO_RSE_SIDEBAND_SENDER_COUNT (RSE_AMOUNT - 1)
#endif

#endif  /* __RSE_EXPANSION_DEVICE_CFG_H__ */
