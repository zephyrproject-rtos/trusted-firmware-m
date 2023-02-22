/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2022-2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "bitops.h"
#include "config_impl.h"
#include "config_spm.h"
#include "critical_section.h"
#include "psa/lifecycle.h"
#include "psa/service.h"
#include "interrupt.h"
#include "spm.h"
#include "tfm_arch.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/interrupt_defs.h"
#include "utilities.h"
#include "ffm/backend.h"
#include "ffm/psa_api.h"
#include "tfm_rpc.h"
#include "tfm_api.h"
#include "tfm_hal_interrupt.h"
#include "tfm_hal_platform.h"
#include "tfm_psa_call_pack.h"
#include "tfm_hal_isolation.h"

#if PSA_FRAMEWORK_HAS_MM_IOVEC

/*
 * The MM-IOVEC status
 * The max total number of invec and outvec is 8.
 * Each invec/outvec takes 4 bit, 32 bits in total.
 *
 * The encoding format of the MM-IOVEC status:
 *--------------------------------------------------------------
 *|  Bit   |  31 - 28  |  27 - 24  | ... |  7 - 4   |  3 - 0   |
 *--------------------------------------------------------------
 *| Vector | outvec[3] | outvec[2] | ... | invec[1] | invec[0] |
 *--------------------------------------------------------------
 *
 * Take invec[0] as an example:
 *
 * bit 0:  whether invec[0] has been mapped.
 * bit 1:  whether invec[0] has been unmapped.
 * bit 2:  whether invec[0] has been accessed using psa_read(), psa_skip() or
 *         psa_write().
 * bit 3:  reserved for invec[0].
 */

#define IOVEC_STATUS_BITS              4   /* Each vector occupies 4 bits. */
#define OUTVEC_IDX_BASE                4   /*
                                            * Base index of outvec.
                                            * There are four invecs in front of
                                            * outvec.
                                            */
#define INVEC_IDX_BASE                 0   /* Base index of invec. */

#define IOVEC_MAPPED_BIT               (1U << 0)
#define IOVEC_UNMAPPED_BIT             (1U << 1)
#define IOVEC_ACCESSED_BIT             (1U << 2)

#define IOVEC_IS_MAPPED(handle, iovec_idx)      \
    ((((handle)->iovec_status) >> ((iovec_idx) * IOVEC_STATUS_BITS)) &  \
                               IOVEC_MAPPED_BIT)
#define IOVEC_IS_UNMAPPED(handle, iovec_idx)    \
    ((((handle)->iovec_status) >> ((iovec_idx) * IOVEC_STATUS_BITS)) &  \
                               IOVEC_UNMAPPED_BIT)
#define IOVEC_IS_ACCESSED(handle, iovec_idx)    \
    ((((handle)->iovec_status) >> ((iovec_idx) * IOVEC_STATUS_BITS)) &  \
                               IOVEC_ACCESSED_BIT)
#define SET_IOVEC_MAPPED(handle, iovec_idx)     \
    (((handle)->iovec_status) |= (IOVEC_MAPPED_BIT <<   \
                              ((iovec_idx) * IOVEC_STATUS_BITS)))
#define SET_IOVEC_UNMAPPED(handle, iovec_idx)   \
    (((handle)->iovec_status) |= (IOVEC_UNMAPPED_BIT << \
                              ((iovec_idx) * IOVEC_STATUS_BITS)))
#define SET_IOVEC_ACCESSED(handle, iovec_idx)   \
    (((handle)->iovec_status) |= (IOVEC_ACCESSED_BIT << \
                              ((iovec_idx) * IOVEC_STATUS_BITS)))

#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC */

void spm_handle_programmer_errors(psa_status_t status)
{
    if (status == PSA_ERROR_PROGRAMMER_ERROR ||
        status == PSA_ERROR_CONNECTION_REFUSED) {
        if (!tfm_spm_is_ns_caller()) {
            tfm_core_panic();
        }
    }
}

uint32_t tfm_spm_get_lifecycle_state(void)
{
    /*
     * FixMe: return PSA_LIFECYCLE_UNKNOWN to the caller directly. It will be
     * implemented in the future.
     */
    return PSA_LIFECYCLE_UNKNOWN;
}

/* PSA Partition API function body */

#if CONFIG_TFM_SPM_BACKEND_IPC == 1 \
    || CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1
