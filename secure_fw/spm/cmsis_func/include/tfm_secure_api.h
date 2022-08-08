/*
 * Copyright (c) 2017-2022, Arm Limited. All rights reserved.
 * Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SECURE_API_H__
#define __TFM_SECURE_API_H__

#include <arm_cmse.h>
#include "tfm_arch.h"
#include "tfm_api.h"
#include "utilities.h"
#include "tfm_boot_status.h"
#include "psa/service.h"
#include "tfm_core_svc.h"

extern void tfm_secure_api_error_handler(void);

typedef int32_t(*sfn_t)(int32_t, int32_t, int32_t, int32_t);

struct tfm_sfn_req_s {
    uint32_t sp_id;
    sfn_t sfn;
    int32_t *args;
    uint32_t caller_part_idx;
    bool ns_caller;
};

enum tfm_status_e tfm_core_get_caller_client_id(int32_t *caller_client_id);

int32_t tfm_core_sfn_request(const struct tfm_sfn_req_s *desc_ptr);

int32_t tfm_spm_sfn_request_thread_mode(struct tfm_sfn_req_s *desc_ptr);

#define TFM_CORE_IOVEC_SFN_REQUEST(id, is_ns, fn, a, b, c, d)        \
        return tfm_core_partition_request(id, is_ns, fn,             \
                (int32_t)a, (int32_t)b, (int32_t)c, (int32_t)d)

int32_t tfm_core_partition_request(uint32_t id, bool is_ns, void *fn,
            int32_t arg1, int32_t arg2, int32_t arg3, int32_t arg4);

__attribute__ ((always_inline)) __STATIC_INLINE
bool tfm_core_is_ns_client(void)
{
    /*
     * This preprocessor condition checks if a version of GCC smaller than
     * 7.3.1 is being used to compile the code.
     * These versions are affected by a bug on the cmse_nonsecure_caller
     * intrinsic which returns incorrect results.
     * Please check Bug 85203 on GCC Bugzilla for more information.
     */
#if defined(__GNUC__) && !defined(__ARMCC_VERSION) && \
    (__GNUC__ < 7 || \
     (__GNUC__ == 7 && (__GNUC_MINOR__ < 3 || \
                       (__GNUC_MINOR__ == 3 && __GNUC_PATCHLEVEL__ < 1))))
    /*
     * Use the fact that, if called from Non-Secure, the LSB of the return
     * address is set to 0.
     */
    return !(
          (uintptr_t)__builtin_extract_return_addr(__builtin_return_address(0U))
          & 0x1);
#else
    /*
     * Convert the result of cmse_nonsecure_caller from an int to a bool
     * to prevent using an int in the tfm_sfn_req_s structure.
     */
    return (cmse_nonsecure_caller() != 0) ? true : false;
#endif /* Check for GCC compiler version smaller than 7.3.1 */
}

#endif /* __TFM_SECURE_API_H__ */
