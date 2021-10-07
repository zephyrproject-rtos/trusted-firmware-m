/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BACKEND_H__
#define __BACKEND_H__

#include <stdint.h>
#include "spm_ipc.h"
#include "tfm_arch.h"
#include "load/spm_load_api.h"
#include "psa/error.h"

/* BASIC TYPE DEFINITIONS */

struct backend_ops_t {
    /*
     * Runtime model-specific component initialization routine. This
     * is an `assuredly` function, would panic if any error occurred.
     */
    void (*comp_init_assuredly)(struct partition_t *p_pt,
                                uint32_t service_setting);

    /*
     * Runtime model-specific kick-off method for the whole system.
     * Returns a hardware-specific control value, which is transparent
     * to SPM common logic.
     */
    uint32_t (*system_run)(void);

    /* Runtime model-specific message handling mechanism. */
    psa_status_t (*messaging)(struct service_t *p_serv,
                              struct tfm_msg_body_t *p_msg);

    /*
     * Runtime model-specific message replying.
     * Return the connection handle or the acked status code.
     */
    int32_t (*replying)(struct tfm_msg_body_t *p_msg, int32_t status);
};

/* RUNTIME MODEL BACKENDS DECLARATION */

/* IPC backend */
extern const struct backend_ops_t backend_instance;

/* The component list, and a MACRO indicate this is not a common global. */
extern struct partition_head_t partition_listhead;
#define PARTITION_LIST_ADDR (&partition_listhead)

/* TODO: Put this into NS Agent related service when available. */
extern struct context_ctrl_t *p_spm_thread_context;
#define SPM_THREAD_CONTEXT p_spm_thread_context

#endif /* __BACKEND_H__ */
