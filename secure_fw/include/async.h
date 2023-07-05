/*
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __ASYNC_H__
#define __ASYNC_H__

#include "psa/error.h"

/**
 * Status returned for a request that will complete asynchronously.
 */
#define AGENT_STATUS_IN_PROGRESS  ((psa_status_t)-256)

/**
 * The signal number for the Secure Partition message acknowledgment.
 */
#define ASYNC_MSG_REPLY    (0x00000004u)

#endif /* __ASYNC_H__ */
