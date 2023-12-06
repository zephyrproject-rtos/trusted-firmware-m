/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H

// <e> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART0]
// <i> Configuration settings for Driver_USART0 in component ::Drivers:USART
#define   RTE_USART0                    1
//   <h> Pin Selection (0xFFFFFFFF means Disconnected)
//     <o> TXD
#define   RTE_USART0_TXD_PIN            27
//     <o> RXD
#define   RTE_USART0_RXD_PIN            26
//     <o> RTS
#define   RTE_USART0_RTS_PIN            14
//     <o> CTS
#define   RTE_USART0_CTS_PIN            15
//   </h> Pin Configuration
// </e> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART0]

// <e> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART1]
// <i> Configuration settings for Driver_USART1 in component ::Drivers:USART
#define   RTE_USART1                    1
//   <h> Pin Selection (0xFFFFFFFF means Disconnected)
//     <o> TXD
#define   RTE_USART1_TXD_PIN            29
//     <o> RXD
#define   RTE_USART1_RXD_PIN            28
//     <o> RTS
#define   RTE_USART1_RTS_PIN            16
//     <o> CTS
#define   RTE_USART1_CTS_PIN            17
//   </h> Pin Configuration
// </e> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART1]

// <e> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART2]
// <i> Configuration settings for Driver_USART2 in component ::Drivers:USART
#define   RTE_USART2                    0
//   <h> Pin Selection (0xFFFFFFFF means Disconnected)
//     <o> TXD
#define   RTE_USART2_TXD_PIN            0xFFFFFFFF
//     <o> RXD
#define   RTE_USART2_RXD_PIN            0xFFFFFFFF
//     <o> RTS
#define   RTE_USART2_RTS_PIN            0xFFFFFFFF
//     <o> CTS
#define   RTE_USART2_CTS_PIN            0xFFFFFFFF
//   </h> Pin Configuration
// </e> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART2]

// <e> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART3]
// <i> Configuration settings for Driver_USART3 in component ::Drivers:USART
#define   RTE_USART3                    0
//   <h> Pin Selection (0xFFFFFFFF means Disconnected)
//     <o> TXD
#define   RTE_USART3_TXD_PIN            0xFFFFFFFF
//     <o> RXD
#define   RTE_USART3_RXD_PIN            0xFFFFFFFF
//     <o> RTS
#define   RTE_USART3_RTS_PIN            0xFFFFFFFF
//     <o> CTS
#define   RTE_USART3_RTS_PIN            0xFFFFFFFF
//   </h> Pin Configuration
// </e> USART (Universal synchronous - asynchronous receiver transmitter) [Driver_USART3]

// <e> FLASH (Flash Memory) [Driver_FLASH0]
// <i> Configuration settings for Driver_FLASH0 in component ::Drivers:FLASH
#define   RTE_FLASH0                    1
// </e> FLASH (Flash Memory) [Driver_FLASH0]

#endif  /* __RTE_DEVICE_H */
