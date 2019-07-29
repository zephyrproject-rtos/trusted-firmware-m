/*
 * Copyright (c) 2017-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdint.h>
#include <stdbool.h>

#include "os_wrapper/mutex.h"

#include "tfm_api.h"
#include "tfm_ns_interface.h"

/**
 * \brief the ns_lock ID
 */
static uint32_t ns_lock_id = (uint32_t)NULL;

__attribute__((weak))
int32_t tfm_ns_interface_dispatch(veneer_fn fn,
                                  uint32_t arg0, uint32_t arg1,
                                  uint32_t arg2, uint32_t arg3)
{
    int32_t result;

    /* TFM request protected by NS lock */
    if (os_wrapper_mutex_acquire(ns_lock_id, OS_WRAPPER_WAIT_FOREVER)
            != OS_WRAPPER_SUCCESS) {
        return (int32_t)TFM_ERROR_GENERIC;
    }

    result = fn(arg0, arg1, arg2, arg3);

    if (os_wrapper_mutex_release(ns_lock_id) != OS_WRAPPER_SUCCESS) {
        return (int32_t)TFM_ERROR_GENERIC;
    }

    return result;
}

__attribute__((weak))
enum tfm_status_e tfm_ns_interface_init(void)
{
    uint32_t id;

    id = os_wrapper_mutex_create();
    if (id == OS_WRAPPER_ERROR) {
        return TFM_ERROR_GENERIC;
    }

    ns_lock_id = id;
    return TFM_SUCCESS;
}
