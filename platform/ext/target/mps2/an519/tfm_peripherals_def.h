/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PERIPHERALS_DEF_H__
#define __TFM_PERIPHERALS_DEF_H__

struct tfm_spm_partition_platform_data_t;

extern struct tfm_spm_partition_platform_data_t tfm_peripheral_uart1;
extern struct tfm_spm_partition_platform_data_t tfm_peripheral_fpga_io;

#define TFM_PERIPHERAL_FPGA_IO   (&tfm_peripheral_fpga_io)
#define TFM_PERIPHERAL_UART1     (&tfm_peripheral_uart1)

#endif /* __TFM_PERIPHERALS_DEF_H__ */
