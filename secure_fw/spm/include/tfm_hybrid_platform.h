/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 */

#ifndef __TFM_HYBRID_PLATFORM_H__
#define __TFM_HYBRID_PLATFORM_H__

/* Hybrid Platform feature is turned off (Default) */
#define TFM_HYBRID_PLAT_SCHED_OFF       0

/*
 * Hybrid Platform Scheduling is SPE:
 * If Secure interrupts pre-empted NSPE, then run the schedule
 */
#define TFM_HYBRID_PLAT_SCHED_SPE       10

/*
 * Hybrid Platform Scheduling is NSPE:
 * If Secure interrupts pre-empted NSPE, then DO NOT run the schedule
 * NSPE will make a psa_call to mailbox to schedule the remote client's request
 */
#define TFM_HYBRID_PLAT_SCHED_NSPE      20

/*
 * Hybrid Platform Scheduling is BALANCED: !NOT YET IMPLEMENTED!
 * The platform's configuration decides which remote client's requests will be:
 * - processed right away, interrupting NSPE
 * - deferred to NSPE's decision, when calling dedicated mailbox service
 */
#define TFM_HYBRID_PLAT_SCHED_BALANCED  30

#endif /* __TFM_HYBRID_PLATFORM_H__ */
