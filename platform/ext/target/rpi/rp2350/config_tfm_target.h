/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Use stored NV seed to provide entropy */
#define CRYPTO_NV_SEED                         0

/* Use external RNG to provide entropy */
#define CRYPTO_EXT_RNG                         1

/* Run the scheduler after handling a secure interrupt if the NSPE was pre-empted */
#define CONFIG_TFM_SCHEDULE_WHEN_NS_INTERRUPTED 1

/* Use ITS for PS and NS NV counters */
#define NV_COUNTER_IN_ITS 1

#endif /* __CONFIG_TFM_TARGET_H__ */
