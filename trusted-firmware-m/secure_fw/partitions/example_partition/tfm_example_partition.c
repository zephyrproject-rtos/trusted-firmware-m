/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>

#include "psa/service.h"
#include "psa/crypto.h"
#include "psa_manifest/tfm_example_partition.h"
#include "tfm_sp_log.h"
#include "tfm/tfm_spm_services.h"
#include "tfm_plat_test.h"

/**
 * \brief An example service implementation that calculates SHA-256 hash of a
 *  given message.
 */

#define SHA256_BLOCK_SIZE 64

static void tfm_example_hash(void)
{
    psa_status_t status;
    uint8_t input[SHA256_BLOCK_SIZE];  //Block size of SHA-256
    size_t input_len;
    size_t hash_len;
    psa_hash_operation_t hash_operation;
    psa_msg_t msg;

    /* Retrieve the message corresponding to the example service signal */
    status = psa_get(TFM_EXAMPLE_HASH_SIGNAL, &msg);
    if (status != PSA_SUCCESS) {
        return;
    }

    input_len = msg.in_size[0];

    /* Decode the message */
    switch (msg.type) {
    case PSA_IPC_CONNECT:
    case PSA_IPC_DISCONNECT:
        /* This service does not require any setup or teardown on connect or
         * disconnect, so just reply with success.
         */
        status = PSA_SUCCESS;
        break;
    case PSA_IPC_CALL:
        if (msg.out_size[0] != PSA_HASH_SIZE(PSA_ALG_SHA_256) ||
            msg.out_size[1] != sizeof(size_t)) {
            status = PSA_ERROR_PROGRAMMER_ERROR;
            break;
        }

        LOG_INFFMT("[Example partition] Calculating SHA-256 hash! \r\n");
        hash_operation = psa_hash_operation_init();
        psa_hash_setup(&hash_operation, PSA_ALG_SHA_256);

        /* Calculate SHA-256 hash of input */
        while(input_len > SHA256_BLOCK_SIZE) {
            psa_read(msg.handle, 0, &input, SHA256_BLOCK_SIZE);
            psa_hash_update(&hash_operation, input, SHA256_BLOCK_SIZE);
            input_len -= SHA256_BLOCK_SIZE;
        }

        if (input_len > 0) {
            psa_read(msg.handle, 0, &input, input_len);
            psa_hash_update(&hash_operation, input, input_len);
        }

        psa_hash_finish(&hash_operation,
					    input, SHA256_BLOCK_SIZE, &hash_len);

        psa_write(msg.handle, 0, input, hash_len);
        psa_write(msg.handle, 1, &hash_len, sizeof(hash_len));

        LOG_INFFMT("[Example partition] Done! \r\n");

        status = PSA_SUCCESS;
        break;
    default:
        /* Invalid message type */
        status = PSA_ERROR_PROGRAMMER_ERROR;
        break;
    }

    /* Reply with the message result status to unblock the client */
    psa_reply(msg.handle, status);
}

/**
 * \brief Templated example service.
 */
// static void tfm_example_service(void)
// {
//     psa_status_t status;
//     psa_msg_t msg;

//     /* Retrieve the message corresponding to the example service signal */
//     status = psa_get(TFM_EXAMPLE_SERVICE_SIGNAL, &msg);
//     if (status != PSA_SUCCESS) {
//         return;
//     }

//     /* Decode the message */
//     switch (msg.type) {
//     /* Any setup or teardown on IPC connect or disconnect goes here. If
//      * nothing then just reply with success.
//      */
//     case PSA_IPC_CONNECT:
//     case PSA_IPC_DISCONNECT:
//         status = PSA_SUCCESS;
//         break;
//
//     /* Service implementation goes here */
//     case PSA_IPC_CALL:
//         /* Start timer. The interrupt triggered when it expires will be handled
//          * by tfm_example_timer_handler().
//          */
//         tfm_plat_test_secure_timer_start();
//         LOG_INFFMT("[Example partition] Timer started...\r\n");

//         status = PSA_SUCCESS;
//         break;
//     default:
//         /* Invalid message type */
//         status = PSA_ERROR_PROGRAMMER_ERROR;
//         break;
//     }

//     /* Reply with the message result status to unblock the client */
//     psa_reply(msg.handle, status);
// }

/**
 * \brief A templated example interrupt handler.
 */
// static void tfm_example_timer_handler(void)
// {
//     /* Stop timer */
//     tfm_plat_test_secure_timer_stop();
//     /* Inform the SPM that the timer interrupt has been handled */
//     psa_eoi(TFM_EXAMPLE_SIGNAL_TIMER_0_IRQ);
// }

/**
 * \brief The example partition's entry function.
 */
void tfm_example_partition_main(void)
{
    psa_signal_t signals;

    /* Enable timer IRQ */
// #ifdef TFM_PSA_API
//     psa_irq_enable(TFM_EXAMPLE_SIGNAL_TIMER_0_IRQ);
// #else
//     tfm_enable_irq(TFM_EXAMPLE_SIGNAL_TIMER_0_IRQ);
// #endif

    /* Continually wait for one or more of the partition's RoT Service or
     * interrupt signals to be asserted and then handle the asserted signal(s).
     */
    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        if (signals & TFM_EXAMPLE_HASH_SIGNAL) {
            tfm_example_hash();
        }
        // /* Check the signal received from SPM and call the respective
        //  * service.
        //  */
        // if (signals & TFM_EXAMPLE_SERVICE_SIGNAL) {
        //     tfm_example_service();
        // }
        // if (signals & TFM_EXAMPLE_SIGNAL_TIMER_0_IRQ) {
        //     tfm_example_timer_handler();
        // }
    }
}
