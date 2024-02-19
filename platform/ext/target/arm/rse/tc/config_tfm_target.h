/*
 * Copyright (c) 2022-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Use stored NV seed to provide entropy */
#define CRYPTO_NV_SEED                         0

/* Set the initial attestation token profile */
#undef ATTEST_TOKEN_PROFILE_PSA_IOT_1
#undef ATTEST_TOKEN_PROFILE_PSA_2_0_0

#define ATTEST_TOKEN_PROFILE_ARM_CCA           1

#define NS_AGENT_MAILBOX_STACK_SIZE            0xC00

/* Run the scheduler after handling a secure interrupt if the NSPE was pre-empted */
#define CONFIG_TFM_SCHEDULE_WHEN_NS_INTERRUPTED 1

#ifdef MAILBOX_ENABLE_INTERRUPTS
#undef MAILBOX_ENABLE_INTERRUPTS
#undef MAILBOX_SIGNAL_IS_ACTIVE
#undef MAILBOX_SIGNAL_GET_ACTIVE
#endif /* MAILBOX_ENABLE_INTERRUPTS */
#define MAILBOX_ENABLE_INTERRUPTS() \
                        psa_irq_enable(MAILBOX_INTERRUPT_SIGNAL); \
                        psa_irq_enable(MAILBOX_INTERRUPT_1_SIGNAL)
#define MAILBOX_SIGNAL_IS_ACTIVE(signals) \
                        (((signals) & MAILBOX_INTERRUPT_SIGNAL) || \
                         ((signals) & MAILBOX_INTERRUPT_1_SIGNAL))
#define MAILBOX_SIGNAL_GET_ACTIVE(signals) \
                        (((signals) & MAILBOX_INTERRUPT_SIGNAL) ? \
                         MAILBOX_INTERRUPT_SIGNAL : MAILBOX_INTERRUPT_1_SIGNAL)

#endif /* __CONFIG_TFM_TARGET_H__ */
