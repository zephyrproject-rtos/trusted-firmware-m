/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>

#include "cmsis.h"
#include "platform_multicore.h"

__STATIC_INLINE void tfm_trigger_pendsv(void)
{
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void MHU0_MSG_0_Handler(void)
{
    uint32_t magic;

    platform_mailbox_fetch_msg_data(&magic);

    /* Sanity check based on predefined arbitrary value */
    if (magic == PSA_CLIENT_CALL_REQ_MAGIC) {
        tfm_trigger_pendsv();
    }
}
