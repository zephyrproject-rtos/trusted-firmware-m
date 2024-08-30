/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_ATTACK_TRACKING_COUNTER_H__
#define __RSE_ATTACK_TRACKING_COUNTER_H__

#ifdef __cplusplus
extern "C" {
#endif

void increment_attack_tracking_counter_major(void);
void increment_attack_tracking_counter_minor(void);

#ifdef __cplusplus
}
#endif

#endif /* __RSE_ATTACK_TRACKING_COUNTER_H__ */
