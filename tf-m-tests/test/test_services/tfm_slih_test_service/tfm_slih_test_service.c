/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/service.h"
#include "psa_manifest/tfm_slih_test_service.h"
#include "tfm_slih_test_service_types.h"
#include "tfm_plat_test.h"
#include "tfm_sp_log.h"

static void timer0_handler(void)
{
    tfm_plat_test_secure_timer_stop();
    psa_irq_disable(TFM_TIMER0_IRQ_SIGNAL);
    psa_eoi(TFM_TIMER0_IRQ_SIGNAL);
}

static void slih_test_case_1(psa_msg_t *msg) {
    psa_irq_enable(TFM_TIMER0_IRQ_SIGNAL);

    tfm_plat_test_secure_timer_start();

    if (psa_wait(TFM_TIMER0_IRQ_SIGNAL, PSA_BLOCK) != TFM_TIMER0_IRQ_SIGNAL) {
        psa_panic();
    }
    timer0_handler();

    psa_reply(msg->handle, PSA_SUCCESS);
}

static void slih_test_get_msg(psa_signal_t signal, psa_msg_t *msg) {
    psa_status_t status;

    status = psa_get(signal, msg);
    if (status != PSA_SUCCESS) {
        psa_panic();
    }
}

void tfm_slih_test_service_entry(void)
{
    psa_signal_t signals = 0;
    psa_msg_t msg;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_SLIH_TEST_CASE_SIGNAL) {
            slih_test_get_msg(TFM_SLIH_TEST_CASE_SIGNAL, &msg);
            switch (msg.type) {
            case TFM_SLIH_TEST_CASE_1:
                slih_test_case_1(&msg);
                break;
            default:
                LOG_ERRFMT("SLIH test service: Invalid message type: 0x%x\r\n",
                   msg.type);
                psa_panic();
                break;
            }
        }
    }
}
