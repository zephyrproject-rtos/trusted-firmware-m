/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file host_base_address.h
 * \brief This file defines the host memory map addresses accessed by RSS.
 */

#ifndef __HOST_BASE_ADDRESS_H__
#define __HOST_BASE_ADDRESS_H__

#include "platform_base_address.h"
#include "size_defs.h"

/*
 * Location of AP SRAM.
 */
#define HOST_AP_SHARED_SRAM_PHYS_BASE 0x0
#define HOST_AP_SHARED_SRAM_PHYS_SIZE 0x100000
#define HOST_AP_SHARED_SRAM_PHYS_LIMIT \
    (HOST_AP_SHARED_SRAM_PHYS_BASE + HOST_AP_SHARED_SRAM_PHYS_SIZE-1)

#endif  /* __HOST_BASE_ADDRESS_H__ */
