/*
 * Copyright (c) 2018 ARM Limited
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

#ifndef __QSPI_IP6514E_DRV_H__
#define __QSPI_IP6514E_DRV_H__

#include "platform_retarget.h"

#define SPI_FLASH_ACCESS_LENGTH       (0x40000)

void qspi_remap_base(uint32_t new_addr);

#endif /* __QSPI_IP6514E_DRV_H__ */