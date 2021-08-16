/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/service.h"
#include "psa_manifest/tfm_flih_test_service.h"
#include "tfm_flih_test_service_types.h"
#include "tfm_plat_test.h"
#include "tfm_sp_log.h"

/* The execution flow ensures there are no race conditions for test_type */
static int32_t test_type = TFM_FLIH_TEST_CASE_INVALID;
/*
 * Records times of triggered
 *
 * The test cases do not care about exact value of flih_timer_triggered.
 * They only needs to know if it has reached a certain value.
 * And it is a single-read-single-writer model.
 * So the race condition of accessing flih_timer_triggered between the Partition
 * thread and IRS is acceptable.
 */
static volatile uint32_t flih_timer_triggered = 0;

psa_flih_result_t tfm_timer0_irq_flih(void)
{
    tfm_plat_test_secure_timer_clear_intr();

    switch (test_type) {
    case TFM_FLIH_TEST_CASE_1:
        flih_timer_triggered += 1;
        return PSA_FLIH_NO_SIGNAL;
    case TFM_FLIH_TEST_CASE_2:
        flih_timer_triggered += 1;
        if (flih_timer_triggered == 10) {
            return PSA_FLIH_SIGNAL;
        } else {
            return PSA_FLIH_NO_SIGNAL;
        }
        break;
    default:
        psa_panic();
        break;
    }

    return PSA_FLIH_NO_SIGNAL;
}

static void flih_test_get_msg(psa_signal_t signal, psa_msg_t *msg) {
    psa_status_t status;

    status = psa_get(signal, msg);
    if (status != PSA_SUCCESS) {
        psa_panic();
    }
}

static void flih_test_case_1(psa_msg_t *msg) {
    flih_timer_triggered = 0;

    psa_irq_enable(TFM_TIMER0_IRQ_SIGNAL);

    tfm_plat_test_secure_timer_start();

    while (flih_timer_triggered < 10);
    tfm_plat_test_secure_timer_stop();

    psa_irq_disable(TFM_TIMER0_IRQ_SIGNAL);

    psa_reply(msg->handle, PSA_SUCCESS);
}

static void flih_test_case_2(psa_msg_t *msg) {
    flih_timer_triggered = 0;

    psa_irq_enable(TFM_TIMER0_IRQ_SIGNAL);

    tfm_plat_test_secure_timer_start();

    if (psa_wait(TFM_TIMER0_IRQ_SIGNAL, PSA_BLOCK) != TFM_TIMER0_IRQ_SIGNAL) {
        psa_panic();
    }
    tfm_plat_test_secure_timer_stop();

    psa_reset_signal(TFM_TIMER0_IRQ_SIGNAL);
    psa_irq_disable(TFM_TIMER0_IRQ_SIGNAL);

    psa_reply(msg->handle, PSA_SUCCESS);
}

void tfm_flih_test_service_entry(void)
{
    psa_signal_t signals = 0;
    psa_msg_t msg;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_FLIH_TEST_CASE_SIGNAL) {
            flih_test_get_msg(TFM_FLIH_TEST_CASE_SIGNAL, &msg);
            test_type = msg.type;
            switch (test_type) {
            case TFM_FLIH_TEST_CASE_1:
                flih_test_case_1(&msg);
                break;
            case TFM_FLIH_TEST_CASE_2:
                flih_test_case_2(&msg);
                break;
            default:
                LOG_ERRFMT("FLIH test service: Invalid message type: 0x%x\r\n",
                   msg.type);
                psa_panic();
                break;
            }
        }
    }
}
