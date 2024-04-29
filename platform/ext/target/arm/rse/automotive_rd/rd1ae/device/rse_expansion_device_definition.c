/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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

/**
 * \file  rse_expansion_device_definition.c
 * \brief This file defines exports the structures based on the peripheral
 *        definitions from device_cfg.h.
 */

#include "rse_expansion_device_definition.h"
#include "platform_base_address.h"

#ifdef RSE_USE_LOCAL_UART
/* Arm UART PL011 driver structures */
static const struct uart_pl011_dev_cfg_t UART0_PL011_DEV_CFG_S = {
    .base = LOCAL_UART0_BASE_S,
    .def_baudrate = DEFAULT_UART_BAUDRATE,
    .def_wlen = UART_PL011_WLEN_8,
    .def_parity = UART_PL011_PARITY_DISABLED,
    .def_stopbit = UART_PL011_STOPBIT_1
};
static struct uart_pl011_dev_data_t UART0_PL011_DEV_DATA_S = {
    .state = 0,
    .uart_clk = 0,
    .baudrate = 0
};
struct uart_pl011_dev_t UART0_PL011_DEV_S = {
    &(UART0_PL011_DEV_CFG_S),
    &(UART0_PL011_DEV_DATA_S)
};
#endif /* RSE_USE_LOCAL_UART */

static const struct cfi_dev_cfg_t CFI_DEV_CFG_S = {
    .base = BOOT_FLASH_BASE_S,
};
struct cfi_dev_t CFI_DEV_S = {
    .cfg = &CFI_DEV_CFG_S,
};

struct cfi_strataflashj3_dev_t SPI_STRATAFLASHJ3_DEV = {
    .controller = &CFI_DEV_S,
    .total_sector_cnt = 0,
    .page_size = 0,
    .sector_size = 0,
    .program_unit = 0,
    .is_initialized = false
};

/* Message Handling Units (MHU) */
struct mhu_v3_x_dev_t MHU_AP_MONITOR_TO_RSE_DEV = {
    .base = MHU0_RECEIVER_BASE_S,
    .frame = MHU_V3_X_MBX_FRAME,
    .subversion = 0
};

struct mhu_v3_x_dev_t MHU_RSE_TO_AP_MONITOR_DEV = {
    .base = MHU0_SENDER_BASE_S,
    .frame = MHU_V3_X_PBX_FRAME,
    .subversion = 0
};

struct mhu_v3_x_dev_t MHU_V3_SCP_TO_RSE_DEV = {
    .base = MHU4_RECEIVER_BASE_S,
    .frame = MHU_V3_X_MBX_FRAME,
    .subversion = 0
};

struct mhu_v3_x_dev_t MHU_V3_RSE_TO_SCP_DEV = {
    .base = MHU4_SENDER_BASE_S,
    .frame = MHU_V3_X_PBX_FRAME,
    .subversion = 0
};
