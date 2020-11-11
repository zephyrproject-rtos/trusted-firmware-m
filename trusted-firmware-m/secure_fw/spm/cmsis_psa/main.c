/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "common/tfm_boot_data.h"
#include "region.h"
#include "spm_ipc.h"
#include "tfm_hal_platform.h"
#include "tfm_hal_isolation.h"
#include "tfm_irq_list.h"
#include "tfm_nspm.h"
#include "tfm_spm_hal.h"
#include "tfm_spm_log.h"
#include "tfm_version.h"

/*
 * Avoids the semihosting issue
 * FixMe: describe 'semihosting issue'
 */
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
__asm("  .global __ARM_use_no_argv\n");
#endif

#ifndef TFM_LVL
#error TFM_LVL is not defined!
#elif (TFM_LVL != 1) && (TFM_LVL != 2) && (TFM_LVL != 3)
#error Invalid TFM_LVL value. Only TFM_LVL 1, 2 and 3 are supported in IPC model!
#endif

REGION_DECLARE(Image$$, ARM_LIB_STACK_MSP,  $$ZI$$Base);

static int32_t tfm_core_init(void)
{
    size_t i;
    enum tfm_hal_status_t hal_status = TFM_HAL_ERROR_GENERIC;
    enum tfm_plat_err_t plat_err = TFM_PLAT_ERR_SYSTEM_ERR;
    enum irq_target_state_t irq_target_state = TFM_IRQ_TARGET_STATE_SECURE;

    /* Enables fault handlers */
    plat_err = tfm_spm_hal_enable_fault_handlers();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_ERROR_GENERIC;
    }

    /* Configures the system reset request properties */
    plat_err = tfm_spm_hal_system_reset_cfg();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_ERROR_GENERIC;
    }

    /* Configures debug authentication */
    plat_err = tfm_spm_hal_init_debug();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_ERROR_GENERIC;
    }

    /*
     * Access to any peripheral should be performed after programming
     * the necessary security components such as PPC/SAU.
     */
    hal_status = tfm_hal_set_up_static_boundaries();
    if (hal_status != TFM_HAL_SUCCESS) {
        return TFM_ERROR_GENERIC;
    }

    /* Performs platform specific initialization */
    hal_status = tfm_hal_platform_init();
    if (hal_status != TFM_HAL_SUCCESS) {
        return TFM_ERROR_GENERIC;
    }

    /* Configures architecture-specific coprocessors */
    tfm_arch_configure_coprocessors();

    SPMLOG_INFMSG("\033[1;34m[Sec Thread] Secure image initializing!\033[0m\r\n");

    SPMLOG_DBGMSGVAL("TF-M isolation level is: ", TFM_LVL);

    tfm_core_validate_boot_data();

    configure_ns_code();

    /* Configures all interrupts to retarget NS state, except for
     * secure peripherals
     */
    plat_err = tfm_spm_hal_nvic_interrupt_target_state_cfg();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_ERROR_GENERIC;
    }

    for (i = 0; i < tfm_core_irq_signals_count; ++i) {
        plat_err = tfm_spm_hal_set_secure_irq_priority(
                                          tfm_core_irq_signals[i].irq_line,
                                          tfm_core_irq_signals[i].irq_priority);
        if (plat_err != TFM_PLAT_ERR_SUCCESS) {
            return TFM_ERROR_GENERIC;
        }
        irq_target_state = tfm_spm_hal_set_irq_target_state(
                                          tfm_core_irq_signals[i].irq_line,
                                          TFM_IRQ_TARGET_STATE_SECURE);
        if (irq_target_state != TFM_IRQ_TARGET_STATE_SECURE) {
            return TFM_ERROR_GENERIC;
        }
    }

    /* Enable secure peripherals interrupts */
    plat_err = tfm_spm_hal_nvic_interrupt_enable();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_ERROR_GENERIC;
    }

    return TFM_SUCCESS;
}

int main(void)
{
    /* set Main Stack Pointer limit */
    tfm_arch_init_secure_msp((uint32_t)&REGION_NAME(Image$$, ARM_LIB_STACK_MSP,
                                               $$ZI$$Base));

    if (tfm_core_init() != TFM_SUCCESS) {
        tfm_core_panic();
    }
    /* Print the TF-M version */
    SPMLOG_INFMSG("\033[1;34mBooting TFM v"VERSION_FULLSTR"\033[0m\r\n");

    if (tfm_spm_db_init() != SPM_ERR_OK) {
        tfm_core_panic();
    }

    /*
     * Prioritise secure exceptions to avoid NS being able to pre-empt
     * secure SVC or SecureFault. Do it before PSA API initialization.
     */
    tfm_arch_set_secure_exception_priorities();

    /* Move to handler mode for further SPM initialization. */
    tfm_core_handler_mode();
}