psa_signal_t tfm_spm_partition_psa_wait(psa_signal_t signal_mask,
                                        uint32_t timeout)
{
    struct partition_t *partition = NULL;

    /*
     * Timeout[30:0] are reserved for future use.
     * SPM must ignore the value of RES.
     */
    timeout &= PSA_TIMEOUT_MASK;

    partition = GET_CURRENT_COMPONENT();

    /*
     * signals_allowed can be 0 for TF-M internal partitions for special usages.
     * Regular Secure Partitions should have at least one signal.
     * This is gauranteed by the manifest tool.
     * It is a PROGRAMMER ERROR if the signal_mask does not include any assigned
     * signals.
     */
    if ((partition->signals_allowed) &&
        (partition->signals_allowed & signal_mask) == 0) {
        tfm_core_panic();
    }

    /*
     * After new signal(s) are available, the return value will be updated in
     * PendSV and blocked thread gets to run.
     */
    if (timeout == PSA_BLOCK) {
        return backend_wait_signals(partition, signal_mask);
    } else {
        return partition->signals_asserted & signal_mask;
    }
}
#endif

#if CONFIG_TFM_SPM_BACKEND_IPC == 1
psa_status_t tfm_spm_partition_psa_get(psa_signal_t signal, psa_msg_t *msg)
{
    struct connection_t *handle = NULL;
    struct partition_t *partition = NULL;
    fih_int fih_rc = FIH_FAILURE;

    /*
     * Only one message could be retrieved every time for psa_get(). It is a
     * fatal error if the input signal has more than a signal bit set.
     */
    if (!IS_ONLY_ONE_BIT_IN_UINT32(signal)) {
        tfm_core_panic();
    }

    partition = GET_CURRENT_COMPONENT();

    /*
     * Write the message to the service buffer. It is a fatal error if the
     * input msg pointer is not a valid memory reference or not read-write.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             partition->boundary, (uintptr_t)msg,
             sizeof(psa_msg_t), TFM_HAL_ACCESS_READWRITE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the caller call psa_get() when no message has
     * been set. The caller must call this function after an RoT Service signal
     * is returned by psa_wait().
     */
    if (partition->signals_asserted == 0) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the RoT Service signal is not currently asserted.
     */
    if ((partition->signals_asserted & signal) == 0) {
        tfm_core_panic();
    }

    /*
     * Get message by signal from partition. It is a fatal error if getting
     * failed, which means the input signal is not correspond to an RoT service.
     */
    handle = spm_get_handle_by_signal(partition, signal);
    if (!handle) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    spm_memcpy(msg, &handle->msg, sizeof(psa_msg_t));

    return PSA_SUCCESS;
}
#endif

