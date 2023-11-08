/*
 * Copyright (c) 2019-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_HAL_MAILBOX_H__
#define __TFM_HAL_MAILBOX_H__

#include "tfm_mailbox.h"

/* A handle to a mailbox message in use */
typedef int32_t    mailbox_msg_handle_t;

#define MAILBOX_MSG_NULL_HANDLE             ((mailbox_msg_handle_t)0)

#ifndef MAILBOX_ENABLE_INTERRUPTS
#define MAILBOX_ENABLE_INTERRUPTS() \
                        (psa_irq_enable(MAILBOX_INTERRUPT_SIGNAL))
#define MAILBOX_SIGNAL_IS_ACTIVE(signals) \
                        ((signals) & MAILBOX_INTERRUPT_SIGNAL)
#define MAILBOX_SIGNAL_GET_ACTIVE(signals) \
                        (MAILBOX_INTERRUPT_SIGNAL)
#endif /* MAILBOX_ENABLE_INTERRUPTS */

/* A single slot structure in SPE mailbox queue */
struct secure_mailbox_slot_t {
    struct mailbox_msg_t msg;

    uint8_t              ns_slot_idx;
    mailbox_msg_handle_t msg_handle;
};

struct secure_mailbox_queue_t {
    mailbox_queue_status_t       empty_slots;      /* bitmask of empty slots */

    struct secure_mailbox_slot_t queue[NUM_MAILBOX_QUEUE_SLOT];
    struct ns_mailbox_queue_t    *ns_queue;
    uint8_t                      cur_proc_slot_idx; /*
                                                     * The index of mailbox
                                                     * queue slot currently
                                                     * under processing.
                                                     */
};

/**
 * \brief Platform specific initialization of SPE mailbox.
 *
 * \param[in] s_queue           The base address of SPE mailbox queue.
 *
 * \retval MAILBOX_SUCCESS      Operation succeeded.
 * \retval Other return code    Operation failed with an error code.
 */
int32_t tfm_mailbox_hal_init(struct secure_mailbox_queue_t *s_queue);

/**
 * \brief Notify NSPE that a PSA client call return result is replied.
 *        Implemented by platform specific inter-processor communication driver.
 *
 * \retval MAILBOX_SUCCESS      The notification is successfully sent out.
 * \retval Other return code    Operation failed with an error code.
 */
int32_t tfm_mailbox_hal_notify_peer(void);

/**
 * \brief Enter critical section of NSPE mailbox
 */
void tfm_mailbox_hal_enter_critical(void);

/**
 * \brief Exit critical section of NSPE mailbox
 */
void tfm_mailbox_hal_exit_critical(void);

#endif /* __TFM_HAL_MAILBOX_H__ */
