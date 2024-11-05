/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_PROVISIONING_MESSAGE_HANDLER_H__
#define __RSE_PROVISIONING_MESSAGE_HANDLER_H__

#include "rse_provisioning_message.h"
#include "tfm_plat_defs.h"
#include "rse_kmu_slot_ids.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct default_blob_handler_ctx_t {
    enum tfm_plat_err_t (*setup_aes_key)(const struct rse_provisioning_message_blob_t *, uint32_t *);
    enum tfm_plat_err_t (*get_rotpk)(uint32_t **, size_t *, uint32_t **, size_t *);
    bool blob_is_chainloaded;
};

enum tfm_plat_err_t default_blob_handler(const struct rse_provisioning_message_blob_t *blob,
                                         size_t msg_size, const void *ctx);

struct provisioning_message_handler_config {
    enum tfm_plat_err_t (*blob_handler)(const struct rse_provisioning_message_blob_t *, size_t, const void *);
    enum tfm_plat_err_t (*cert_handler)(const struct rse_provisioning_message_cert_t *, size_t, const void *);
    enum tfm_plat_err_t (*plain_data_handler)(const struct rse_provisioning_message_plain_t *, size_t, const void *);
};

enum tfm_plat_err_t handle_provisioning_message(const struct rse_provisioning_message_t *msg, size_t msg_size,
                                                struct provisioning_message_handler_config *config, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __RSE_PROVISIONING_MESSAGE_HANDLER_H__ */