size_t tfm_spm_partition_psa_read(psa_handle_t msg_handle, uint32_t invec_idx,
                                  void *buffer, size_t num_bytes)
{
    size_t bytes;
    struct connection_t *handle = NULL;
    struct partition_t *curr_partition = GET_CURRENT_COMPONENT();
    fih_int fih_rc = FIH_FAILURE;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

#if PSA_FRAMEWORK_HAS_MM_IOVEC
    /*
     * It is a fatal error if the input vector has already been mapped using
     * psa_map_invec().
     */
    if (IOVEC_IS_MAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_ACCESSED(handle, (invec_idx + INVEC_IDX_BASE));
#endif

    /* There was no remaining data in this input vector */
    if (handle->msg.in_size[invec_idx] == 0) {
        return 0;
    }

    /*
     * Copy the client data to the service buffer. It is a fatal error
     * if the memory reference for buffer is invalid or not read-write.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)buffer,
             num_bytes, TFM_HAL_ACCESS_READWRITE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }

    bytes = num_bytes > handle->msg.in_size[invec_idx] ?
                        handle->msg.in_size[invec_idx] : num_bytes;

    spm_memcpy(buffer, handle->invec[invec_idx].base, bytes);

    /* There maybe some remaining data */
    handle->invec[invec_idx].base =
                                (char *)handle->invec[invec_idx].base + bytes;
    handle->msg.in_size[invec_idx] -= bytes;

    return bytes;
}

size_t tfm_spm_partition_psa_skip(psa_handle_t msg_handle, uint32_t invec_idx,
                                  size_t num_bytes)
{
    struct connection_t *handle = NULL;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

#if PSA_FRAMEWORK_HAS_MM_IOVEC
    /*
     * It is a fatal error if the input vector has already been mapped using
     * psa_map_invec().
     */
    if (IOVEC_IS_MAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_ACCESSED(handle, (invec_idx + INVEC_IDX_BASE));
#endif

    /* There was no remaining data in this input vector */
    if (handle->msg.in_size[invec_idx] == 0) {
        return 0;
    }

    /*
     * If num_bytes is greater than the remaining size of the input vector then
     * the remaining size of the input vector is used.
     */
    if (num_bytes > handle->msg.in_size[invec_idx]) {
        num_bytes = handle->msg.in_size[invec_idx];
    }

    /* There maybe some remaining data */
    handle->invec[invec_idx].base =
                            (char *)handle->invec[invec_idx].base + num_bytes;
    handle->msg.in_size[invec_idx] -= num_bytes;

    return num_bytes;
}

void tfm_spm_partition_psa_write(psa_handle_t msg_handle, uint32_t outvec_idx,
                                 const void *buffer, size_t num_bytes)
{
    struct connection_t *handle = NULL;
    struct partition_t *curr_partition = GET_CURRENT_COMPONENT();
    fih_int fih_rc = FIH_FAILURE;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if outvec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (outvec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the call attempts to write data past the end of
     * the client output vector
     */
    if (num_bytes > handle->msg.out_size[outvec_idx] -
        handle->outvec[outvec_idx].len) {
        tfm_core_panic();
    }

#if PSA_FRAMEWORK_HAS_MM_IOVEC
    /*
     * It is a fatal error if the output vector has already been mapped using
     * psa_map_outvec().
     */
    if (IOVEC_IS_MAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_ACCESSED(handle, (outvec_idx + OUTVEC_IDX_BASE));
#endif

    /*
     * Copy the service buffer to client outvecs. It is a fatal error
     * if the memory reference for buffer is invalid or not readable.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)buffer,
             num_bytes, TFM_HAL_ACCESS_READABLE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }

    spm_memcpy((char *)handle->outvec[outvec_idx].base +
               handle->outvec[outvec_idx].len, buffer, num_bytes);

    /* Update the write number */
    handle->outvec[outvec_idx].len += num_bytes;
}

psa_status_t tfm_spm_partition_psa_reply(psa_handle_t msg_handle,
                                         psa_status_t status)
{
    struct service_t *service;
    struct connection_t *handle;
    psa_status_t ret = PSA_SUCCESS;
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * RoT Service information is needed in this function, stored it in message
     * body structure. Only two parameters are passed in this function: handle
     * and status, so it is useful and simply to do like this.
     */
    service = handle->service;
    if (!service) {
        tfm_core_panic();
    }

    switch (handle->msg.type) {
    case PSA_IPC_CONNECT:
        /*
         * Reply to PSA_IPC_CONNECT message. Connect handle is returned if the
         * input status is PSA_SUCCESS. Others return values are based on the
         * input status.
         */
        if (status == PSA_SUCCESS) {
            ret = msg_handle;
        } else if (status == PSA_ERROR_CONNECTION_REFUSED) {
            /* Refuse the client connection, indicating a permanent error. */
            ret = PSA_ERROR_CONNECTION_REFUSED;
            handle->status = TFM_HANDLE_STATUS_TO_FREE;
        } else if (status == PSA_ERROR_CONNECTION_BUSY) {
            /* Fail the client connection, indicating a transient error. */
            ret = PSA_ERROR_CONNECTION_BUSY;
        } else {
            tfm_core_panic();
        }
        break;
    case PSA_IPC_DISCONNECT:
        /* Service handle is not used anymore */
        handle->status = TFM_HANDLE_STATUS_TO_FREE;

        /*
         * If the message type is PSA_IPC_DISCONNECT, then the status code is
         * ignored
         */
        break;
    default:
        if (handle->msg.type >= PSA_IPC_CALL) {

#if PSA_FRAMEWORK_HAS_MM_IOVEC

            /*
             * If the unmapped function is not called for an input/output vector
             * that has been mapped, the framework will remove the mapping.
             */
            int i;

            for (i = 0; i < PSA_MAX_IOVEC * 2; i++) {
                if (IOVEC_IS_MAPPED(handle, i) &&
                    (!IOVEC_IS_UNMAPPED(handle, i))) {
                    SET_IOVEC_UNMAPPED(handle, i);
                    /*
                     * Any output vectors that are still mapped will report that
                     * zero bytes have been written.
                     */
                    if (i >= OUTVEC_IDX_BASE) {
                        handle->outvec[i - OUTVEC_IDX_BASE].len = 0;
                    }
                }
            }

#endif
            /* Reply to a request message. Return values are based on status */
            ret = status;
            /*
             * The total number of bytes written to a single parameter must be
             * reported to the client by updating the len member of the
             * psa_outvec structure for the parameter before returning from
             * psa_call().
             */
            update_caller_outvec_len(handle);
            if (SERVICE_IS_STATELESS(service->p_ldinf->flags)) {
                handle->status = TFM_HANDLE_STATUS_TO_FREE;
            }
        } else {
            tfm_core_panic();
        }
    }

    if (ret == PSA_ERROR_PROGRAMMER_ERROR) {
        /*
         * If the source of the programmer error is a Secure Partition, the SPM
         * must panic the Secure Partition in response to a PROGRAMMER ERROR.
         */
        if (!TFM_CLIENT_ID_IS_NS(handle->msg.client_id)) {
            tfm_core_panic();
        }
    }

    /*
     * TODO: It can be optimized further by moving critical section protection
     * to mailbox. Also need to check implementation when secure context is
     * involved.
     */
    CRITICAL_SECTION_ENTER(cs_assert);
    ret = backend_replying(handle, ret);
    CRITICAL_SECTION_LEAVE(cs_assert);

    if (handle->status == TFM_HANDLE_STATUS_TO_FREE) {
        spm_free_connection(handle);
    } else {
        handle->status = TFM_HANDLE_STATUS_IDLE;
    }

    return ret;
}

#if CONFIG_TFM_DOORBELL_API == 1
void tfm_spm_partition_psa_notify(int32_t partition_id)
{
    struct partition_t *p_pt = tfm_spm_get_partition_by_id(partition_id);

    backend_assert_signal(p_pt, PSA_DOORBELL);
}

void tfm_spm_partition_psa_clear(void)
{
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;
    struct partition_t *partition = NULL;

    partition = GET_CURRENT_COMPONENT();

    /*
     * It is a fatal error if the Secure Partition's doorbell signal is not
     * currently asserted.
     */
    if ((partition->signals_asserted & PSA_DOORBELL) == 0) {
        tfm_core_panic();
    }

    CRITICAL_SECTION_ENTER(cs_assert);
    partition->signals_asserted &= ~PSA_DOORBELL;
    CRITICAL_SECTION_LEAVE(cs_assert);
}
#endif /* CONFIG_TFM_DOORBELL_API == 1 */

void tfm_spm_partition_psa_panic(void)
{
#ifdef CONFIG_TFM_HALT_ON_CORE_PANIC
    tfm_hal_system_halt();
#else
    /*
     * PSA FF recommends that the SPM causes the system to restart when a secure
     * partition panics.
     */
    tfm_hal_system_reset();
#endif
}

#if CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1
void tfm_spm_partition_psa_irq_enable(psa_signal_t irq_signal)
{
    struct partition_t *partition;
    const struct irq_load_info_t *irq_info;

    partition = GET_CURRENT_COMPONENT();

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    if (!irq_info) {
        tfm_core_panic();
    }

    tfm_hal_irq_enable(irq_info->source);
}

psa_irq_status_t tfm_spm_partition_psa_irq_disable(psa_signal_t irq_signal)
{
    struct partition_t *partition;
    const struct irq_load_info_t *irq_info;

    partition = GET_CURRENT_COMPONENT();

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    if (!irq_info) {
        tfm_core_panic();
    }

    tfm_hal_irq_disable(irq_info->source);

    return 1;
}

/* This API is only used for FLIH. */
#if CONFIG_TFM_FLIH_API == 1
void tfm_spm_partition_psa_reset_signal(psa_signal_t irq_signal)
{
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;
    const struct irq_load_info_t *irq_info;
    struct partition_t *partition;

    partition = GET_CURRENT_COMPONENT();

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    if (!irq_info) {
        tfm_core_panic();
    }

    if (!irq_info->flih_func) {
        /* This API is for FLIH IRQs only */
        tfm_core_panic();
    }

    if ((partition->signals_asserted & irq_signal) == 0) {
        /* The signal is not asserted */
        tfm_core_panic();
    }

    CRITICAL_SECTION_ENTER(cs_assert);
    partition->signals_asserted &= ~irq_signal;
    CRITICAL_SECTION_LEAVE(cs_assert);
}
#endif

/* This API is only used for SLIH. */
#if CONFIG_TFM_SLIH_API == 1
void tfm_spm_partition_psa_eoi(psa_signal_t irq_signal)
{
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;
    const struct irq_load_info_t *irq_info = NULL;
    struct partition_t *partition = NULL;

    partition = GET_CURRENT_COMPONENT();

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    /* It is a fatal error if passed signal is not an interrupt signal. */
    if (!irq_info) {
        tfm_core_panic();
    }

    if (irq_info->flih_func) {
        /* This API is for SLIH IRQs only */
        tfm_core_panic();
    }

    /* It is a fatal error if passed signal is not currently asserted */
    if ((partition->signals_asserted & irq_signal) == 0) {
        tfm_core_panic();
    }

    CRITICAL_SECTION_ENTER(cs_assert);
    partition->signals_asserted &= ~irq_signal;
    CRITICAL_SECTION_LEAVE(cs_assert);

    tfm_hal_irq_clear_pending(irq_info->source);
    tfm_hal_irq_enable(irq_info->source);
}
#endif
#endif /* CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1 */

#if PSA_FRAMEWORK_HAS_MM_IOVEC

const void *tfm_spm_partition_psa_map_invec(psa_handle_t msg_handle,
                                            uint32_t invec_idx)
{
    struct connection_t *handle;
    struct partition_t *partition = NULL;
    fih_int fih_rc = FIH_FAILURE;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    partition = handle->service->partition;

    /*
     * It is a fatal error if MM-IOVEC has not been enabled for the RoT
     * Service that received the message.
     */
    if (!SERVICE_ENABLED_MM_IOVEC(handle->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message.
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC.
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /* It is a fatal error if the input vector has length zero. */
    if (handle->msg.in_size[invec_idx] == 0) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the input vector has already been mapped using
     * psa_map_invec().
     */
    if (IOVEC_IS_MAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the input vector has already been accessed
     * using psa_read() or psa_skip().
     */
    if (IOVEC_IS_ACCESSED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the memory reference for the wrap input vector is
     * invalid or not readable.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             partition->boundary, (uintptr_t)handle->invec[invec_idx].base,
             handle->invec[invec_idx].len, TFM_HAL_ACCESS_READABLE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }

    SET_IOVEC_MAPPED(handle, (invec_idx + INVEC_IDX_BASE));

    return handle->invec[invec_idx].base;
}

void tfm_spm_partition_psa_unmap_invec(psa_handle_t msg_handle,
                                       uint32_t invec_idx)
{
    struct connection_t *handle;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if MM-IOVEC has not been enabled for the RoT
     * Service that received the message.
     */
    if (!SERVICE_ENABLED_MM_IOVEC(handle->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message.
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC.
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if The input vector has not been mapped by a call to
     * psa_map_invec().
     */
    if (!IOVEC_IS_MAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the input vector has already been unmapped by a
     * call to psa_unmap_invec().
     */
    if (IOVEC_IS_UNMAPPED(handle, (invec_idx + INVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_UNMAPPED(handle, (invec_idx + INVEC_IDX_BASE));
}

void *tfm_spm_partition_psa_map_outvec(psa_handle_t msg_handle,
                                       uint32_t outvec_idx)
{
    struct connection_t *handle;
    struct partition_t *partition = NULL;
    fih_int fih_rc = FIH_FAILURE;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    partition = handle->service->partition;

    /*
     * It is a fatal error if MM-IOVEC has not been enabled for the RoT
     * Service that received the message.
     */
    if (!SERVICE_ENABLED_MM_IOVEC(handle->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message.
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if outvec_idx is equal to or greater than
     * PSA_MAX_IOVEC.
     */
    if (outvec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /* It is a fatal error if the output vector has length zero. */
    if (handle->msg.out_size[outvec_idx] == 0) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the output vector has already been mapped using
     * psa_map_outvec().
     */
    if (IOVEC_IS_MAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the output vector has already been accessed
     * using psa_write().
     */
    if (IOVEC_IS_ACCESSED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the output vector is invalid or not read-write.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             partition->boundary, (uintptr_t)handle->outvec[outvec_idx].base,
             handle->outvec[outvec_idx].len, TFM_HAL_ACCESS_READWRITE);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        tfm_core_panic();
    }
    SET_IOVEC_MAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE));

    return handle->outvec[outvec_idx].base;
}

void tfm_spm_partition_psa_unmap_outvec(psa_handle_t msg_handle,
                                        uint32_t outvec_idx, size_t len)
{
    struct connection_t *handle;

    /* It is a fatal error if message handle is invalid */
    handle = spm_msg_handle_to_connection(msg_handle);
    if (!handle) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if MM-IOVEC has not been enabled for the RoT
     * Service that received the message.
     */
    if (!SERVICE_ENABLED_MM_IOVEC(handle->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message.
     */
    if (handle->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if outvec_idx is equal to or greater than
     * PSA_MAX_IOVEC.
     */
    if (outvec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if len is greater than the output vector size.
     */
    if (len > handle->msg.out_size[outvec_idx]) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if The output vector has not been mapped by a call to
     * psa_map_outvec().
     */
    if (!IOVEC_IS_MAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the output vector has already been unmapped by a
     * call to psa_unmap_outvec().
     */
    if (IOVEC_IS_UNMAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE))) {
        tfm_core_panic();
    }

    SET_IOVEC_UNMAPPED(handle, (outvec_idx + OUTVEC_IDX_BASE));

    /* Update the write number */
    handle->outvec[outvec_idx].len = len;
}

#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC */
