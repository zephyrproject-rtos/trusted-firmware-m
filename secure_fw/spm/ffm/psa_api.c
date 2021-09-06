/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "bitops.h"
#include "psa/lifecycle.h"
#include "psa/service.h"
#include "spm_ipc.h"
#include "tfm_arch.h"
#include "tfm_core_utils.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/interrupt_defs.h"
#include "psa_api.h"
#include "utilities.h"
#include "ffm/backend.h"
#include "ffm/spm_error_base.h"
#include "tfm_rpc.h"
#include "tfm_spm_hal.h"
#include "tfm_hal_interrupt.h"
#include "tfm_hal_platform.h"
#include "tfm_psa_call_param.h"

#define GET_STATELESS_SERVICE(index)    (stateless_services_ref_tbl[index])
extern struct service_t *stateless_services_ref_tbl[];

uint32_t tfm_spm_get_lifecycle_state(void)
{
    /*
     * FixMe: return PSA_LIFECYCLE_UNKNOWN to the caller directly. It will be
     * implemented in the future.
     */
    return PSA_LIFECYCLE_UNKNOWN;
}

/* PSA Client API function body */

uint32_t tfm_spm_client_psa_framework_version(void)
{
    return PSA_FRAMEWORK_VERSION;
}

uint32_t tfm_spm_client_psa_version(uint32_t sid)
{
    struct service_t *service;
    bool ns_caller = tfm_spm_is_ns_caller();

    /*
     * It should return PSA_VERSION_NONE if the RoT Service is not
     * implemented.
     */
    service = tfm_spm_get_service_by_sid(sid);
    if (!service) {
        return PSA_VERSION_NONE;
    }

    /*
     * It should return PSA_VERSION_NONE if the caller is not authorized
     * to access the RoT Service.
     */
    if (tfm_spm_check_authorization(sid, service, ns_caller) != SPM_SUCCESS) {
        return PSA_VERSION_NONE;
    }

    return service->p_ldinf->version;
}

psa_status_t tfm_spm_client_psa_connect(uint32_t sid, uint32_t version)
{
    struct service_t *service;
    struct tfm_msg_body_t *msg;
    struct tfm_conn_handle_t *connect_handle;
    int32_t client_id;
    psa_handle_t handle;
    bool ns_caller = tfm_spm_is_ns_caller();

    /*
     * It is a PROGRAMMER ERROR if the RoT Service does not exist on the
     * platform.
     */
    service = tfm_spm_get_service_by_sid(sid);
    if (!service) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_CONNECTION_REFUSED);
    }

    /* It is a PROGRAMMER ERROR if connecting to a stateless service. */
    if (SERVICE_IS_STATELESS(service->p_ldinf->flags)) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
    }

    /*
     * It is a PROGRAMMER ERROR if the caller is not authorized to access the
     * RoT Service.
     */
    if (tfm_spm_check_authorization(sid, service, ns_caller) != SPM_SUCCESS) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_CONNECTION_REFUSED);
    }

    /*
     * It is a PROGRAMMER ERROR if the version of the RoT Service requested is
     * not supported on the platform.
     */
    if (tfm_spm_check_client_version(service, version) != SPM_SUCCESS) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_CONNECTION_REFUSED);
    }

    client_id = tfm_spm_get_client_id(ns_caller);

    /*
     * Create connection handle here since it is possible to return the error
     * code to client when creation fails.
     */
    connect_handle = tfm_spm_create_conn_handle(service, client_id);
    if (!connect_handle) {
        return PSA_ERROR_CONNECTION_BUSY;
    }

    msg = tfm_spm_get_msg_buffer_from_conn_handle(connect_handle);
    if (!msg) {
        /* Have no enough resource to create message */
        return PSA_ERROR_CONNECTION_BUSY;
    }

    handle = tfm_spm_to_user_handle(connect_handle);
    /* No input or output needed for connect message */
    tfm_spm_fill_msg(msg, service, handle, PSA_IPC_CONNECT,
                     client_id, NULL, 0, NULL, 0, NULL);


    return backend_instance.messaging(service, msg);
}

