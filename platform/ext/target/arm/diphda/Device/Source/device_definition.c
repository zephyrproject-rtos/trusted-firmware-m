/*
 * Copyright (c) 2016-2021, Arm Limited. All rights reserved.
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

#include "device_definition.h"
#include "platform_base_address.h"

#ifdef UART0_PL011
static const struct uart_pl011_dev_cfg_t UART0_PL011_DEV_CFG = {
    .base = DIPHDA_UART_BASE,
    .def_baudrate = DEFAULT_UART_BAUDRATE,
    .def_wlen = UART_PL011_WLEN_8,
    .def_parity = UART_PL011_PARITY_DISABLED,
    .def_stopbit = UART_PL011_STOPBIT_1};
static struct uart_pl011_dev_data_t UART0_PL011_DEV_DATA = {
    .state = UART_PL011_UNINITIALIZED,
    .uart_clk = 0,
    .baudrate = 0};
struct uart_pl011_dev_t UART0_PL011_DEV = {&(UART0_PL011_DEV_CFG),
                                           &(UART0_PL011_DEV_DATA)};
#endif

#ifdef MHU1_SE_TO_HOST
struct mhu_v2_x_dev_t MHU1_SE_TO_HOST_DEV = {(DIPHDA_SEH_1_SENDER_BASE),
                                             (MHU_V2_X_SENDER_FRAME)};
#endif

#ifdef MHU1_HOST_TO_SE
struct mhu_v2_x_dev_t MHU1_HOST_TO_SE_DEV = {(DIPHDA_HSE_1_RECEIVER_BASE),
                                             (MHU_V2_X_RECEIVER_FRAME)};
#endif
