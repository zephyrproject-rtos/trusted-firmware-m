/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <inttypes.h>
#include <stdio.h>
#include "tfm_thread.h"

/*
 * Thread functionality:
 * 1. PSA compliant APIs
 * 2. No priority, no interrupt and tickless
 * 3. Passive context switch in m-series thread mode
 *
 * Steps:
 * 1. Provide a `two threads` context switch first <we are here>;
 * 2. Support multiple thread scheduling and waiting objects;
 * 3. Full thread management;
 */

#if defined(__ARM_ARCH_8M_MAIN__)
/* FIXME: this function needs to be moved into cpu specfic source! */
__attribute__((naked)) static void _context_switch(void *curr, void *next)
{
    /*
     * r0 = curr : place to save current context
     * r1 = next : next context to be loaded and executed
     */
    __asm(
        "stmia r0, {r4-r11, lr}     \n"
        "ldmia r1, {r4-r11, lr}     \n"
        "mrs r2, msp                \n"
        "str r2, [r0, %0]           \n"
        "mrs r2, msplim             \n"
        "str r2, [r0, %1]           \n"
        "ldr r2, [r1, %0]           \n"
        "msr msp, r2                \n"
        "ldr r2, [r1, %1]           \n"
        "msr msplim, r2             \n"
        "mov r0, #0                 \n" /* clear r0-r3, r12 for security */
        "mov r1, #0                 \n"
        "mov r2, #0                 \n"
        "mov r3, #0                 \n"
        "mov r12, #0                \n"
        "bx lr                      \n"
        ::"I"(SP_POS), "I"(SP_LIMIT_POS)
    );
}
#elif defined(__ARM_ARCH_8M_BASE__)
__attribute__((naked)) static void _context_switch(void *curr, void *next)
{
}
#else
#error "Unsupported ARM Architecture."
#endif

/* Main thread is the first executing conext */
static struct tfm_thread_info main_thrd;
static struct tfm_thread_info *p_current;

/*
 * FIXME: this global needs to be replaced with thread list
 * when multiple thread is supported
 */
extern struct tfm_thread_info test_thrd;

static void _thread_wrapper(void)
{
    /*
     * Use local variable to save client thread conext in case
     * lost tracking of wrapper client due to global changed.
     */
    struct tfm_thread_info *p_thrd_info = p_current;
    /*
     * Wrapper runs in client thread context and wait for thread
     * function return. A return indicates thread is finished.
     */
    p_thrd_info->status = TFM_THRD_ACTIVATED;
    p_thrd_info->retval = p_thrd_info->p_fn(p_thrd_info->param);
    p_thrd_info->status = TFM_THRD_EXITED;

    /* This thread context is done; switch back to main thread */
    _context_switch(&p_thrd_info->ctx, &main_thrd.ctx);
}

/*
 * Schedule current has no scheduling for current;
 * direct switch to another thread.
 */
void tfm_thread_schedule()
{
    struct tfm_thread_info *p_background = p_current;

    if (p_current == &main_thrd) {
        p_current = &test_thrd;
    } else {
        p_current = &main_thrd;
    }

    if (p_current->status != TFM_THRD_EXITED) {
        _context_switch(&p_background->ctx, &p_current->ctx);
    }
}

int32_t tfm_thread_init()
{
    /*
     * Mark current main thread activated; ignore other info items
     * since they do not make sense here
     */
    main_thrd.status = TFM_THRD_ACTIVATED;
    p_current = &main_thrd;

    /*
     * FIXME: change client thread context with thread management
     * logics later after multiple thread is supported.
     */
    test_thrd.ctx.sp = (uint32_t)test_thrd.sp;
    test_thrd.ctx.sp_limit = (uint32_t)test_thrd.sp_limit;
    test_thrd.ctx.lr = (uint32_t)_thread_wrapper;

    return 0;
}
