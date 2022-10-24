/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_BASE_H__
#define __CONFIG_BASE_H__

/* Platform Partition Configs */

/* Size of input buffer in platform service */
#define PLATFORM_SERVICE_INPUT_BUFFER_SIZE     64

/* Size of output buffer in platform service */
#define PLATFORM_SERVICE_OUTPUT_BUFFER_SIZE    64

/* The stack size of the Platform Secure Partition */
#define PLATFORM_SP_STACK_SIZE                 0x500

/* Disable Non-volatile counter module */
#define PLATFORM_NV_COUNTER_MODULE_DISABLED    0

#endif /* __CONFIG_BASE_H__ */
