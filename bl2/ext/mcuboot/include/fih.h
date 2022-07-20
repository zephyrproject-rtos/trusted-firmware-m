/*
 * Copyright (c) 2020-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Interop between TF-M fih.h and mcuboot fault_injection_hardening.h, so that
 * platform code can target fih.h and for bl2 this will be redirected to
 * fault_injection_hardening.h
 */

#ifndef __INTEROP_FIH_H__
#define __INTEROP_FIH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "stdint.h"

#undef FIH_ENABLE_GLOBAL_FAIL
#undef FIH_ENABLE_CFI
#undef FIH_ENABLE_DOUBLE_VARS
#undef FIH_ENABLE_DELAY

#if !defined(MCUBOOT_FIH_PROFILE_OFF)

#if defined(MCUBOOT_FIH_PROFILE_LOW)
#define FIH_ENABLE_GLOBAL_FAIL
#define FIH_ENABLE_CFI

#elif defined(MCUBOOT_FIH_PROFILE_MEDIUM)
#define FIH_ENABLE_DOUBLE_VARS
#define FIH_ENABLE_GLOBAL_FAIL
#define FIH_ENABLE_CFI

#elif defined(MCUBOOT_FIH_PROFILE_HIGH)
#define FIH_ENABLE_DELAY         /* Requires an hardware entropy source */
#define FIH_ENABLE_DOUBLE_VARS
#define FIH_ENABLE_GLOBAL_FAIL
#define FIH_ENABLE_CFI

#else
#error "Invalid FIH Profile configuration"
#endif /* MCUBOOT_FIH_PROFILE */

/*
 * FIH return type macro changes the function return types to fih_int.
 * All functions that need to be protected by FIH and called via FIH_CALL must
 * return a fih_int type.
 */
#define FIH_RET_TYPE(type)    fih_int

#include "bootutil/fault_injection_hardening.h"

#else /* MCUBOOT_FIH_PROFILE_OFF */
typedef int32_t fih_int;

#define FIH_INT_INIT(x)       (x)

#define FIH_SUCCESS           0
#define FIH_FAILURE           -1

#define fih_int_validate(x)

#define fih_int_decode(x)     (x)

#define fih_int_encode(x)     (x)

#define fih_int_encode_zero_equality(x) ((x) == 0 ? 0 : 1)

#define fih_eq(x, y)          ((x) == (y))

#define fih_not_eq(x, y)      ((x) != (y))

#define fih_delay_init()      (0)
#define fih_delay()

#define FIH_CALL(f, ret, ...) \
    do { \
        ret = f(__VA_ARGS__); \
    } while (0)

#define FIH_RET(ret) \
    do { \
        return ret; \
    } while (0)

#define FIH_PANIC do { \
        while(1) {}; \
    } while (0)

#define FIH_RET_TYPE(type)    type

#define FIH_CFI_STEP_INIT(x)
#define FIH_CFI_STEP_DECREMENT()
#define FIH_CFI_STEP_ERR_RESET()

#define FIH_LABEL_CRITICAL_POINT()

#endif /* !MCUBOOT_FIH_PROFILE_OFF */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INTEROP_FIH_H__ */
