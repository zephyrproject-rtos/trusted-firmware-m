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
#include "tfm_queue.h"
#include "secure_utilities.h"

/*
 * Message queue
 */
static struct tfm_msg_queue_item msg_queue[TFM_MSG_QUEUE_DEPTH];
static struct tfm_msg_queue_item *q_used, *q_avail;

#define QUEUE_EMPTY() (q_used->msg.handle == (psa_handle_t)0)
#define QUEUE_FULL() (q_avail->msg.handle != (psa_handle_t)0)

void tfm_queue_init(void)
{
    struct tfm_msg_queue_item *p_item = msg_queue;
    uint32_t i;

    for (i = 0; i < TFM_MSG_QUEUE_DEPTH; i++) {
        p_item[i].msg.handle = (psa_handle_t)0;
        p_item[i].next = &p_item[(i + 1) % TFM_MSG_QUEUE_DEPTH];
        p_item[i].prev = &p_item[(i + TFM_MSG_QUEUE_DEPTH - 1) %
                                 TFM_MSG_QUEUE_DEPTH];
    }

    q_used = p_item;
    q_avail = p_item;
}

uint32_t tfm_queue_get_msg_signal(void)
{
    if (QUEUE_EMPTY()) {
        return 0;
    }

    return q_used->signal;
}

psa_error_t tfm_queue_put_msg(psa_signal_t signal, uint32_t type,
                            psa_invec *invec, size_t in_len,
                            psa_outvec *outvec, size_t out_len)
{
    uint32_t i;

    /* FixMe: need to be clarified */
    if (QUEUE_FULL()) {
        return TFM_QUEUE_ERROR_GENERIC;
    }

    if ((!invec && in_len != 0) || (!outvec && out_len != 0)) {
        ERROR_MSG("parameters error.\r\n");
        return TFM_QUEUE_ERROR_GENERIC;
    }

    /* copy contents */
    q_avail->signal = signal;

    q_avail->msg.type = type;

    q_avail->invec = invec;
    q_avail->outvec = outvec;

    for (i = 0; i < in_len; i++) {
        q_avail->msg.in_size[i] = invec[i].len;
    }

    for (i = 0; i < out_len; i++) {
        q_avail->msg.out_size[i] = outvec[i].len;
    }

    q_avail->msg.handle = (psa_handle_t)q_avail;

    q_avail = q_avail->next;

    return TFM_QUEUE_SUCCESS;
}

/* Utils */
void *memcpy_m(void *dst, const void *src, uint32_t size)
{
    uint8_t *dst_8 = (uint8_t *)dst;
    uint8_t *src_8 = (uint8_t *)src;

    while (size--) {
        *dst_8++ = *src_8++;
    }

    return dst;
}

void tfm_queue_get_msg_body(psa_msg_t *msg)
{
    memcpy_m(msg, &q_used->msg, sizeof(*msg));
    q_used = q_used->next;
}
