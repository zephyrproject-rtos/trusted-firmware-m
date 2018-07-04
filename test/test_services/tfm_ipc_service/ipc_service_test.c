/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <assert.h>
#include <stdio.h>
#include "psa_client.h"
#include "psa_service.h"
#include "secure_fw/core/secure_utilities.h"
#include "secure_fw/core/tfm_secure_api.h"
#include "tfm_api.h"

#define IPC_SERVICE_BUFFER_LEN 64
#define IPC_BASIC_SIGNAL 1

static int inuse = 0;

static psa_status_t ipc_service_call(psa_msg_t *msg)
{
    int i;
    uint8_t rec_buf[IPC_SERVICE_BUFFER_LEN];
    uint8_t send_buf[IPC_SERVICE_BUFFER_LEN] = "It is just for IPC call test.";

    for (i = 0; i < PSA_MAX_IOVEC; i++) {
        if (msg->in_size[i] != 0) {
            psa_read(msg->handle, i, rec_buf, IPC_SERVICE_BUFFER_LEN);
        }
        if (msg->out_size[i] != 0) {
            psa_write(msg->handle, i, send_buf, IPC_SERVICE_BUFFER_LEN);
        }
    }
    return PSA_SUCCESS;
}

/* Test thread */
void ipc_service_test_main(void *param)
{
    uint32_t signals = 0;
    psa_msg_t msg;
    psa_status_t r;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & IPC_BASIC_SIGNAL) {
            psa_get(IPC_BASIC_SIGNAL, &msg);
            switch (msg.type) {
            case PSA_IPC_CONNECT:
                if (inuse) {
                    r = PSA_CONNECTION_REFUSED;
                } else {
                    inuse = 1;
                    r = PSA_SUCCESS;
                }
                psa_reply(msg.handle, r);
                break;
            case PSA_IPC_CALL:
                psa_reply(msg.handle, ipc_service_call(&msg));
                break;
            case PSA_IPC_DISCONNECT:
                assert (inuse == 1);
                inuse = 0;
                psa_reply(msg.handle, PSA_SUCCESS);
                break;
            default:
                /* cannot get here? [broken SPM]. TODO*/
                break;
            }
        }
    }

    return;
}
