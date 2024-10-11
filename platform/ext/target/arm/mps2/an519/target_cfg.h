/*
 * Copyright (c) 2017-2024 Arm Limited
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

#ifndef __TARGET_CFG_H__
#define __TARGET_CFG_H__

#define TFM_DRIVER_STDIO    Driver_USART0
#define NS_DRIVER_STDIO     Driver_USART0

/**
 * \brief Defines the indexes of Subordinate Peripheral Protection Controller
 *        registers.
 */
typedef enum
{
    PPC_SP_DO_NOT_CONFIGURE = -1,
    PPC_SP_AHB_PPC0 = 0,
    PPC_SP_RES0,
    PPC_SP_RES1,
    PPC_SP_RES2,
    PPC_SP_AHB_PPC_EXP0,
    PPC_SP_AHB_PPC_EXP1,
    PPC_SP_AHB_PPC_EXP2,
    PPC_SP_AHB_PPC_EXP3,
    PPC_SP_APB_PPC0,
    PPC_SP_APB_PPC1,
    PPC_SP_RES3,
    PPC_SP_RES4,
    PPC_SP_APB_PPC_EXP0,
    PPC_SP_APB_PPC_EXP1,
    PPC_SP_APB_PPC_EXP2,
    PPC_SP_APB_PPC_EXP3,
} ppc_bank_t;

#endif /* __TARGET_CFG_H__ */
