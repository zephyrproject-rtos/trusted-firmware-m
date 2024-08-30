/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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

#include "rse_clocks.h"
#include "config_tfm.h"
#include "tfm_hal_device_header.h"

int rse_clock_config(void)
{
    struct rse_sysctrl_t *sysctrl = (struct rse_sysctrl_t *)RSE_SYSCTRL_BASE_S;

    uint32_t sysclkstatus = (sysctrl->clk_cfg1 & CLK_CFG1_SYSCLKCFGSTATUS_MASK)
                            >> CLK_CFG1_SYSCLKCFGSTATUS_POS;
    uint32_t sysclkcfg = (sysctrl->clk_cfg1 & CLK_CFG1_SYSCLKCFG_MASK)
                         >> CLK_CFG1_SYSCLKCFG_POS;

    /* Check that the targeted CLKCFGSTATUS field matches its associated CLKCFG
     * value before the write occurs.
     */
    if (sysclkstatus != sysclkcfg) {
        return -1;
    }

    /* Configure SYSCLK. CPU0CLK is tied to SYSCLK. */
    sysctrl->clk_cfg1 = (SYSCLKCFG_VAL << CLK_CFG1_SYSCLKCFG_POS)
                        & CLK_CFG1_SYSCLKCFG_MASK;

    /* After writing to each CLKCFG field, poll the associated CLKCFGSTATUS
     * field until CLKCFGSTATUS is the same as CLKCFG before doing any other
     * operations.
     */
    do {
        sysclkstatus = (sysctrl->clk_cfg1 & CLK_CFG1_SYSCLKCFGSTATUS_MASK)
                       >> CLK_CFG1_SYSCLKCFGSTATUS_POS;
        sysclkcfg = (sysctrl->clk_cfg1 & CLK_CFG1_SYSCLKCFG_MASK)
                    >> CLK_CFG1_SYSCLKCFG_POS;
    } while (sysclkstatus != sysclkcfg);

    return 0;
}

uint32_t rse_get_sysclk(void)
{
    /* Secure SysTick calibration value. CPU0CFGSSTCALIB[23:0] TENMS: Integer
     * value to compute a 10ms (100Hz) delay from the CPU0CLK.
     */
    uint32_t calib = SysTick->CALIB & SysTick_CALIB_TENMS_Msk;

    /* Calculate CPU0CLK as 100 * calibration value if available, otherwise
     * use statically-defined SYSCLK value. CPU0CLK is tied to SYSCLK.
     */
    return (calib == 0) ? SYSCLK : (100 * calib);
}
