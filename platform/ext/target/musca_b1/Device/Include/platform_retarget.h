/*
 * Copyright (c) 2017-2018 Arm Limited
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
 * \file platform_retarget.h
 * \brief This file defines all the peripheral base addresses for the Musca-B1
 *        platform.
 */

#ifndef __MUSCA_B1_RETARGET_H__
#define __MUSCA_B1_RETARGET_H__

#include "platform_regs.h"           /* Platform registers */
#include "system_cmsdk_musca.h"

/* SRAM MPC ranges and limits */
/* Internal memory */
#define MPC_ISRAM0_RANGE_BASE_NS     0x20000000
#define MPC_ISRAM0_RANGE_LIMIT_NS    0x2001FFFF
#define MPC_ISRAM0_RANGE_BASE_S      0x30000000
#define MPC_ISRAM0_RANGE_LIMIT_S     0x3001FFFF

#define MPC_ISRAM1_RANGE_BASE_NS     0x20020000
#define MPC_ISRAM1_RANGE_LIMIT_NS    0x2003FFFF
#define MPC_ISRAM1_RANGE_BASE_S      0x30020000
#define MPC_ISRAM1_RANGE_LIMIT_S     0x3003FFFF

#define MPC_ISRAM2_RANGE_BASE_NS     0x20040000
#define MPC_ISRAM2_RANGE_LIMIT_NS    0x2005FFFF
#define MPC_ISRAM2_RANGE_BASE_S      0x30040000
#define MPC_ISRAM2_RANGE_LIMIT_S     0x3005FFFF

#define MPC_ISRAM3_RANGE_BASE_NS     0x20060000
#define MPC_ISRAM3_RANGE_LIMIT_NS    0x2007FFFF
#define MPC_ISRAM3_RANGE_BASE_S      0x30060000
#define MPC_ISRAM3_RANGE_LIMIT_S     0x3007FFFF

/* Code SRAM memory */
#define MPC_CODE_SRAM_RANGE_BASE_NS  0x0A400000
#define MPC_CODE_SRAM_RANGE_LIMIT_NS 0x0A47FFFF
#define MPC_CODE_SRAM_RANGE_BASE_S   0x1A400000
#define MPC_CODE_SRAM_RANGE_LIMIT_S  0x1A47FFFF

/* QSPI Flash memory */
#define MPC_QSPI_RANGE_BASE_NS       0x00000000
#define MPC_QSPI_RANGE_LIMIT_NS      0x01FFFFFF
#define MPC_QSPI_RANGE_BASE_S        0x10000000
#define MPC_QSPI_RANGE_LIMIT_S       0x11FFFFFF

#endif  /* __MUSCA_B1_RETARGET_H__ */
