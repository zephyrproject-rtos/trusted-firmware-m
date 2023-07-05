/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/*
 * This file has declarations needed by startup.c and
 * startup_<platform>.c.
 */

#ifndef __STARTUP_H__
#define __STARTUP_H__

extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint64_t __STACK_SEAL;
#endif

typedef void(*VECTOR_TABLE_Type)(void);

void __PROGRAM_START(void) __NO_RETURN;

#define DEFAULT_IRQ_HANDLER(handler_name)  \
void __WEAK handler_name(void) __NO_RETURN; \
void handler_name(void) { \
    while(1); \
}

__NO_RETURN void Reset_Handler(void);

extern const VECTOR_TABLE_Type __VECTOR_TABLE[];

#endif /* __STARTUP_H__ */
