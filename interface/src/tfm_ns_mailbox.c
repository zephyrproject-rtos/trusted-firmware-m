/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string.h>
#include "tfm_ns_mailbox.h"
#include "tfm_plat_ns.h"

/* The pointer to NSPE mailbox queue */
static struct ns_mailbox_queue_t *mailbox_queue_ptr = NULL;

static inline void clear_queue_slot_empty(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        mailbox_queue_ptr->empty_slots &= ~(1 << idx);
    }
}

static inline void set_queue_slot_empty(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        mailbox_queue_ptr->empty_slots |= (1 << idx);
    }
}

static inline void set_queue_slot_pend(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        mailbox_queue_ptr->pend_slots |= (1 << idx);
    }
}

static inline int32_t get_mailbox_msg_handle(uint8_t idx,
                                             mailbox_msg_handle_t *handle)
{
    if ((idx >= NUM_MAILBOX_QUEUE_SLOT) || !handle) {
        return MAILBOX_INVAL_PARAMS;
    }

    *handle = (mailbox_msg_handle_t)(idx + 1);

    return MAILBOX_SUCCESS;
}

static inline int32_t get_mailbox_msg_idx(mailbox_msg_handle_t handle,
                                          uint8_t *idx)
{
    if ((handle == MAILBOX_MSG_NULL_HANDLE) || !idx) {
        return MAILBOX_INVAL_PARAMS;
    }

    *idx = (uint8_t)(handle - 1);

    return MAILBOX_SUCCESS;
}

static inline void clear_queue_slot_replied(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        mailbox_queue_ptr->replied_slots &= ~(1 << idx);
    }
}

static inline void set_queue_slot_woken(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        mailbox_queue_ptr->queue[idx].is_woken = true;
    }
}

static inline bool is_queue_slot_woken(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        return mailbox_queue_ptr->queue[idx].is_woken;
    }

    return false;
}

static inline void clear_queue_slot_woken(uint8_t idx)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        mailbox_queue_ptr->queue[idx].is_woken = false;
    }
}

static uint8_t acquire_empty_slot(const struct ns_mailbox_queue_t *queue)
{
    uint8_t idx;
    mailbox_queue_status_t status;

    tfm_ns_mailbox_hal_enter_critical();
    status = queue->empty_slots;

    if (!status) {
        /* No empty slot */
        tfm_ns_mailbox_hal_exit_critical();
        return NUM_MAILBOX_QUEUE_SLOT;
    }

    for (idx = 0; idx < NUM_MAILBOX_QUEUE_SLOT; idx++) {
        if (status & (1 << idx)) {
            break;
        }
    }

    clear_queue_slot_empty(idx);

    tfm_ns_mailbox_hal_exit_critical();

    return idx;
}

static void set_msg_owner(uint8_t idx, const void *owner)
{
    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        mailbox_queue_ptr->queue[idx].owner = owner;
    }
}

mailbox_msg_handle_t tfm_ns_mailbox_tx_client_req(uint32_t call_type,
                                       const struct psa_client_params_t *params,
                                       int32_t client_id)
{
    uint8_t idx;
    struct mailbox_msg_t *msg_ptr;
    mailbox_msg_handle_t handle;
    const void *task_handle;

    if (!mailbox_queue_ptr) {
        return MAILBOX_MSG_NULL_HANDLE;
    }

    if (!params) {
        return MAILBOX_MSG_NULL_HANDLE;
    }

    idx = acquire_empty_slot(mailbox_queue_ptr);
    if (idx >= NUM_MAILBOX_QUEUE_SLOT) {
        return MAILBOX_QUEUE_FULL;
    }

    /* Fill the mailbox message */
    msg_ptr = &mailbox_queue_ptr->queue[idx].msg;

    msg_ptr->call_type = call_type;
    memcpy(&msg_ptr->params, params, sizeof(msg_ptr->params));
    msg_ptr->client_id = client_id;

    /*
     * Fetch the current task handle. The task will be woken up according the
     * handle value set in the owner field.
     */
    task_handle = tfm_ns_mailbox_get_task_handle();
    set_msg_owner(idx, task_handle);

    get_mailbox_msg_handle(idx, &handle);

    tfm_ns_mailbox_hal_enter_critical();
    set_queue_slot_pend(idx);
    tfm_ns_mailbox_hal_exit_critical();

    tfm_ns_mailbox_hal_notify_peer();

    return handle;
}

