/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform_sp.h"

#include "platform/include/tfm_platform_system.h"
#include "secure_fw/include/tfm_spm_services_api.h"

#ifdef TFM_PSA_API
#include "psa_manifest/tfm_platform.h"
#include "psa/client.h"
#include "psa/service.h"
#include "region_defs.h"

#define INPUT_BUFFER_SIZE  64
#define OUTPUT_BUFFER_SIZE 64

typedef enum tfm_platform_err_t (*plat_func_t)(const psa_msg_t *msg);
#endif

enum tfm_platform_err_t platform_sp_system_reset(void)
{
    /* Check if SPM allows the system reset */

    if (tfm_spm_request_reset_vote() != 0) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    /* FIXME: The system reset functionality is only supported in isolation
     *        level 1.
     */

    tfm_platform_hal_system_reset();

    return TFM_PLATFORM_ERR_SUCCESS;
}

#ifndef TFM_PSA_API

enum tfm_platform_err_t
platform_sp_ioctl(psa_invec  *in_vec,  uint32_t num_invec,
                  psa_outvec *out_vec, uint32_t num_outvec)
{
    void *input, *output;
    tfm_platform_ioctl_req_t request;

    if ((num_invec < 1) || (num_invec > 2) ||
        (num_outvec > 1) ||
        (in_vec[0].base == NULL) ||
        (in_vec[0].len != sizeof(tfm_platform_ioctl_req_t))) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    input = (num_invec == 1) ? NULL : &in_vec[1];
    output = out_vec;
    request = *((tfm_platform_ioctl_req_t *)in_vec[0].base);

    return tfm_platform_hal_ioctl(request, input, output);
}

#else /* TFM_PSA_API */

static enum tfm_platform_err_t
platform_sp_system_reset_ipc(const psa_msg_t *msg)
{
    (void)msg; /* unused parameter */

    return platform_sp_system_reset();
}

static enum tfm_platform_err_t
platform_sp_ioctl_ipc(const psa_msg_t *msg)
{
    void *input = NULL;
    void *output = NULL;
    psa_invec invec = {0};
    psa_outvec outvec = {0};
    uint8_t input_buffer[INPUT_BUFFER_SIZE] = {0};
    uint8_t output_buffer[OUTPUT_BUFFER_SIZE] = {0};
    tfm_platform_ioctl_req_t request = 0;
    enum tfm_platform_err_t ret = TFM_PLATFORM_ERR_SYSTEM_ERROR;
    int num = 0;
    uint32_t in_len = PSA_MAX_IOVEC;
    uint32_t out_len = PSA_MAX_IOVEC;

    while ((in_len > 0) && (msg->in_size[in_len - 1] == 0)) {
        in_len--;
    }

    while ((out_len > 0) && (msg->out_size[out_len - 1] == 0)) {
        out_len--;
    }

    if ((in_len < 1) || (in_len > 2) ||
        (out_len > 1)) {
        return TFM_PLATFORM_ERR_SYSTEM_ERROR;
    }

    num = psa_read(msg->handle, 0, &request, sizeof(request));
    if (num != sizeof(request)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (in_len > 1) {
        if (msg->in_size[1] > INPUT_BUFFER_SIZE) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
        num = psa_read(msg->handle, 1, &input_buffer, msg->in_size[1]);
        if (num != msg->in_size[1]) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
        invec.base = input_buffer;
        invec.len = msg->in_size[1];
        input = &invec;
    }

    if (out_len > 0) {
        if (msg->out_size[0] > OUTPUT_BUFFER_SIZE) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
        outvec.base = output_buffer;
        outvec.len = msg->out_size[0];
        output = &outvec;
    }

    ret = tfm_platform_hal_ioctl(request, input, output);

    if (output != NULL) {
        psa_write(msg->handle, 0, outvec.base, outvec.len);
    }

    return ret;
}

static void platform_signal_handle(psa_signal_t signal, plat_func_t pfn)
{
    psa_msg_t msg;
    psa_status_t status;

    status = psa_get(signal, &msg);
    switch (msg.type) {
    case PSA_IPC_CONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    case PSA_IPC_CALL:
        status = (psa_status_t)pfn(&msg);
        psa_reply(msg.handle, status);
        break;
    case PSA_IPC_DISCONNECT:
        psa_reply(msg.handle, PSA_SUCCESS);
        break;
    default:
        /* FIXME: Should be replaced by a call to psa_panic() when it
         * becomes available.
         */
        while (1) {
            ;
        }
    }
}

#endif /* TFM_PSA_API */

enum tfm_platform_err_t platform_sp_init(void)
{
#ifdef TFM_PSA_API
    psa_signal_t signals = 0;

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_SP_PLATFORM_SYSTEM_RESET_SIGNAL) {
            platform_signal_handle(TFM_SP_PLATFORM_SYSTEM_RESET_SIGNAL,
                                   platform_sp_system_reset_ipc);
        } else if (signals & TFM_SP_PLATFORM_IOCTL_SIGNAL) {
            platform_signal_handle(TFM_SP_PLATFORM_IOCTL_SIGNAL,
                                   platform_sp_ioctl_ipc);
        } else {
            /* FIXME: Should be replaced by a call to psa_panic() when it
             * becomes available.
             */
            while (1) {
               ;
            }
        }
    }

#endif /* TFM_PSA_API */

    return TFM_PLATFORM_ERR_SUCCESS;
}
