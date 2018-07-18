/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <inttypes.h>
#include <stdio.h>
#include "psa_client.h"
#include "psa_service.h"
#include "tfm_thread.h"
#include "tfm_queue.h"
#include "secure_utilities.h"

/* Service APIs */

/* FixMe: Initial prototype. */
uint32_t psa_wait_any(uint32_t timeout)
{
    uint32_t msg_signals = 0;

    while (1) {
        msg_signals = tfm_queue_get_msg_signal();

        if (msg_signals == 0 && timeout == PSA_BLOCK) {
            tfm_thread_schedule();
        } else {
            break;
        }
    }
    return msg_signals;
}

void psa_get(psa_signal_t signal, psa_msg_t *msg)
{
    if (msg) {
        tfm_queue_get_msg_body(msg);
    }
}

void psa_end(psa_handle_t msg_handle, psa_error_t retval)
{
    struct tfm_msg_queue_item *q_msg = (struct tfm_msg_queue_item *)msg_handle;

    q_msg->msg.handle = (psa_handle_t)0;
}

size_t psa_read(psa_handle_t msg_handle, uint32_t invec_idx,
                    void *buffer, size_t num_bytes)
{
    size_t bytes;
    struct tfm_msg_queue_item *q_msg = (struct tfm_msg_queue_item *)msg_handle;

    if (invec_idx >= PSA_MAX_IOVEC) {
        return 0;
    }

    bytes = num_bytes > q_msg->invec[invec_idx].len ?
                        q_msg->invec[invec_idx].len : num_bytes;

    memcpy_m(buffer, q_msg->invec[invec_idx].base, bytes);

    return bytes;
}

void psa_write(psa_handle_t msg_handle, uint32_t outvec_idx,
                    const void *buffer, size_t num_bytes)
{
    size_t bytes;
    struct tfm_msg_queue_item *q_msg = (struct tfm_msg_queue_item *)msg_handle;

    if (outvec_idx >= PSA_MAX_IOVEC) {
        return;
    }

    bytes = num_bytes > q_msg->outvec[outvec_idx].len ?
                        q_msg->outvec[outvec_idx].len : num_bytes;

    memcpy_m(q_msg->outvec[outvec_idx].base, (void *)buffer, bytes);
}