int32_t tfm_ns_mailbox_rx_client_reply(mailbox_msg_handle_t handle,
                                       int32_t *reply)
{
    uint8_t idx;
    int32_t ret;

    if (!mailbox_queue_ptr) {
        return MAILBOX_INVAL_PARAMS;
    }

    if ((handle == MAILBOX_MSG_NULL_HANDLE) || (!reply)) {
        return MAILBOX_INVAL_PARAMS;
    }

    ret = get_mailbox_msg_idx(handle, &idx);
    if (ret != MAILBOX_SUCCESS) {
        return ret;
    }

    *reply = mailbox_queue_ptr->queue[idx].reply.return_val;

    /* Clear up the owner field */
    set_msg_owner(idx, NULL);

    tfm_ns_mailbox_hal_enter_critical();
    clear_queue_slot_replied(idx);
    clear_queue_slot_woken(idx);
    /*
     * Make sure that the empty flag is set after all the other status flags are
     * re-initialized.
     */
    set_queue_slot_empty(idx);
    tfm_ns_mailbox_hal_exit_critical();

    return MAILBOX_SUCCESS;
}

bool tfm_ns_mailbox_is_msg_replied(mailbox_msg_handle_t handle)
{
    uint8_t idx;
    int32_t ret;
    mailbox_queue_status_t status;

    if (!mailbox_queue_ptr) {
        return false;
    }

    if (handle == MAILBOX_MSG_NULL_HANDLE) {
        return false;
    }

    ret = get_mailbox_msg_idx(handle, &idx);
    if (ret != MAILBOX_SUCCESS) {
        return false;
    }

    tfm_ns_mailbox_hal_enter_critical();
    status = mailbox_queue_ptr->replied_slots;
    tfm_ns_mailbox_hal_exit_critical();

    if (status & (1 << idx)) {
        return true;
    }

    return false;
}

mailbox_msg_handle_t tfm_ns_mailbox_fetch_reply_msg_isr(void)
{
    uint8_t idx;
    mailbox_msg_handle_t handle;
    mailbox_queue_status_t replied_status;

    if (!mailbox_queue_ptr) {
        return MAILBOX_MSG_NULL_HANDLE;
    }

    tfm_ns_mailbox_hal_enter_critical_isr();
    replied_status = mailbox_queue_ptr->replied_slots;
    tfm_ns_mailbox_hal_exit_critical_isr();

    if (!replied_status) {
        return MAILBOX_MSG_NULL_HANDLE;
    }

    for (idx = 0; idx < NUM_MAILBOX_QUEUE_SLOT; idx++) {
        /* Find the first replied message in queue */
        if (replied_status & (0x1UL << idx)) {
            tfm_ns_mailbox_hal_enter_critical_isr();
            clear_queue_slot_replied(idx);
            set_queue_slot_woken(idx);
            tfm_ns_mailbox_hal_exit_critical_isr();

            if (get_mailbox_msg_handle(idx, &handle) == MAILBOX_SUCCESS) {
                return handle;
            }
       }
    }

    return MAILBOX_MSG_NULL_HANDLE;
}

const void *tfm_ns_mailbox_get_msg_owner(mailbox_msg_handle_t handle)
{
    uint8_t idx;

    if (get_mailbox_msg_idx(handle, &idx) != MAILBOX_SUCCESS) {
        return NULL;
    }

    if (idx < NUM_MAILBOX_QUEUE_SLOT) {
        return mailbox_queue_ptr->queue[idx].owner;
    }

    return NULL;
}

int32_t tfm_ns_mailbox_init(struct ns_mailbox_queue_t *queue)
{
    int32_t ret;

    if (!queue) {
        return MAILBOX_INVAL_PARAMS;
    }

    /*
     * Further verification of mailbox queue address may be required according
     * to non-secure memory assignment.
     */

    memset(queue, 0, sizeof(*queue));

    /* Initialize empty bitmask */
    queue->empty_slots = (mailbox_queue_status_t)((1 << NUM_MAILBOX_QUEUE_SLOT)
                                                  - 1);

    mailbox_queue_ptr = queue;

    /* Platform specific initialization. */
    ret = tfm_ns_mailbox_hal_init(queue);

    return ret;
}

#ifdef TFM_MULTI_CORE_MULTI_CLIENT_CALL
int32_t tfm_ns_mailbox_wait_reply(mailbox_msg_handle_t handle)
{
    uint8_t idx;
    int32_t ret;

    if (!mailbox_queue_ptr) {
        return MAILBOX_INVAL_PARAMS;
    }

    if (handle == MAILBOX_MSG_NULL_HANDLE) {
        return MAILBOX_INVAL_PARAMS;
    }

    ret = get_mailbox_msg_idx(handle, &idx);
    if (ret != MAILBOX_SUCCESS) {
        return ret;
    }

    while (1) {
        tfm_ns_mailbox_hal_wait_reply(handle);

        /*
         * Woken up from sleep
         * Check the completed flag to make sure that the current thread is
         * woken up by reply event, rather than other events.
         */
        tfm_ns_mailbox_hal_enter_critical();
        if (is_queue_slot_woken(idx)) {
            tfm_ns_mailbox_hal_exit_critical();
            break;
        }
        tfm_ns_mailbox_hal_exit_critical();
    }

    return MAILBOX_SUCCESS;
}
#endif
