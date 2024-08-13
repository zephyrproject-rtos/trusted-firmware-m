/*
 * Copyright (c) 2022-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Use stored NV seed to provide entropy */
#undef CRYPTO_NV_SEED
#define CRYPTO_NV_SEED                         0

/* Use external RNG to provide entropy */
#define CRYPTO_EXT_RNG                         1

#define NS_AGENT_MAILBOX_STACK_SIZE            0xC00

/* Run the scheduler after handling a secure interrupt if the NSPE was pre-empted */
#define CONFIG_TFM_SCHEDULE_WHEN_NS_INTERRUPTED 1

#endif /* __CONFIG_TFM_TARGET_H__ */
