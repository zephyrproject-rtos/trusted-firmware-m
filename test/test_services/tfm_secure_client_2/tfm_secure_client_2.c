/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifdef TFM_PSA_API
#include "psa/service.h"
#include "psa_manifest/tfm_secure_client_2.h"
#else
#include "psa/client.h"
#endif

static psa_status_t secure_client_2_dispatch(int32_t id, const void *arg,
                                             size_t arg_len)
{
    switch (id) {
    default:
        return PSA_ERROR_PROGRAMMER_ERROR;
    }
}

#ifdef TFM_PSA_API
#define SECURE_CLIENT_2_MAX_ARG_LEN 8U

void tfm_secure_client_2_init(void)
{
    psa_msg_t msg;
    size_t len;
    char arg[SECURE_CLIENT_2_MAX_ARG_LEN] __attribute__((__aligned__(8)));

    while (1) {
        (void)psa_wait(TFM_SECURE_CLIENT_2_SIGNAL, PSA_BLOCK);
        if (psa_get(TFM_SECURE_CLIENT_2_SIGNAL, &msg) != PSA_SUCCESS) {
            continue;
        }
        switch (msg.type) {
        case PSA_IPC_CONNECT:
        case PSA_IPC_DISCONNECT:
            psa_reply(msg.handle, PSA_SUCCESS);
            break;
        default:
            len = psa_read(msg.handle, 0, arg, SECURE_CLIENT_2_MAX_ARG_LEN);
            psa_reply(msg.handle, secure_client_2_dispatch(msg.type, arg, len));
            break;
        }
    }
}
#else /* TFM_PSA_API */
psa_status_t tfm_secure_client_2_init(void)
{
    return PSA_SUCCESS;
}

psa_status_t tfm_secure_client_2_call(psa_invec *in_vec, size_t in_len,
                                      psa_outvec *out_vec, size_t out_len)
{
    int32_t id;

    (void)out_vec;

    if (in_len != 2 || out_len != 0 || in_vec[0].len != sizeof(id)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    id = *((int32_t *)in_vec[0].base);

    return secure_client_2_dispatch(id, in_vec[1].base, in_vec[1].len);
}
#endif /* TFM_PSA_API */