psa_status_t tfm_spm_client_psa_call(psa_handle_t handle,
                                     uint32_t ctrl_param,
                                     const psa_invec *inptr,
                                     psa_outvec *outptr)
{
    psa_invec invecs[PSA_MAX_IOVEC];
    psa_outvec outvecs[PSA_MAX_IOVEC];
    struct tfm_conn_handle_t *conn_handle;
    struct service_t *service;
    struct tfm_msg_body_t *msg;
    int i, j;
    int32_t client_id;
    uint32_t sid, version, index;
    uint32_t privileged;
    bool ns_caller = tfm_spm_is_ns_caller();
    int32_t type = (int32_t)(int16_t)((ctrl_param & TYPE_MASK) >> TYPE_OFFSET);
    size_t in_num = (size_t)((ctrl_param & IN_LEN_MASK) >> IN_LEN_OFFSET);
    size_t out_num = (size_t)((ctrl_param & OUT_LEN_MASK) >> OUT_LEN_OFFSET);

    /* The request type must be zero or positive. */
    if (type < 0) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
    }

    /* It is a PROGRAMMER ERROR if in_len + out_len > PSA_MAX_IOVEC. */
    if ((in_num > PSA_MAX_IOVEC) ||
        (out_num > PSA_MAX_IOVEC) ||
        (in_num + out_num > PSA_MAX_IOVEC)) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
    }

    client_id = tfm_spm_get_client_id(ns_caller);

    /* Allocate space from handle pool for static handle. */
    if (IS_STATIC_HANDLE(handle)) {
        index = GET_INDEX_FROM_STATIC_HANDLE(handle);

        if (!IS_VALID_STATIC_HANDLE_IDX(index)) {
            TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
        }

        service = GET_STATELESS_SERVICE(index);
        if (!service) {
            tfm_core_panic();
        }

        sid = service->p_ldinf->sid;

        /*
         * It is a PROGRAMMER ERROR if the caller is not authorized to access
         * the RoT Service.
         */
        if (tfm_spm_check_authorization(sid, service, ns_caller)
            != SPM_SUCCESS) {
            TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_CONNECTION_REFUSED);
        }

        version = GET_VERSION_FROM_STATIC_HANDLE(handle);

        if (tfm_spm_check_client_version(service, version) != SPM_SUCCESS) {
            TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
        }

        conn_handle = tfm_spm_create_conn_handle(service, client_id);

        if (!conn_handle) {
            TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_CONNECTION_BUSY);
        }

        conn_handle->rhandle = NULL;
        handle = tfm_spm_to_user_handle(conn_handle);
    } else {
        conn_handle = tfm_spm_to_handle_instance(handle);

        /* It is a PROGRAMMER ERROR if an invalid handle was passed. */
        if (tfm_spm_validate_conn_handle(conn_handle, client_id)
            != SPM_SUCCESS) {
            TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
        }

        /*
         * It is a PROGRAMMER ERROR if the connection is currently
         * handling a request.
         */
        if (conn_handle->status == TFM_HANDLE_STATUS_ACTIVE) {
            TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
        }

        /*
         * Return PSA_ERROR_PROGRAMMER_ERROR immediately for the connection
         * has been terminated by the RoT Service.
         */
        if (conn_handle->status == TFM_HANDLE_STATUS_CONNECT_ERROR) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        service = conn_handle->service;
    }

    if (!service) {
        /* FixMe: Need to implement one mechanism to resolve this failure. */
        tfm_core_panic();
    }

    privileged = tfm_spm_get_caller_privilege_mode();

    /*
     * Read client invecs from the wrap input vector. It is a PROGRAMMER ERROR
     * if the memory reference for the wrap input vector is invalid or not
     * readable.
     */
    if (tfm_memory_check(inptr, in_num * sizeof(psa_invec), ns_caller,
        TFM_MEMORY_ACCESS_RO, privileged) != SPM_SUCCESS) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
    }

    /*
     * Read client outvecs from the wrap output vector and will update the
     * actual length later. It is a PROGRAMMER ERROR if the memory reference for
     * the wrap output vector is invalid or not read-write.
     */
    if (tfm_memory_check(outptr, out_num * sizeof(psa_outvec), ns_caller,
        TFM_MEMORY_ACCESS_RW, privileged) != SPM_SUCCESS) {
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
    }

    spm_memset(invecs, 0, sizeof(invecs));
    spm_memset(outvecs, 0, sizeof(outvecs));

    /* Copy the address out to avoid TOCTOU attacks. */
    spm_memcpy(invecs, inptr, in_num * sizeof(psa_invec));
    spm_memcpy(outvecs, outptr, out_num * sizeof(psa_outvec));

    /*
     * For client input vector, it is a PROGRAMMER ERROR if the provided payload
     * memory reference was invalid or not readable.
     */
    for (i = 0; i < in_num; i++) {
        if (tfm_memory_check(invecs[i].base, invecs[i].len, ns_caller,
            TFM_MEMORY_ACCESS_RO, privileged) != SPM_SUCCESS) {
            TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
        }
    }

    /*
     * Clients must never overlap input parameters because of the risk of a
     * double-fetch inconsistency.
     * Overflow is checked in tfm_memory_check functions.
     */
    for (i = 0; i + 1 < in_num; i++) {
        for (j = i+1; j < in_num; j++) {
            if (!((char *) invecs[j].base + invecs[j].len <=
                  (char *) invecs[i].base ||
                  (char *) invecs[j].base >=
                  (char *) invecs[i].base + invecs[i].len)) {
                TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
            }
        }
    }

    /*
     * For client output vector, it is a PROGRAMMER ERROR if the provided
     * payload memory reference was invalid or not read-write.
     */
    for (i = 0; i < out_num; i++) {
        if (tfm_memory_check(outvecs[i].base, outvecs[i].len,
            ns_caller, TFM_MEMORY_ACCESS_RW, privileged) != SPM_SUCCESS) {
            TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
        }
    }

    /*
     * FixMe: Need to check if the message is unrecognized by the RoT
     * Service or incorrectly formatted.
     */
    msg = tfm_spm_get_msg_buffer_from_conn_handle(conn_handle);
    if (!msg) {
        /* FixMe: Need to implement one mechanism to resolve this failure. */
        TFM_PROGRAMMER_ERROR(ns_caller, PSA_ERROR_PROGRAMMER_ERROR);
    }

    tfm_spm_fill_msg(msg, service, handle, type, client_id,
                     invecs, in_num, outvecs, out_num, outptr);

    return backend_instance.messaging(service, msg);
}

