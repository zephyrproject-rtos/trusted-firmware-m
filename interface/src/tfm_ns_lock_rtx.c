/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdint.h>
#include <stdbool.h>

#include "cmsis.h"
#include "cmsis_os2.h"

#include "tfm_api.h"
#include "tfm_ns_svc.h"

/**
 * \brief struct ns_lock_state type
 */
struct ns_lock_state
{
    bool        init;
    osMutexId_t id;
};

/**
 * \brief ns_lock status
 */
static struct ns_lock_state ns_lock = {.init=false, .id=NULL};

/**
 * \brief Mutex properties, NS lock
 */
static const osMutexAttr_t ns_lock_attrib = {
    .name = "ns_lock",
    .attr_bits = osMutexPrioInherit
};

/**
 * \def TFM_SVC_DISPATCH_NAME
 *
 * \brief Macro to declare a SVC dispatch function name
 */
#define TFM_SVC_DISPATCH_NAME(SVC_ENUM) tfm_svc_dispatch_##SVC_ENUM

/**
 * \def TFM_SVC_DISPATCH_FUNCTION
 *
 * \brief Macro to declare a SVC dispatch naked function body (4 bytes each)
 */
#define TFM_SVC_DISPATCH_FUNCTION(SVC_ENUM) \
    __attribute__((naked)) \
    static uint32_t TFM_SVC_DISPATCH_NAME(SVC_ENUM)(uint32_t arg0, \
                                                    uint32_t arg1, \
                                                    uint32_t arg2, \
                                                    uint32_t arg3) \
    { \
        SVC(SVC_ENUM); \
        __ASM("BX LR"); \
    }

/**
 * \brief Naked functions associated to each
 *        SVC in the list of X macros
 *        \ref LIST_SVC_DISPATCHERS
 */
#define X(SVC_ENUM, SVC_HANDLER) TFM_SVC_DISPATCH_FUNCTION(SVC_ENUM);
LIST_SVC_DISPATCHERS
#undef X

/**
 * \brief Array with function pointers to the
 *        naked functions. Entry 0 is treated
 *        as invalid. The other entries are
 *        taken automatically from the list of
 *        X macros \ref LIST_SVC_DISPATCHERS
 */
static void *tfm_svc_dispatch_functions[] = {
    (void *) NULL, /* SVC_INVALID */
#define X(SVC_ENUM, SVC_HANDLER) (void *)TFM_SVC_DISPATCH_NAME(SVC_ENUM),
    LIST_SVC_DISPATCHERS
#undef X
};

/**
 * \brief NS world, NS lock based dispatcher
 */
uint32_t tfm_ns_lock_svc_dispatch(enum tfm_svc_num svc_num,
                                  uint32_t arg0,
                                  uint32_t arg1,
                                  uint32_t arg2,
                                  uint32_t arg3)
{
    uint32_t result;
    uint32_t (*tfm_svc_dispatch_function_p)(uint32_t, uint32_t,
                                            uint32_t, uint32_t);

    const uint32_t num_svc_dispatchers =
      sizeof(tfm_svc_dispatch_functions)/sizeof(tfm_svc_dispatch_functions[0]);

    /* Check the NS lock has been initialized */
    if (ns_lock.init == false) {
        return TFM_ERROR_GENERIC;
    }

    /* Validate the SVC number requested */
    if ((svc_num > SVC_INVALID) && (svc_num < num_svc_dispatchers)) {
        tfm_svc_dispatch_function_p = tfm_svc_dispatch_functions[svc_num];

        /* TFM request protected by NS lock */
        osMutexAcquire(ns_lock.id,osWaitForever);
        result = (*tfm_svc_dispatch_function_p)(arg0, arg1, arg2, arg3);
        osMutexRelease(ns_lock.id);

        return result;
    }
    else {
        return TFM_ERROR_GENERIC;
    }
}

/**
 * \brief NS world, Init NS lock
 */
uint32_t tfm_ns_lock_init()
{
    if (ns_lock.init == false) {
        ns_lock.id = osMutexNew(&ns_lock_attrib);
        ns_lock.init = true;
        return TFM_SUCCESS;
    }
    else {
        return TFM_ERROR_GENERIC;
    }
}
