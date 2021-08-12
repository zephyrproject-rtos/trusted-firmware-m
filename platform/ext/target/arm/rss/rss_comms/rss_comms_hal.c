/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "rss_comms_hal.h"

#include <string.h>

#include "rss_comms.h"
#include "rss_comms_queue.h"
#include "mhu.h"
#include "device_definition.h"
#include "tfm_spm_log.h"
#include "tfm_pools.h"
#include "tfm_psa_call_pack.h"

TFM_POOL_DECLARE(req_pool, sizeof(struct client_request_t),
                 RSS_COMMS_MAX_CONCURRENT_REQ);

static enum tfm_plat_err_t initialize_mhu(void)
{
    enum mhu_error_t err;

    err = mhu_init_sender(&MHU_RSS_TO_AP_DEV);
    if (err != MHU_ERR_NONE) {
        SPMLOG_ERRMSGVAL("[COMMS] RSS to AP MHU driver init failed: ", err);
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = mhu_init_receiver(&MHU_AP_TO_RSS_DEV);
    if (err != MHU_ERR_NONE) {
        SPMLOG_ERRMSGVAL("[COMMS] AP to RSS MHU driver init failed: ", err);
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    SPMLOG_DBGMSG("[COMMS] MHU driver initialized successfully.\r\n");
    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t deserialise_message(struct client_request_t *req,
                                               uint8_t *message_buf,
                                               size_t message_len)
{
    uint32_t payload_size = 0;
    uint32_t i;
    size_t bytes_processed = 0;
    size_t in_len, out_len;

    /* Header */
    memcpy(&req->msg, message_buf, sizeof(req->msg));
    bytes_processed = sizeof(req->msg);

    if (req->msg.protocol_ver != 0) {
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    in_len = PARAM_UNPACK_IN_LEN(req->msg.ctrl_param);
    out_len = PARAM_UNPACK_OUT_LEN(req->msg.ctrl_param);

    /* Only support 4 iovecs */
    if (in_len + out_len > 4) {
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    /* Invecs */
    for (i = 0; i < in_len; ++i) {
        req->in_vec[i].base = req->payload_buf + payload_size;
        req->in_vec[i].len = req->msg.io_size[i];
        payload_size += req->msg.io_size[i];
    }

    /* Check input payload is not too big for request buffer */
    if (payload_size > sizeof(req->payload_buf)) {
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    /* Check input payload is not larger than message length */
    if (bytes_processed + payload_size > message_len) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    /* Copy payload into the buffer */
    memcpy(req->payload_buf, message_buf + bytes_processed, payload_size);

    /* Outvecs */
    for (i = 0; i < out_len; ++i) {
        req->out_vec[i].base = req->payload_buf + payload_size;
        req->out_vec[i].len = req->msg.io_size[in_len + i];
        payload_size += req->msg.io_size[in_len + i];
    }

    /* Check output payload is not too big for request buffer */
    if (payload_size > sizeof(req->payload_buf)) {
        return TFM_PLAT_ERR_UNSUPPORTED;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t serialise_reply(struct client_request_t *req,
                                           uint8_t *message_buf,
                                           size_t *message_len)
{
    size_t payload_size = 0;
    size_t len;
    size_t out_len;
    uint32_t i;

    len = sizeof(req->reply);
    memcpy(message_buf, &req->reply, len);
    payload_size += len;

    /* Outvecs */
    out_len = PARAM_UNPACK_OUT_LEN(req->msg.ctrl_param);
    for (i = 0; i < out_len; ++i) {
        len = req->reply.out_size[i];
        memcpy(message_buf + payload_size, req->out_vec[i].base, len);
        payload_size += len;
    }

    *message_len = payload_size;

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_multi_core_hal_receive(void)
{
    enum mhu_error_t mhu_err;
    enum tfm_plat_err_t err;
    uint8_t payload_buf[512] __aligned(4);
    size_t payload_size = sizeof(payload_buf);

    struct client_request_t *req = tfm_pool_alloc(req_pool);
    if (!req) {
        /* No free capacity, drop message */
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Receive complete message */
    mhu_err = mhu_receive_data(&MHU_AP_TO_RSS_DEV, payload_buf, &payload_size);
    if (mhu_err != MHU_ERR_NONE) {
        SPMLOG_DBGMSGVAL("[COMMS] MHU receive failed: ", mhu_err);
        err = TFM_PLAT_ERR_SYSTEM_ERR;
        goto free;
    }

    memset(req, 0, sizeof(*req));
    err = deserialise_message(req, payload_buf, payload_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        /* Deserialisation failed, drop message */
        SPMLOG_DBGMSGVAL("[COMMS] Deserialize message failed: ", err);
        goto free;
    }

    SPMLOG_DBGMSGVAL("[COMMS] Received message: seq_num=", req->msg.seq_num);

    /* TODO: Program DMA for out-band payload */

    if (queue_enqueue(req) != 0) {
        /* No queue capacity, drop message */
        err = TFM_PLAT_ERR_SYSTEM_ERR;
        goto free;
    }

    /* Message successfully received */
    /* FIXME: need to send a failure reply in error cases so client can abort */

    return TFM_PLAT_ERR_SUCCESS;

free:
    tfm_pool_free(req_pool, req);
    return err;
}

enum tfm_plat_err_t tfm_multi_core_hal_reply(struct client_request_t *req)
{
    enum tfm_plat_err_t err;
    enum mhu_error_t mhu_err;
    /* FIXME: The beginning of the payload buffer in client_request_t
     *   contains the input data. This area could be reused when serialising
     *   the reply message making the local reply_buf unnecessary.
     */
    uint8_t reply_buf[sizeof(req->reply)
                    + sizeof(req->payload_buf)] __aligned(4);
    size_t reply_size = sizeof(reply_buf);

    if (!is_valid_chunk_data_in_pool(req_pool, (uint8_t *)req)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* FIXME: Queue replies to avoid blocking execution */

    /* TODO: Program DMA for out-band payload */

    err = serialise_reply(req, reply_buf, &reply_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        SPMLOG_DBGMSGVAL("[COMMS] Serialize reply failed: ", err);
        goto free;
    }

    mhu_err = mhu_send_data(&MHU_RSS_TO_AP_DEV, reply_buf, reply_size);
    if (mhu_err != MHU_ERR_NONE) {
        SPMLOG_DBGMSGVAL("[COMMS] MHU send failed: ", mhu_err);
        err = TFM_PLAT_ERR_SYSTEM_ERR;
        goto free;
    }

    SPMLOG_DBGMSG("[COMMS] Sent reply\r\n");

free:
    tfm_pool_free(req_pool, req);
    return err;
}

enum tfm_plat_err_t tfm_multi_core_hal_init(void)
{
    int32_t spm_err;

    spm_err = tfm_pool_init(req_pool, POOL_BUFFER_SIZE(req_pool),
                            sizeof(struct client_request_t),
                            RSS_COMMS_MAX_CONCURRENT_REQ);
    if (spm_err) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return initialize_mhu();
}