void tfm_spm_client_psa_close(psa_handle_t handle)
{
    struct service_t *service;
    struct tfm_msg_body_t *msg;
    struct tfm_conn_handle_t *conn_handle;
    int32_t client_id;
    bool ns_caller = tfm_spm_is_ns_caller();

    /* It will have no effect if called with the NULL handle */
    if (handle == PSA_NULL_HANDLE) {
        return;
    }

    /* It is a PROGRAMMER ERROR if called with a stateless handle. */
    if (IS_STATIC_HANDLE(handle)) {
        TFM_PROGRAMMER_ERROR(ns_caller, PROGRAMMER_ERROR_NULL);
    }

    client_id = tfm_spm_get_client_id(ns_caller);

    conn_handle = tfm_spm_to_handle_instance(handle);
    /*
     * It is a PROGRAMMER ERROR if an invalid handle was provided that is not
     * the null handle.
     */
    if (tfm_spm_validate_conn_handle(conn_handle, client_id) != SPM_SUCCESS) {
        TFM_PROGRAMMER_ERROR(ns_caller, PROGRAMMER_ERROR_NULL);
    }

    service = conn_handle->service;
    if (!service) {
        /* FixMe: Need to implement one mechanism to resolve this failure. */
        tfm_core_panic();
    }

    msg = tfm_spm_get_msg_buffer_from_conn_handle(conn_handle);
    if (!msg) {
        /* FixMe: Need to implement one mechanism to resolve this failure. */
        tfm_core_panic();
    }

    /*
     * It is a PROGRAMMER ERROR if the connection is currently handling a
     * request.
     */
    if (conn_handle->status == TFM_HANDLE_STATUS_ACTIVE) {
        TFM_PROGRAMMER_ERROR(ns_caller, PROGRAMMER_ERROR_NULL);
    }

    /* No input or output needed for close message */
    tfm_spm_fill_msg(msg, service, handle, PSA_IPC_DISCONNECT, client_id,
                     NULL, 0, NULL, 0, NULL);

    (void)backend_instance.messaging(service, msg);
}

