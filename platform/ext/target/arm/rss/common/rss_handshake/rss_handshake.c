/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rss_handshake.h"
#include "device_definition.h"
#include "mhu.h"
#include "tfm_plat_otp.h"
#include "rss_key_derivation.h"
#include "rss_kmu_slot_ids.h"
#include "crypto.h"
#include "cc3xx_aes.h"
#include "cc3xx_rng.h"
#include "log.h"

enum rss_handshake_msg_type {
    RSS_HANDSHAKE_SESSION_KEY_MSG,
    RSS_HANDSHAKE_VHUK_MSG,
    RSS_HANDSHAKE_MAX_MSG = UINT32_MAX,
};

struct __attribute__((__packed__)) rss_handshake_header {
    enum rss_handshake_msg_type type;
    uint32_t rss_id;
    uint8_t ccm_iv[12];
};

struct  __attribute__((__packed__)) rss_handshake_trailer {
    uint8_t ccm_tag[16];
};

struct __attribute__((__packed__)) rss_handshake_msg {
    struct rss_handshake_header header;
    union __attribute__((__packed__)) {
        uint8_t vhuk_contribution[32];
        uint8_t session_key_iv[32];
    } body;
    struct rss_handshake_trailer trailer;
};

static int32_t rss_handshake_header_init(struct rss_handshake_msg *msg,
                                         enum rss_handshake_msg_type type)
{
    int32_t err;

    msg->header.type = type;

    err = tfm_plat_otp_read(PLAT_OTP_ID_RSS_ID,
                            sizeof(msg->header.rss_id),
                            (uint8_t*)&msg->header.rss_id);
    if (err != 0)
        return err;

    err = cc3xx_rng_get_random((uint8_t *)&msg->header.ccm_iv,
                               sizeof(msg->header.ccm_iv));
    if (err != 0) {
        return err;
    }

    return 0;
}

static int32_t rss_handshake_session_init(struct rss_handshake_msg *msg)
{
    int32_t err;

    err = rss_handshake_header_init(msg, RSS_HANDSHAKE_SESSION_KEY_MSG);
    if (err) {
        return err;
    }

    err = cc3xx_rng_get_random((uint8_t*)msg->body.session_key_iv,
                               sizeof(msg->body.session_key_iv));
    if (err != 0) {
        return err;
    }

    return 0;
}

static int32_t rss_handshake_vhuk_init(struct rss_handshake_msg *msg)
{
    int32_t err;
    size_t size;

    err = rss_handshake_header_init(msg, RSS_HANDSHAKE_VHUK_MSG);
    if (err) {
        return err;
    }

    err = rss_derive_vhuk_seed((uint32_t*)msg->body.vhuk_contribution,
                               sizeof(msg->body.vhuk_contribution),
                               &size);
    if (err) {
        return err;
    }

    return 0;
}

static int32_t rss_handshake_msg_crypt(cc3xx_aes_direction_t direction,
                                       struct rss_handshake_msg *msg)
{
    int32_t err;

    err = cc3xx_aes_init(direction, CC3XX_AES_MODE_CCM, RSS_KMU_SLOT_SESSION_KEY_0,
                         NULL, CC3XX_AES_KEYSIZE_256,
                         (uint32_t *)msg->header.ccm_iv, sizeof(msg->header.ccm_iv));
    if (err != 0) {
        return err;
    }

    cc3xx_aes_set_tag_len(sizeof(msg->trailer.ccm_tag));
    cc3xx_aes_set_data_len(sizeof(msg->body),
                           sizeof(msg->header));

    cc3xx_aes_update_authed_data((uint8_t *)msg,
                                 sizeof(msg->header));

    cc3xx_aes_set_output_buffer((uint8_t*)&msg->body,
                                sizeof(msg->body));

    err = cc3xx_aes_update((uint8_t*)&msg->body,
                           sizeof(msg->body));
    if (err != 0) {
        return err;
    }

    err = cc3xx_aes_finish((uint32_t*)&msg->trailer.ccm_tag,
                           sizeof(msg->trailer.ccm_tag));
    if (err != 0) {
        return err;
    }

    return 0;
}

static int32_t rss_handshake_msg_send(void *mhu_sender_dev,
                                      struct rss_handshake_msg *msg,
                                      bool crypt)
{
    int32_t err;

    err = mhu_init_sender(mhu_sender_dev);
    if (err != MHU_ERR_NONE && err != MHU_ERR_ALREADY_INIT) {
        return err;
    }

    if (crypt) {
        err = rss_handshake_msg_crypt(CC3XX_AES_DIRECTION_ENCRYPT, msg);
        if (err != 0) {
            return err;
        }
    }

    err = mhu_send_data(mhu_sender_dev,
                        (uint8_t*)msg,
                        sizeof(struct rss_handshake_msg));
    if (err != 0) {
        return err;
    }

    return 0;
}

