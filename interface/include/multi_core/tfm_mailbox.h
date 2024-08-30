/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 * Copyright (c) 2022-2024 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * This is header file of common mailbox objects shared by NSPE and SPE.
 * Please refer to tfm_ns_mailbox.h for the definitions only used in NSPE
 * mailbox library.
 * Please refer to tfm_spe_mailbox.h for the SPE specific definitions and APIs.
 */

#ifndef __TFM_MAILBOX_H__
#define __TFM_MAILBOX_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "cmsis_compiler.h"
#include "psa/client.h"
#include "tfm_mailbox_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/*
 * If the mailbox itself can be cached (in either the SPE or NSPE, or both),
 * we want to add some padding to avoid any cache line conflicts.
 * Platforms can set both MAILBOX_IS_UNCACHED_S and MAILBOX_IS_UNCACHED_NS to 1
 * to shrink the mailbox struct a bit if it cannot be cached.
 */
#if (MAILBOX_IS_UNCACHED_S == 1) && (MAILBOX_IS_UNCACHED_NS == 1)
/* Let the compiler pick the alignment */
#define MAILBOX_ALIGN
#else
#if !defined(MAILBOX_CACHE_LINE_SIZE)
/* 32 bytes is a common dcache line size on Armv8-M */
#define MAILBOX_CACHE_LINE_SIZE 32
#endif
#define MAILBOX_ALIGN  __ALIGNED(MAILBOX_CACHE_LINE_SIZE)
#endif


/* PSA client call type value */
#define MAILBOX_PSA_FRAMEWORK_VERSION       (0x1)
#define MAILBOX_PSA_VERSION                 (0x2)
#define MAILBOX_PSA_CONNECT                 (0x3)
#define MAILBOX_PSA_CALL                    (0x4)
#define MAILBOX_PSA_CLOSE                   (0x5)

/* Return code of mailbox APIs */
#define MAILBOX_SUCCESS                     (0)
#define MAILBOX_QUEUE_FULL                  (INT32_MIN + 1)
#define MAILBOX_INVAL_PARAMS                (INT32_MIN + 2)
#define MAILBOX_NO_PERMS                    (INT32_MIN + 3)
#define MAILBOX_NO_PEND_EVENT               (INT32_MIN + 4)
#define MAILBOX_CHAN_BUSY                   (INT32_MIN + 5)
#define MAILBOX_CALLBACK_REG_ERROR          (INT32_MIN + 6)
#define MAILBOX_INIT_ERROR                  (INT32_MIN + 7)
#define MAILBOX_GENERIC_ERROR               (INT32_MIN + 8)

/*
 * This structure holds the parameters used in a PSA client call.
 */
struct psa_client_params_t {
    union {
        struct {
            uint32_t        sid;
        } psa_version_params;

        struct {
            uint32_t        sid;
            uint32_t        version;
        } psa_connect_params;

        struct {
            psa_handle_t    handle;
            int32_t         type;
            const psa_invec *in_vec;
            size_t          in_len;
            psa_outvec      *out_vec;
            size_t          out_len;
        } psa_call_params;

        struct {
            psa_handle_t    handle;
        } psa_close_params;
    };
};

/* Mailbox message passed from NSPE to SPE to deliver a PSA client call */
struct mailbox_msg_t {
    uint32_t                    call_type; /* PSA client call type */
    struct psa_client_params_t  params;    /* Contain parameters used in PSA
                                            * client call
                                            */

    int32_t                     client_id; /* Optional client ID of the
                                            * non-secure caller.
                                            * It is required to identify the
                                            * non-secure task when NSPE OS
                                            * enforces non-secure task isolation
                                            */
};

/*
 * Mailbox reply structure in non-secure memory
 * to hold the PSA client call return result from SPE
 */
struct mailbox_reply_t {
    int32_t    return_val;
};

/*
 * A single slot structure in NSPE mailbox queue.
 * This structure is an ABI between SPE and NSPE mailbox instances.
 * So, it must not include data that are not used by SPE like information about NS threads
 * or that depends on NSPE build settings.
 */
struct mailbox_slot_t {
    /* In the array, msg must not share a cache line with the preceding reply */
    struct mailbox_msg_t   msg    MAILBOX_ALIGN;
    /* reply.return_val must not share a cache line with msg */
    struct mailbox_reply_t reply  MAILBOX_ALIGN;
} MAILBOX_ALIGN;

typedef uint32_t   mailbox_queue_status_t;

/*
 * NSPE mailbox status shared between TF-M and mailbox client.
 * This structure is separated from slots to allow flexible allocation of slots.
 * So, it's safe to change number of slots on non-secure side without rebuild of TF-M.
 * Access to mailbox status should be guarded by critical section between cores.
 * Thus there is no need to allocate a separate cache line for each flag.
 */
struct mailbox_status_t {
    mailbox_queue_status_t   pend_slots;        /* Bitmask of slots pending
                                                 * for SPE handling
                                                 */
    mailbox_queue_status_t   replied_slots;     /* Bitmask of active slots
                                                 * containing PSA client call
                                                 * return result
                                                 */
} MAILBOX_ALIGN;

/*
 * Data used to send information to mailbox partition about mailbox queue allocated by non-secure image.
 * It's expected that data in this structure is not modified by the secure side.
 */
struct mailbox_init_t {
    /* Shared data with fixed size */
    struct mailbox_status_t *status MAILBOX_ALIGN;

    /* Number of slots allocated by NS. */
    uint32_t slot_count;

    /* Pointer to struct mailbox_slot_t[slot_count] allocated by NS */
    struct mailbox_slot_t *slots;
};

#ifdef __cplusplus
}
#endif

#endif /* __TFM_MAILBOX_H__ */