/* PSA Partition API function body */

psa_signal_t tfm_spm_partition_psa_wait(psa_signal_t signal_mask,
                                        uint32_t timeout)
{
    struct partition_t *partition = NULL;

    /*
     * Timeout[30:0] are reserved for future use.
     * SPM must ignore the value of RES.
     */
    timeout &= PSA_TIMEOUT_MASK;

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    /*
     * It is a PROGRAMMER ERROR if the signal_mask does not include any assigned
     * signals.
     */
    if ((partition->signals_allowed & signal_mask) == 0) {
        tfm_core_panic();
    }

    /*
     * thrd_wait_on() blocks the caller thread if no signals are available.
     * In this case, the return value of this function is temporary set into
     * runtime context. After new signal(s) are available, the return value
     * is updated with the available signal(s) and blocked thread gets to run.
     */
    if (timeout == PSA_BLOCK &&
        (partition->signals_asserted & signal_mask) == 0) {
        partition->signals_waiting = signal_mask;
        thrd_wait_on(&partition->waitobj,
                     &(tfm_spm_get_running_partition()->thrd));
    } else if ((partition->signals_asserted & signal_mask) == 0) {
        /* Activate scheduler to check if any higher priority thread to run */
        tfm_arch_trigger_pendsv();
    }

    return partition->signals_asserted & signal_mask;
}

