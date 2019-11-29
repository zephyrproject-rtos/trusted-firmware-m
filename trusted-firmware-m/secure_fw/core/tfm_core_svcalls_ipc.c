/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "tfm_api.h"
#include "tfm_arch.h"
#include "tfm_internal.h"
#include "core/tfm_core_svc.h"
#include "tfm_utils.h"
#include "tfm_svcalls.h"
#include "spm_api.h"

uint32_t tfm_core_svc_handler(uint32_t *svc_args, uint32_t exc_return)
{
    uint8_t svc_number = 0;
    /*
     * Stack contains:
     * r0, r1, r2, r3, r12, r14 (lr), the return address and xPSR
     * First argument (r0) is svc_args[0]
     */
    if (is_return_secure_stack(exc_return)) {
        /* SV called directly from secure context. Check instruction for
         * svc_number
         */
        svc_number = ((uint8_t *)svc_args[6])[-2];
    } else {
        /* Secure SV executing with NS return.
         * NS cannot directly trigger S SVC so this should not happen. This is
         * an unrecoverable error.
         */
        tfm_core_panic();
    }
    switch (svc_number) {
    case TFM_SVC_HANDLER_MODE:
        tfm_arch_clear_fp_status();
        exc_return = tfm_spm_init();
        break;
    case TFM_SVC_GET_BOOT_DATA:
        tfm_core_get_boot_data_handler(svc_args);
        break;
    default:
        svc_args[0] = SVC_Handler_IPC(svc_number, svc_args, exc_return);
        break;
    }

    return exc_return;
}

__attribute__ ((naked)) void tfm_core_handler_mode(void)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_HANDLER_MODE));
}

void tfm_access_violation_handler(void)
{
    while (1) {
        ;
    }
}
