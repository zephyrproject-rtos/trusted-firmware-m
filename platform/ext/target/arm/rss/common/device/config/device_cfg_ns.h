/*
 * Copyright (c) 2019-2023 Arm Limited. All rights reserved.
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

/* This file is installed to NS build and renamed to device_cfg.h */

#ifndef __DEVICE_CFG_H__
#define __DEVICE_CFG_H__

/**
 * \file device_cfg.h
 * \brief
 * This is the device configuration file with only used peripherals
 * defined and configured via the secure and/or non-secure base address.
 */

#include "host_device_cfg.h"
#ifdef RSS_HAS_EXPANSION_PERIPHERALS
#include "rss_expansion_device_cfg.h"
#endif /* RSS_HAS_EXPANSION_PERIPHERALS */

#ifdef RSS_DEBUG_UART
/* ARM UART CMSDK */
#define DEFAULT_UART_CONTROL 0
#define DEFAULT_UART_BAUDRATE  115200
#define UART0_CMSDK_NS
#endif /* RSS_DEBUG_UART */

#endif  /* __DEVICE_CFG_H__ */
