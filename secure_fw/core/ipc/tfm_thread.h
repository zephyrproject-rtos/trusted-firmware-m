/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_THREAD_H__
#define __TFM_THREAD_H__

/* FIXME: CPU specific settings, needs to be moved into CPU headers ! */
struct _ctx {
    uint32_t r4;
    uint32_t r5;
    uint32_t r6;
    uint32_t r7;
    uint32_t r8;
    uint32_t r9;
    uint32_t r10;
    uint32_t r11;
    uint32_t lr;
    uint32_t sp;
    uint32_t sp_limit;
};

#define SP_POS          0x28
#define SP_LIMIT_POS    0x2c

/* thread types */
#define TFM_THRD_CREATED    0
#define TFM_THRD_ACTIVATED  1
#define TFM_THRD_EXITED     2

typedef void *(*thrd_func_t)(void *);

struct tfm_thread_info {
    uint32_t status;        /* THRD_ACTIVATED or THRD_EXITED */
    thrd_func_t p_fn;       /* entry function */
    uint8_t *sp;            /* stack pointer */
    uint8_t *sp_limit;      /* stack limit */
    void *param;            /* entry param */
    void *retval;           /* return value */
    struct _ctx ctx;
};

/* init thread module */
int32_t tfm_thread_init(void);
void tfm_thread_schedule(void);

/* No dynamic allocation is there; all thread context are static allocated */
#define REGISTER_TFM_THREAD(thrd_func, param, stack_size)               \
    static uint8_t stack_test[stack_size] __attribute__((aligned(4))); \
    struct tfm_thread_info test_thrd = {TFM_THRD_CREATED,               \
                              thrd_func,                                \
                              stack_test + stack_size,                  \
                              stack_test,                               \
                              (param),                                  \
                              NULL}
#endif