static int32_t rss_handshake_msg_receive(void *mhu_receiver_dev,
                                         struct rss_handshake_msg *msg,
                                         bool crypt)
{
    int32_t err;
    size_t size;

    err = mhu_init_receiver(mhu_receiver_dev);
    if (err != MHU_ERR_NONE && err != MHU_ERR_ALREADY_INIT) {
        return err;
    }

    err = mhu_wait_data(mhu_receiver_dev);
    if (err != 0) {
        return err;
    }

    size = sizeof(struct rss_handshake_msg);
    err = mhu_receive_data(mhu_receiver_dev, (uint8_t*)msg, &size);
    if (err != 0) {
        return err;
    }

    if (crypt) {
        err = rss_handshake_msg_crypt(CC3XX_AES_DIRECTION_DECRYPT, msg);
        if (err != 0) {
            return err;
        }
    }

    return 0;
}

int32_t rss_handshake(void)
{
    int err;

    struct rss_handshake_msg session0;
    struct rss_handshake_msg msg0;

    struct rss_handshake_msg session1;
    struct rss_handshake_msg msg1;

    uint8_t session_seed[64];
    uint8_t vhuk_seed[64];

    err = rss_handshake_session_init(&session0);
    if (err != 0) {
        return err;
    }

    err = rss_handshake_vhuk_init(&msg0);
    if (err != 0) {
        return err;
    }

    if (msg0.header.rss_id == 0) {
        memcpy(session_seed, session0.body.session_key_iv,
               sizeof(session0.body.session_key_iv));

        err = rss_handshake_msg_send(&MHU_RSS_TO_RSS_SENDER_DEVS[msg0.header.rss_id],
                                     &session0, false);
        if (err != 0) {
            return err;
        }

        err = rss_handshake_msg_receive(&MHU_RSS_TO_RSS_RECEIVER_DEVS[msg0.header.rss_id],
                                        &session1, false);
        if (err != 0) {
            return err;
        }

        memcpy(session_seed + 32, session1.body.session_key_iv,
               sizeof(session1.body.session_key_iv));

        err = rss_derive_session_key((uint8_t*)session_seed,
                                     sizeof(session_seed),
                                     RSS_KMU_SLOT_SESSION_KEY_0);
        if (err) {
            return err;
        }

        memcpy(vhuk_seed, msg0.body.vhuk_contribution,
               sizeof(msg0.body.vhuk_contribution));

        err = rss_handshake_msg_send(&MHU_RSS_TO_RSS_SENDER_DEVS[msg0.header.rss_id],
                                     &msg0, true);
        if (err != 0) {
            return err;
        }

        err = rss_handshake_msg_receive(&MHU_RSS_TO_RSS_RECEIVER_DEVS[msg0.header.rss_id],
                                        &msg1, true);
        if (err != 0) {
            return err;
        }

        memcpy(vhuk_seed + 32, msg1.body.vhuk_contribution,
               sizeof(msg1.body.vhuk_contribution));
    }
    else if (msg0.header.rss_id == 1) {
        memcpy(session_seed + 32, session0.body.session_key_iv,
               sizeof(session0.body.session_key_iv));

        err = rss_handshake_msg_receive(&MHU_RSS_TO_RSS_RECEIVER_DEVS[msg0.header.rss_id],
                                        &session1, false);
        if (err != 0) {
            return err;
        }

        err = rss_handshake_msg_send(&MHU_RSS_TO_RSS_SENDER_DEVS[msg0.header.rss_id],
                                     &session0, false);
        if (err != 0) {
            return err;
        }

        memcpy(session_seed, session1.body.session_key_iv,
               sizeof(session1.body.session_key_iv));

        err = rss_derive_session_key((uint8_t*)session_seed,
                                     sizeof(session_seed),
                                     RSS_KMU_SLOT_SESSION_KEY_0);
        if (err) {
            return err;
        }

        memcpy(vhuk_seed + 32, msg0.body.vhuk_contribution,
               sizeof(msg0.body.vhuk_contribution));

        err = rss_handshake_msg_receive(&MHU_RSS_TO_RSS_RECEIVER_DEVS[msg0.header.rss_id],
                                        &msg1, true);
        if (err != 0) {
            return err;
        }

        err = rss_handshake_msg_send(&MHU_RSS_TO_RSS_SENDER_DEVS[msg0.header.rss_id],
                                     &msg0, true);
        if (err != 0) {
            return err;
        }

        memcpy(vhuk_seed, msg1.body.vhuk_contribution,
               sizeof(msg1.body.vhuk_contribution));
    }
    else {
        return -1;
    }

    err = rss_derive_vhuk((uint8_t *)vhuk_seed, sizeof(vhuk_seed), RSS_KMU_SLOT_VHUK);
    if (err) {
        return err;
    }

    BL1_LOG("[INF] vHUK generated and exported to KMU..\r\n");

    return 0;
}
