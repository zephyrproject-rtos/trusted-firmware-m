/*
 * Copyright (c) 2021-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2021-2023 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "async.h"
#include "config_tfm.h"
#include "ns_agent_mailbox_signal_utils.h"
#include "psa/service.h"
#include "psa_manifest/ns_agent_mailbox.h"
#include "tfm_hal_mailbox.h"
#include "tfm_hal_multi_core.h"
#include "tfm_hal_platform.h"
#include "tfm_multi_core.h"
#include "tfm_rpc.h"
#include "tfm_sp_log.h"

static void boot_ns_core(void)
{
    /* Boot up non-secure core */
    LOG_DBGFMT("Enabling non-secure core...\r\n");

    tfm_hal_boot_ns_cpu(tfm_hal_get_ns_VTOR());
    tfm_hal_wait_for_ns_cpu_ready();
}

void ns_agent_mailbox_entry(void)
{
    psa_signal_t signals = 0;

    boot_ns_core();

    if (tfm_inter_core_comm_init()) {
        LOG_ERRFMT("Inter-core communication init failed\r\n");
        psa_panic();
    }

    mailbox_enable_interrupts();

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (mailbox_signal_is_active(signals)) {
            psa_eoi(mailbox_signal_get_active(signals));
            tfm_rpc_client_call_handler();
#if CONFIG_TFM_SPM_BACKEND_IPC == 1
        } else if (signals & ASYNC_MSG_REPLY) {
            tfm_rpc_client_call_reply();
#endif
        } else {
            psa_panic();
        }
    }
}
