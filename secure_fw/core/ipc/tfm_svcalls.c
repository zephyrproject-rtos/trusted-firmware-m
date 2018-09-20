/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "psa_client.h"
#include "psa_service.h"
#include "tfm_svc.h"
#include "tfm_svcalls.h"

/************************* SVC handler for PSA Client APIs *******************/

uint32_t tfm_svcall_psa_framework_version(void)
{
    return PSA_FRAMEWORK_VERSION;
}

uint32_t tfm_svcall_psa_version(uint32_t *args, int32_t ns_caller)
{
    return PSA_VERSION_NONE;
}

psa_handle_t tfm_svcall_psa_connect(uint32_t *args, int32_t ns_caller)
{
    return PSA_NULL_HANDLE;
}

psa_status_t tfm_svcall_psa_call(uint32_t *args, int32_t ns_caller)
{
    return PSA_SUCCESS;
}

void tfm_svcall_psa_close(uint32_t *args, int32_t ns_caller)
{
}

/*********************** SVC handler for PSA Service APIs ********************/

/**
 * \brief SVC handler for \ref psa_wait.
 *
 * \param[in] args              Include all input arguments:
 *                              signal_mask, timeout.
 *
 * \retval >0                   At least one signal is asserted.
 * \retval 0                    No signals are asserted. This is only seen when
 *                              a polling timeout is used.
 */
static psa_signal_t tfm_svcall_psa_wait(uint32_t *args)
{
    return 0;
}

/**
 * \brief SVC handler for \ref psa_get.
 *
 * \param[in] args              Include all input arguments: signal, msg.
 *
 * \retval PSA_SUCCESS          Success, *msg will contain the delivered
 *                              message.
 * \retval PSA_ERR_NOMSG        Message could not be delivered.
 * \retval "Does not return"    The call is invalid because one or more of the
 *                              following are true:
 * \arg                           signal has more than a single bit set.
 * \arg                           signal does not correspond to a RoT Service.
 * \arg                           The RoT Service signal is not currently
 *                                asserted.
 * \arg                           The msg pointer provided is not a valid memory
 *                                reference.
 */
static psa_status_t tfm_svcall_psa_get(uint32_t *args)
{
    return PSA_SUCCESS;
}

/**
 * \brief SVC handler for \ref psa_set_rhandle.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, rhandle.
 *
 * \retval void                 Success, rhandle will be provided with all
 *                              subsequent messages delivered on this
 *                              connection.
 * \retval "Does not return"    msg_handle is invalid.
 */
static void tfm_svcall_psa_set_rhandle(uint32_t *args)
{
}

/**
 * \brief SVC handler for \ref psa_read.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, invec_idx, buffer, num_bytes.
 *
 * \retval >0                   Number of bytes copied.
 * \retval 0                    There was no remaining data in this input
 *                              vector.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a
 *                                \ref PSA_IPC_CALL message.
 * \arg                           invec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 * \arg                           the memory reference for buffer is invalid or
 *                                not writable.
 */
static size_t tfm_svcall_psa_read(uint32_t *args)
{
    return 0;
}

/**
 * \brief SVC handler for \ref psa_skip.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, invec_idx, num_bytes.
 *
 * \retval >0                   Number of bytes skipped.
 * \retval 0                    There was no remaining data in this input
 *                              vector.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a
 *                                \ref PSA_IPC_CALL message.
 * \arg                           invec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 */
static size_t tfm_svcall_psa_skip(uint32_t *args)
{
    return 0;
}

/**
 * \brief SVC handler for \ref psa_write.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, outvec_idx, buffer, num_bytes.
 *
 * \retval void                 Success
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           msg_handle is invalid.
 * \arg                           msg_handle does not refer to a
 *                                \ref PSA_IPC_CALL message.
 * \arg                           outvec_idx is equal to or greater than
 *                                \ref PSA_MAX_IOVEC.
 * \arg                           The memory reference for buffer is invalid.
 * \arg                           The call attempts to write data past the end
 *                                of the client output vector.
 */
static void tfm_svcall_psa_write(uint32_t *args)
{
}

/**
 * \brief SVC handler for \ref psa_reply.
 *
 * \param[in] args              Include all input arguments:
 *                              msg_handle, status.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                         msg_handle is invalid.
 * \arg                         An invalid status code is specified for the
 *                              type of message.
 */
static void tfm_svcall_psa_reply(uint32_t *args)
{
}

/**
 * \brief SVC handler for \ref psa_notify.
 *
 * \param[in] args              Include all input arguments: partition_id.
 *
 * \retval void                 Success.
 * \retval "Does not return"    partition_id does not correspond to a Secure
 *                              Partition.
 */
static void tfm_svcall_psa_notify(uint32_t *args)
{
}

/**
 * \brief SVC handler for \ref psa_clear.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The Secure Partition's doorbell signal is not
 *                              currently asserted.
 */
static void tfm_svcall_psa_clear(uint32_t *args)
{
}

/**
 * \brief SVC handler for \ref psa_eoi.
 *
 * \param[in] args              Include all input arguments: irq_signal.
 *
 * \retval void                 Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           irq_signal is not an interrupt signal.
 * \arg                           irq_signal indicates more than one signal.
 * \arg                           irq_signal is not currently asserted.
 */
static void tfm_svcall_psa_eoi(uint32_t *args)
{
}

int32_t SVC_Handler_IPC(tfm_svc_number_t svc_num, uint32_t *ctx)
{
    switch (svc_num) {
    case TFM_SVC_PSA_FRAMEWORK_VERSION:
        return tfm_svcall_psa_framework_version();
    case TFM_SVC_PSA_VERSION:
        return tfm_svcall_psa_version(ctx, 0);
    case TFM_SVC_PSA_CONNECT:
        return tfm_svcall_psa_connect(ctx, 0);
    case TFM_SVC_PSA_CALL:
        return tfm_svcall_psa_call(ctx, 0);
    case TFM_SVC_PSA_CLOSE:
        tfm_svcall_psa_close(ctx, 0);
        break;
    case TFM_SVC_PSA_WAIT:
        return tfm_svcall_psa_wait(ctx);
    case TFM_SVC_PSA_GET:
        return tfm_svcall_psa_get(ctx);
    case TFM_SVC_PSA_SET_RHANDLE:
        tfm_svcall_psa_set_rhandle(ctx);
        break;
    case TFM_SVC_PSA_READ:
        return tfm_svcall_psa_read(ctx);
    case TFM_SVC_PSA_SKIP:
        return tfm_svcall_psa_skip(ctx);
    case TFM_SVC_PSA_WRITE:
        tfm_svcall_psa_write(ctx);
        break;
    case TFM_SVC_PSA_REPLY:
        tfm_svcall_psa_reply(ctx);
        break;
    case TFM_SVC_PSA_NOTIFY:
        tfm_svcall_psa_notify(ctx);
        break;
    case TFM_SVC_PSA_CLEAR:
        tfm_svcall_psa_clear(ctx);
        break;
    case TFM_SVC_PSA_EOI:
        tfm_svcall_psa_eoi(ctx);
        break;
    default:
        break;
    }
    return PSA_SUCCESS;
}
