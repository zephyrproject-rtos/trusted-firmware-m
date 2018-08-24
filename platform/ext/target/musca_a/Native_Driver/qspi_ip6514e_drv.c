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

#include <stdint.h>
#include "qspi_ip6514e_drv.h"

struct _qspi_ip6514e_reg_t {
    volatile uint32_t qspi_cfg;                           /*!< 0x00 (R/W) */
    volatile uint32_t hidden1[8];
    volatile uint32_t remap_addr;                         /*!< 0x24 (R/W) */
};

#define  QSPI_CONTROLLER ((volatile struct _qspi_ip6514e_reg_t *) \
         QSPI_CONTROLLER_ADDRESS)

void qspi_remap_base(uint32_t new_addr)
{
    __DSB();
    __ISB();
    QSPI_CONTROLLER->qspi_cfg &= ~1U;
    QSPI_CONTROLLER->remap_addr = new_addr;
    QSPI_CONTROLLER->qspi_cfg |= (1U << 16);
    QSPI_CONTROLLER->qspi_cfg |= 1U;
    __DSB();
    __ISB();
}