psa_status_t tfm_spm_partition_psa_get(psa_signal_t signal, psa_msg_t *msg)
{
    struct tfm_msg_body_t *tmp_msg = NULL;
    struct partition_t *partition = NULL;
    uint32_t privileged;

    /*
     * Only one message could be retrieved every time for psa_get(). It is a
     * fatal error if the input signal has more than a signal bit set.
     */
    if (!IS_ONLY_ONE_BIT_IN_UINT32(signal)) {
        tfm_core_panic();
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }
    privileged = tfm_spm_partition_get_privileged_mode(
        partition->p_ldinf->flags);

    /*
     * Write the message to the service buffer. It is a fatal error if the
     * input msg pointer is not a valid memory reference or not read-write.
     */
    if (tfm_memory_check(msg, sizeof(psa_msg_t), false, TFM_MEMORY_ACCESS_RW,
        privileged) != SPM_SUCCESS) {
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
    tmp_msg = tfm_spm_get_msg_by_signal(partition, signal);
    if (!tmp_msg) {
        return PSA_ERROR_DOES_NOT_EXIST;
    }

    (TO_CONTAINER(tmp_msg,
                  struct tfm_conn_handle_t,
                  internal_msg))->status = TFM_HANDLE_STATUS_ACTIVE;

    spm_memcpy(msg, &tmp_msg->msg, sizeof(psa_msg_t));

    return PSA_SUCCESS;
}

void tfm_spm_partition_psa_set_rhandle(psa_handle_t msg_handle, void *rhandle)
{
    struct tfm_msg_body_t *msg = NULL;
    struct tfm_conn_handle_t *conn_handle;

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    /* It is a PROGRAMMER ERROR if a stateless service sets rhandle. */
    if (SERVICE_IS_STATELESS(msg->service->p_ldinf->flags)) {
        tfm_core_panic();
    }

    msg->msg.rhandle = rhandle;
    conn_handle = tfm_spm_to_handle_instance(msg_handle);

    /* Store reverse handle for following client calls. */
    tfm_spm_set_rhandle(msg->service, conn_handle, rhandle);
}

size_t tfm_spm_partition_psa_read(psa_handle_t msg_handle, uint32_t invec_idx,
                                  void *buffer, size_t num_bytes)
{
    size_t bytes;
    struct tfm_msg_body_t *msg = NULL;
    uint32_t privileged;
    struct partition_t *partition = NULL;

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    partition = msg->service->partition;
    privileged = tfm_spm_partition_get_privileged_mode(
        partition->p_ldinf->flags);

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (msg->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /* There was no remaining data in this input vector */
    if (msg->msg.in_size[invec_idx] == 0) {
        return 0;
    }

    /*
     * Copy the client data to the service buffer. It is a fatal error
     * if the memory reference for buffer is invalid or not read-write.
     */
    if (tfm_memory_check(buffer, num_bytes, false,
        TFM_MEMORY_ACCESS_RW, privileged) != SPM_SUCCESS) {
        tfm_core_panic();
    }

    bytes = num_bytes > msg->msg.in_size[invec_idx] ?
                        msg->msg.in_size[invec_idx] : num_bytes;

    spm_memcpy(buffer, msg->invec[invec_idx].base, bytes);

    /* There maybe some remaining data */
    msg->invec[invec_idx].base = (char *)msg->invec[invec_idx].base + bytes;
    msg->msg.in_size[invec_idx] -= bytes;

    return bytes;
}

size_t tfm_spm_partition_psa_skip(psa_handle_t msg_handle, uint32_t invec_idx,
                                  size_t num_bytes)
{
    struct tfm_msg_body_t *msg = NULL;

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (msg->msg.type < PSA_IPC_CALL) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if invec_idx is equal to or greater than
     * PSA_MAX_IOVEC
     */
    if (invec_idx >= PSA_MAX_IOVEC) {
        tfm_core_panic();
    }

    /* There was no remaining data in this input vector */
    if (msg->msg.in_size[invec_idx] == 0) {
        return 0;
    }

    /*
     * If num_bytes is greater than the remaining size of the input vector then
     * the remaining size of the input vector is used.
     */
    if (num_bytes > msg->msg.in_size[invec_idx]) {
        num_bytes = msg->msg.in_size[invec_idx];
    }

    /* There maybe some remaining data */
    msg->invec[invec_idx].base = (char *)msg->invec[invec_idx].base +
                                 num_bytes;
    msg->msg.in_size[invec_idx] -= num_bytes;

    return num_bytes;
}

void tfm_spm_partition_psa_write(psa_handle_t msg_handle, uint32_t outvec_idx,
                                 const void *buffer, size_t num_bytes)
{
    struct tfm_msg_body_t *msg = NULL;
    uint32_t privileged;
    struct partition_t *partition = NULL;

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    partition = msg->service->partition;
    privileged = tfm_spm_partition_get_privileged_mode(
        partition->p_ldinf->flags);

    /*
     * It is a fatal error if message handle does not refer to a request
     * message
     */
    if (msg->msg.type < PSA_IPC_CALL) {
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
    if (num_bytes > msg->msg.out_size[outvec_idx] -
        msg->outvec[outvec_idx].len) {
        tfm_core_panic();
    }

    /*
     * Copy the service buffer to client outvecs. It is a fatal error
     * if the memory reference for buffer is invalid or not readable.
     */
    if (tfm_memory_check(buffer, num_bytes, false,
        TFM_MEMORY_ACCESS_RO, privileged) != SPM_SUCCESS) {
        tfm_core_panic();
    }

    spm_memcpy((char *)msg->outvec[outvec_idx].base +
               msg->outvec[outvec_idx].len, buffer, num_bytes);

    /* Update the write number */
    msg->outvec[outvec_idx].len += num_bytes;
}

void tfm_spm_partition_psa_reply(psa_handle_t msg_handle, psa_status_t status)
{
    struct service_t *service = NULL;
    struct tfm_msg_body_t *msg = NULL;
    int32_t ret = PSA_SUCCESS;
    struct tfm_conn_handle_t *conn_handle;

    /* It is a fatal error if message handle is invalid */
    msg = tfm_spm_get_msg_from_handle(msg_handle);
    if (!msg) {
        tfm_core_panic();
    }

    /*
     * RoT Service information is needed in this function, stored it in message
     * body structure. Only two parameters are passed in this function: handle
     * and status, so it is useful and simply to do like this.
     */
    service = msg->service;
    if (!service) {
        tfm_core_panic();
    }

    /*
     * Three type of message are passed in this function: CONNECTION, REQUEST,
     * DISCONNECTION. It needs to process differently for each type.
     */
    conn_handle = tfm_spm_to_handle_instance(msg_handle);
    switch (msg->msg.type) {
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
            tfm_spm_free_conn_handle(service, conn_handle);
            ret = PSA_ERROR_CONNECTION_REFUSED;
        } else if (status == PSA_ERROR_CONNECTION_BUSY) {
            /* Fail the client connection, indicating a transient error. */
            ret = PSA_ERROR_CONNECTION_BUSY;
        } else {
            tfm_core_panic();
        }
        break;
    case PSA_IPC_DISCONNECT:
        /* Service handle is not used anymore */
        tfm_spm_free_conn_handle(service, conn_handle);

        /*
         * If the message type is PSA_IPC_DISCONNECT, then the status code is
         * ignored
         */
        break;
    default:
        if (msg->msg.type >= PSA_IPC_CALL) {
            /* Reply to a request message. Return values are based on status */
            ret = status;
            /*
             * The total number of bytes written to a single parameter must be
             * reported to the client by updating the len member of the
             * psa_outvec structure for the parameter before returning from
             * psa_call().
             */
            update_caller_outvec_len(msg);
            if (SERVICE_IS_STATELESS(service->p_ldinf->flags)) {
                tfm_spm_free_conn_handle(service, conn_handle);
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
        if (TFM_CLIENT_ID_IS_NS(msg->msg.client_id)) {
            conn_handle->status = TFM_HANDLE_STATUS_CONNECT_ERROR;
        } else {
            tfm_core_panic();
        }
    } else {
        conn_handle->status = TFM_HANDLE_STATUS_IDLE;
    }

    if (is_tfm_rpc_msg(msg)) {
        tfm_rpc_client_call_reply(msg, ret);
    } else {
        thrd_wake_up(&msg->ack_evnt, ret);
    }
}

void tfm_spm_partition_psa_notify(int32_t partition_id)
{
    struct partition_t *p_pt = tfm_spm_get_partition_by_id(partition_id);

    spm_assert_signal(p_pt, PSA_DOORBELL);
}

void tfm_spm_partition_psa_clear(void)
{
    struct partition_t *partition = NULL;

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if the Secure Partition's doorbell signal is not
     * currently asserted.
     */
    if ((partition->signals_asserted & PSA_DOORBELL) == 0) {
        tfm_core_panic();
    }
    partition->signals_asserted &= ~PSA_DOORBELL;
}

void tfm_spm_partition_psa_eoi(psa_signal_t irq_signal)
{
    struct irq_load_info_t *irq_info = NULL;
    struct partition_t *partition = NULL;

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    /* It is a fatal error if passed signal is not an interrupt signal. */
    if (!irq_info) {
        tfm_core_panic();
    }

    if (irq_info->flih_func) {
        /* This API is for SLIH IRQs only */
        psa_panic();
    }

    /* It is a fatal error if passed signal is not currently asserted */
    if ((partition->signals_asserted & irq_signal) == 0) {
        tfm_core_panic();
    }

    partition->signals_asserted &= ~irq_signal;

    tfm_hal_irq_clear_pending(irq_info->source);
    tfm_hal_irq_enable(irq_info->source);
}

void tfm_spm_partition_psa_panic(void)
{
    /*
     * PSA FF recommends that the SPM causes the system to restart when a secure
     * partition panics.
     */
    tfm_hal_system_reset();
}

void tfm_spm_partition_irq_enable(psa_signal_t irq_signal)
{
    struct partition_t *partition;
    struct irq_load_info_t *irq_info;

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    if (!irq_info) {
        tfm_core_panic();
    }

    tfm_hal_irq_enable(irq_info->source);
}

psa_irq_status_t tfm_spm_partition_irq_disable(psa_signal_t irq_signal)
{
    struct partition_t *partition;
    struct irq_load_info_t *irq_info;

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    irq_info = get_irq_info_for_signal(partition->p_ldinf, irq_signal);
    if (!irq_info) {
        tfm_core_panic();
    }

    tfm_hal_irq_disable(irq_info->source);

    return 1;
}

void tfm_spm_partition_psa_reset_signal(psa_signal_t irq_signal)
{
    struct irq_load_info_t *irq_info;
    struct partition_t *partition;

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

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

    partition->signals_asserted &= ~irq_signal;
}
