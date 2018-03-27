/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SECURE_API_H__
#define __TFM_SECURE_API_H__

#include "arm_cmse.h"
#include "tfm_svc.h"
#include "secure_utilities.h"
#include "tfm_core.h"

/*!
 * \def __tfm_secure_gateway_attributes__
 *
 * \brief Attributes for secure gateway functions
 */
#define __tfm_secure_gateway_attributes__ \
        __attribute__((cmse_nonsecure_entry, noinline, section("SFN")))

/* Currently only fully blocking NS while partitions are running is supported */
#define TFM_API_DEPRIORITIZE

#if !(defined(TFM_API_DEPRIORITIZE) || defined(TFM_API_SVCCLEAR))
#error "No API type selected!"
#endif

/* Hide specific errors if not debugging */
#ifdef TFM_CORE_DEBUG
#define TFM_ERROR_STATUS(status) (status)
#else
#define TFM_ERROR_STATUS(status) (TFM_PARTITION_BUSY)
#endif

#ifndef TFM_LVL
#ifndef __DOMAIN_NS
#error TFM_LVL is not defined!
#endif
#endif

extern void tfm_secure_api_error_handler(void);

struct tfm_sfn_req_s {
    uint32_t ss_id;
    int32_t (*sfn)(int32_t, int32_t, int32_t, int32_t);
    uint32_t *args;
    uint32_t exc_num;
    int32_t ns_caller : 1;
};

enum tfm_buffer_share_region_e {
    TFM_BUFFER_SHARE_DISABLE,
    TFM_BUFFER_SHARE_NS_CODE,
    TFM_BUFFER_SHARE_SCRATCH,
    TFM_BUFFER_SHARE_PRIV, /* only for TCB in level 2, all in level 1 */
    TFM_BUFFER_SHARE_DEFAULT,
};

enum tfm_ns_region_e {
    TFM_NS_REGION_CODE = 0,
    TFM_NS_REGION_DATA,
    TFM_NS_REGION_VENEER,
    TFM_NS_REGION_PERIPH_1,
    TFM_NS_REGION_PERIPH_2,
};

enum tfm_memory_access_e {
    TFM_MEMORY_ACCESS_RO = 1,
    TFM_MEMORY_ACCESS_RW = 2,
};

/* This function is called if veneer is running in handler mode */
extern int32_t tfm_core_sfn_request_function(
        struct tfm_sfn_req_s *desc_ptr);

extern int32_t tfm_core_set_buffer_area(enum tfm_buffer_share_region_e share);

extern int32_t tfm_core_validate_secure_caller(void);

extern int32_t tfm_core_memory_permission_check(
        void *ptr, uint32_t size, int32_t access);

#define TFM_CORE_SFN_REQUEST(id, fn, a, b, c, d) \
        return tfm_core_partition_request(id, fn, (int32_t)a, (int32_t)b, \
            (int32_t)c, (int32_t)d)

__attribute__ ((always_inline)) __STATIC_INLINE
int32_t tfm_core_partition_request(uint32_t id, void *fn,
            int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4)
{
    uint32_t args[4] = {arg1, arg2, arg3, arg4};
    struct tfm_sfn_req_s desc, *desc_ptr = &desc;

    desc.ss_id = id;
    desc.sfn = fn;
    desc.args = args;
    desc.ns_caller = cmse_nonsecure_caller();
    desc.exc_num = __get_active_exc_num();
#if TFM_LVL != 1
    if (desc.exc_num == EXC_NUM_THREAD_MODE) {
        int32_t res;

        __ASM("MOV r0, %1\n"
              "SVC %2\n"
              "MOV %0, r0\n"
              : "=r" (res)
              : "r" (desc_ptr), "I" (TFM_SVC_SFN_REQUEST)
              : "r0");
        return res;
    }
#endif
    return tfm_core_sfn_request_function(desc_ptr);
}

#endif /* __TFM_SECURE_API_H__ */
