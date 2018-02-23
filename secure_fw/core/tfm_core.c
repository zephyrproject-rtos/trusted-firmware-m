/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "tfm_core.h"
#include "target_cfg.h"
#include "uart_stdout.h"
#include "secure_utilities.h"
#include "secure_fw/spm/spm_api.h"

#ifdef TEST_FRAMEWORK_S
#include "test/framework/integ_test.h"
#endif

/*
 * Avoids the semihosting issue
 * FixMe: describe 'semihosting issue'
 */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif

#ifndef TFM_LVL
#error TFM_LVL is not defined!
#endif
#if (TFM_LVL != 1) && (TFM_LVL != 3)
#error Only TFM_LVL 1 and 3 are supported!
#endif

/* Macros to pick linker symbols and allow to form the service data base */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)

REGION_DECLARE(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Limit);

#define SPNIDEN_SEL_STATUS (0x01u << 7)
#define SPNIDEN_STATUS     (0x01u << 6)
#define SPIDEN_SEL_STATUS  (0x01u << 5)
#define SPIDEN_STATUS      (0x01u << 4)
#define NIDEN_SEL_STATUS   (0x01u << 3)
#define NIDEN_STATUS       (0x01u << 2)
#define DBGEN_SEL_STATUS   (0x01u << 1)
#define DBGEN_STATUS       (0x01u << 0)

#define All_SEL_STATUS (SPNIDEN_SEL_STATUS | SPIDEN_SEL_STATUS | \
                        NIDEN_SEL_STATUS | DBGEN_SEL_STATUS)

void configure_debug_registers(void)
{
    volatile struct sysctrl_t *sys_ctrl =
        (struct sysctrl_t *)CMSDK_SYSCTRL_BASE_S;

#if defined(DAUTH_NONE)
    /* Set all the debug enable selector bits to 1 */
    sys_ctrl->secdbgset = All_SEL_STATUS;
    /* Set all the debug enable bits to 0 */
    sys_ctrl->secdbgclr =
        DBGEN_STATUS | NIDEN_STATUS | SPIDEN_STATUS | SPNIDEN_STATUS;
#elif defined(DAUTH_NS_ONLY)
    /* Set all the debug enable selector bits to 1 */
    sys_ctrl->secdbgset = All_SEL_STATUS;
    /* Set the debug enable bits to 1 for NS, and 0 for S mode */
    sys_ctrl->secdbgset = DBGEN_STATUS | NIDEN_STATUS;
    sys_ctrl->secdbgclr = SPIDEN_STATUS | SPNIDEN_STATUS;
#elif defined(DAUTH_FULL)
    /* Set all the debug enable selector bits to 1 */
    sys_ctrl->secdbgset = All_SEL_STATUS;
    /* Set all the debug enable bits to 1 */
    sys_ctrl->secdbgset =
        DBGEN_STATUS | NIDEN_STATUS | SPIDEN_STATUS | SPNIDEN_STATUS;
#else
#if !defined(DAUTH_CHIP_DEFAULT)
#error "No debug authentication setting is provided."
#endif
    /* Set all the debug enable selector bits to 0 */
    sys_ctrl->secdbgclr = All_SEL_STATUS;
    /* No need to set any enable bits because the value depends on
     *input signals
     */
#endif
}

int32_t tfm_core_init(void)
{
    /* Enables fault handlers */
    enable_fault_handlers();

    /* Configure the debug configuration registers */
    configure_debug_registers();

    __enable_irq();

#ifdef TFM_CORE_DEBUG
    /* Initializes UART0 to print messages. The STDOUT is redirected to UART0 */
    uart_init(UART0_CHANNEL);

    LOG_MSG("Secure image initializing!");
    printf("TFM level is: %d\r\n", TFM_LVL);
#else
    /* FixMe: redirect secure log entries to secure log */
    uart_init(UART0_CHANNEL);

    LOG_MSG("Secure image initializing!");
#endif

    /* Configures non-secure memory spaces in the target */
    sau_and_idau_cfg();
    mpc_init_cfg();
    ppc_init_cfg();
    configure_ns_code();

    /* Configures all interrupts to retarget NS state, except for
     * secure peripherals
     */
    nvic_interrupt_target_state_cfg();
    /* Enable secure peripherals interrupts */
    nvic_interrupt_enable();

    tfm_scratch_area =
        (uint8_t *)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
    tfm_scratch_area_size =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Limit) -
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
    return 0;
}

int main(void)
{
    tfm_core_init();

    tfm_spm_db_init();
#if TFM_LVL != 1
    tfm_spm_mpu_init();
#endif
    if (tfm_spm_service_init() != SPM_ERR_OK) {
        /* Certain systems might refuse to boot altogether if services fail
         * to initialize. This is a placeholder for such an error handler
         */
    }

    tfm_spm_service_set_state(TFM_SEC_FUNC_CORE_ID, SPM_PART_STATE_RUNNING);

    extern uint32_t Stack_Mem[];

    __set_PSPLIM((uint32_t)Stack_Mem);

#ifdef TEST_FRAMEWORK_S
    start_integ_test();
#endif

#ifdef TFM_CORE_DEBUG
    /* Jumps to non-secure code */
    LOG_MSG("Jumping to non-secure code...");
#endif

#if TFM_LVL != 1
    /* FixMe: service MPU regions need to be overloaded for snippet of
     * code jumping to NS to work in unprivileged thread mode
     */
#ifdef UNPRIV_JUMP_TO_NS
    /* Initialization is done, set thread mode to unprivileged. */
    CONTROL_Type ctrl;

    ctrl.w = __get_CONTROL();
    ctrl.b.nPRIV = 1;
    __set_CONTROL(ctrl.w);
    __DSB();
    __ISB();
#endif
#endif

    /* We close the TFM_SEC_FUNC_CORE_ID service, because its only purpose is
     * to be able to pass the state checks for the tests started from secure.
     */
    tfm_spm_service_set_state(TFM_SEC_FUNC_CORE_ID, SPM_PART_STATE_CLOSED);
    tfm_spm_service_set_state(TFM_SEC_FUNC_NON_SECURE_ID,
                              SPM_PART_STATE_RUNNING);

    jump_to_ns_code();
}
