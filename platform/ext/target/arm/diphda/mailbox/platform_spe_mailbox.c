/*
 * Copyright (c) 2020-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform_multicore.h"
#include "tfm_spe_mailbox.h"

/* -------------------------------------- HAL API ------------------------------------ */

/* Stub implementation to allow TFM to be compiled */

int32_t tfm_mailbox_hal_notify_peer(void)
{
    return PLATFORM_MAILBOX_SUCCESS;
}

int32_t tfm_mailbox_hal_init(struct secure_mailbox_queue_t *s_queue)
{
    return PLATFORM_MAILBOX_SUCCESS;
}

void tfm_mailbox_hal_enter_critical(void)
{
}

void tfm_mailbox_hal_exit_critical(void)
{
}
