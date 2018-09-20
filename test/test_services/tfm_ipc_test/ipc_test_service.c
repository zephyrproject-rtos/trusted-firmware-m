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

#define IPC_SERVICE_BUFFER_LEN 256
#define PSA_IPC_SIGNAL 1

static int inuse = 0;

static psa_status_t ipc_service_connect(psa_msg_t *msg)
{
    uint32_t minor_version;

    if (msg->in_size[0] == 0) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_read(msg->handle, 0, &minor_version, sizeof(minor_version));
    printf("Requested minor version for service1 connect: %d.\r\n",
           minor_version);

    /* notify client if accepted or refused */
    if (minor_version == 0) {
        return PSA_CONNECTION_REFUSED;
    }
    return PSA_SUCCESS;
}

static psa_status_t ipc_service_call(psa_msg_t *msg)
{
    int i;
    uint8_t rec_buf[IPC_SERVICE_BUFFER_LEN];
    uint8_t send_buf[IPC_SERVICE_BUFFER_LEN] = "It is just for IPC call test.";

    for (i = 0; i < PSA_MAX_IOVEC; i++) {
        if (msg->in_size[i] != 0) {
            psa_read(msg->handle, i, rec_buf, IPC_SERVICE_BUFFER_LEN);
            printf("receive buffer index is %d, data is %s.\r\n", i,
                                            (char *)rec_buf);
        }
        if (msg->out_size[i] != 0) {
            psa_write(msg->handle, i, send_buf, IPC_SERVICE_BUFFER_LEN);
        }
    }
    return PSA_SUCCESS;
}

/* Test thread */
void *ipc_test_partition_main(void *param)
{
    uint32_t signals = 0;
    psa_msg_t msg;
    psa_status_t r;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        printf("ipc get signals 0x%x\r\n", signals);

        if (signals & PSA_IPC_SIGNAL) {
            psa_get(PSA_IPC_SIGNAL, &msg);
            switch (msg.type) {
            case PSA_IPC_CONNECT:
                if (inuse) {
                    r = PSA_CONNECTION_REFUSED;
                } else {
                    inuse = 1;
                    r = ipc_service_connect(&msg);
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

    return NULL;
}
