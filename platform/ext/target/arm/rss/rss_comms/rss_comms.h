/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSS_COMMS_H__
#define __RSS_COMMS_H__

#include "psa/client.h"
#include "cmsis_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/* PSA client call packed into a message to send over MHU to SPE */
struct __PACKED packed_psa_call_t {
    uint8_t protocol_ver;
    uint8_t seq_num;
    uint16_t client_id;
    psa_handle_t handle;
    uint32_t ctrl_param; /* type, in_len, out_len */
    uint16_t io_size[4];
};

/* PSA reply packed into a message to send over MHU from SPE */
struct __PACKED packed_psa_reply_t {
    uint8_t protocol_ver;
    uint8_t seq_num;
    uint16_t client_id;
    int32_t return_val;
    uint16_t out_size[4];
};

/*
 * Allocated for each client request.
 *
 * TODO: Sizing of payload_buf, this should be platform dependent:
 * - sum in_vec size
 * - sum out_vec size
 */
struct client_request_t {
    struct packed_psa_call_t msg;
    struct packed_psa_reply_t reply;
    psa_invec in_vec[4];
    psa_outvec out_vec[4];
    uint8_t payload_buf[0x40 + 0x800]; /* size suits to get_attest_token() */
};

#ifdef __cplusplus
}
#endif

#endif /* __RSS_COMMS_H__ */
