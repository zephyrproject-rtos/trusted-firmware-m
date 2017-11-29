/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
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
 * \def NUM_SVC_DISPATCHERS
 *
 */
#define NUM_SVC_DISPATCHERS (6)

/**
 * \brief Naked functions associated to each
 *        SVC needed
 */
__attribute__((naked))
static uint32_t tfm_svc_dispatch_SST_GET_HANDLE(uint32_t arg0, uint32_t arg1,
                                                uint32_t arg2, uint32_t arg3)
{
    SVC(SVC_TFM_SST_GET_HANDLE);
    __ASM("BX LR");
}

__attribute__((naked))
static uint32_t tfm_svc_dispatch_SST_CREATE(uint32_t arg0, uint32_t arg1,
                                                uint32_t arg2, uint32_t arg3)
{
    SVC(SVC_TFM_SST_CREATE);
    __ASM("BX LR");
}

__attribute__((naked))
static uint32_t tfm_svc_dispatch_SST_GET_ATTRIBUTES(uint32_t arg0,uint32_t arg1,
                                                    uint32_t arg2,uint32_t arg3)
{
    SVC(SVC_TFM_SST_GET_ATTRIBUTES);
    __ASM("BX LR");
}

__attribute__((naked))
static uint32_t tfm_svc_dispatch_SST_READ(uint32_t arg0, uint32_t arg1,
                                              uint32_t arg2, uint32_t arg3)
{
    SVC(SVC_TFM_SST_READ);
    __ASM("BX LR");
}

__attribute__((naked))
static uint32_t tfm_svc_dispatch_SST_WRITE(uint32_t arg0, uint32_t arg1,
                                               uint32_t arg2, uint32_t arg3)
{
    SVC(SVC_TFM_SST_WRITE);
    __ASM("BX LR");
}

__attribute__((naked))
static uint32_t tfm_svc_dispatch_SST_DELETE(uint32_t arg0, uint32_t arg1,
                                                uint32_t arg2, uint32_t arg3)
{
    SVC(SVC_TFM_SST_DELETE);
    __ASM("BX LR");
}

/**
 * \brief Array with function pointers to the
 *        naked functions. Entry 0 is treated
*         as invalid
 */
static void *tfm_svc_dispatch_functions[NUM_SVC_DISPATCHERS+1] = {
    (void *) NULL, /* SVC_INVALID */
    (void *) tfm_svc_dispatch_SST_GET_HANDLE,
    (void *) tfm_svc_dispatch_SST_CREATE,
    (void *) tfm_svc_dispatch_SST_GET_ATTRIBUTES,
    (void *) tfm_svc_dispatch_SST_READ,
    (void *) tfm_svc_dispatch_SST_WRITE,
    (void *) tfm_svc_dispatch_SST_DELETE
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

    /* Check the NS lock has been initialized */
    if (ns_lock.init == false) {
        return TFM_ERROR_GENERIC;
    }

    /* Validate the SVC number requested */
    if ((svc_num > SVC_INVALID) && (svc_num < (NUM_SVC_DISPATCHERS+1))) {
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
