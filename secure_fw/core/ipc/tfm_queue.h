/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_QUEUE_H__
#define __TFM_QUEUE_H__

#define TFM_MSG_QUEUE_DEPTH    5

struct tfm_msg_queue_item {
    psa_msg_t msg;
    uint32_t signal;
    /* Put in/out vectors in msg body */
    psa_invec *invec;
    psa_outvec *outvec;
    /* List opearators */
    struct tfm_msg_queue_item *prev;
    struct tfm_msg_queue_item *next;
};

/* internal error codes */
#define TFM_QUEUE_SUCCESS           0
#define TFM_QUEUE_ERROR_GENERIC     (INT32_MIN)

void tfm_queue_init(void);
uint32_t tfm_queue_get_msg_signal(void);
psa_status_t tfm_queue_put_msg(psa_signal_t signal, uint32_t type,
                               psa_invec *invec, size_t in_len,
                               psa_outvec *outvec, size_t out_len);
void tfm_queue_get_msg_body(psa_msg_t *msg);
void *memcpy_m(void *dst, const void *src, uint32_t size);

#endif
