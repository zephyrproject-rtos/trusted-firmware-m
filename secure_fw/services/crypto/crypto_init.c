/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "tfm_crypto_api.h"
#include "crypto_engine.h"

#ifdef TFM_PSA_API
#include "psa_service.h"
#include "tfm_crypto_signal.h"
#include "secure_fw/core/tfm_memory_utils.h"

/**
 * \brief Table containing all the Uniform Signature API exposed
 *        by the TF-M Crypto partition
 */
static const tfm_crypto_us_t sfid_func_table[TFM_CRYPTO_SFID_MAX] = {
    tfm_crypto_import_key,
    tfm_crypto_destroy_key,
    tfm_crypto_get_key_information,
    tfm_crypto_export_key,
    tfm_crypto_key_policy_init,
    tfm_crypto_key_policy_set_usage,
    tfm_crypto_key_policy_get_usage,
    tfm_crypto_key_policy_get_algorithm,
    tfm_crypto_set_key_policy,
    tfm_crypto_get_key_policy,
    tfm_crypto_set_key_lifetime,
    tfm_crypto_get_key_lifetime,
    tfm_crypto_cipher_set_iv,
    tfm_crypto_cipher_encrypt_setup,
    tfm_crypto_cipher_decrypt_setup,
    tfm_crypto_cipher_update,
    tfm_crypto_cipher_abort,
    tfm_crypto_cipher_finish,
    tfm_crypto_hash_setup,
    tfm_crypto_hash_update,
    tfm_crypto_hash_finish,
    tfm_crypto_hash_verify,
    tfm_crypto_hash_abort,
    tfm_crypto_mac_sign_setup,
    tfm_crypto_mac_verify_setup,
    tfm_crypto_mac_update,
    tfm_crypto_mac_sign_finish,
    tfm_crypto_mac_verify_finish,
    tfm_crypto_mac_abort,
    tfm_crypto_aead_encrypt,
    tfm_crypto_aead_decrypt
};

/**
 * \brief Aligns a value x up to an alignment a.
 */
#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

/**
 * \brief Maximum alignment required by any iovec parameters to the TF-M Crypto
 *        partition.
 */
#define TFM_CRYPTO_IOVEC_ALIGNMENT (4u)

/**
 * \brief Default size of the internal scratch buffer used for IOVec allocations
 *        in bytes
 */
#ifndef TFM_CRYPTO_IOVEC_BUFFER_SIZE
#define TFM_CRYPTO_IOVEC_BUFFER_SIZE (1024)
#endif

/**
 * \brief Internal scratch used for IOVec allocations
 *
 */
static struct tfm_crypto_scratch {
    __attribute__((__aligned__(TFM_CRYPTO_IOVEC_ALIGNMENT)))
    uint8_t buf[TFM_CRYPTO_IOVEC_BUFFER_SIZE];
    uint32_t alloc_index;
} scratch = {.buf = {0}, .alloc_index = 0};

static psa_status_t tfm_crypto_alloc_scratch(size_t requested_size, void **buf)
{
    /* Ensure alloc_index remains aligned to the required iovec alignment */
    requested_size = ALIGN(requested_size, TFM_CRYPTO_IOVEC_ALIGNMENT);

    if (requested_size > (sizeof(scratch.buf) - scratch.alloc_index)) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    /* Compute the pointer to the allocated space */
    *buf = (void *)&scratch.buf[scratch.alloc_index];

    /* Increase the allocated size */
    scratch.alloc_index += requested_size;

    return PSA_SUCCESS;
}

static psa_status_t tfm_crypto_clear_scratch(void)
{
    scratch.alloc_index = 0;
    (void)tfm_memset(scratch.buf, 0, sizeof(scratch.buf));

    return PSA_SUCCESS;
}

static psa_status_t tfm_crypto_call_sfn(psa_msg_t *msg,
                                        struct tfm_crypto_pack_iovec *iov,
                                        const uint32_t sfn_id)
{
    psa_status_t status = PSA_SUCCESS;
    size_t in_len = 0, out_len = 0, i, read_size;
    psa_invec in_vec[PSA_MAX_IOVEC] = { {0} };
    psa_outvec out_vec[PSA_MAX_IOVEC] = { {0} };
    void *alloc_buf_ptr = NULL;

    /* Check the number of in_vec filled */
    while ((in_len < PSA_MAX_IOVEC) && (msg->in_size[in_len] != 0)) {
        in_len++;
    }

    /* There will always be a tfm_crypto_pack_iovec in the first iovec */
    if (in_len < 1) {
        return PSA_ERROR_UNKNOWN_ERROR;
    }
    /* Initialise the first iovec with the IOV read when parsing */
    in_vec[0].base = iov;
    in_vec[0].len = sizeof(struct tfm_crypto_pack_iovec);

    /* Alloc/read from the second element as the first is read when parsing */
    for (i = 1; i < in_len; i++) {
        /* Allocate necessary space in the internal scratch */
        status = tfm_crypto_alloc_scratch(msg->in_size[i], &alloc_buf_ptr);
        if (status != PSA_SUCCESS) {
            return status;
        }
        /* Read from the IPC framework inputs into the scratch */
        read_size = psa_read(msg->handle, i, alloc_buf_ptr, msg->in_size[i]);
        /* Populate the fields of the input to the secure function */
        in_vec[i].base = alloc_buf_ptr;
        in_vec[i].len = msg->in_size[i];
    }

    /* Check the number of out_vec filled */
    while ((out_len < PSA_MAX_IOVEC) && (msg->out_size[out_len] != 0)) {
        out_len++;
    }

    for (i = 0; i < out_len; i++) {
        /* Allocate necessary space for the output in the internal scratch */
        status = tfm_crypto_alloc_scratch(msg->out_size[i], &alloc_buf_ptr);
        if (status != PSA_SUCCESS) {
            return status;
        }
        /* Populate the fields of the output to the secure function */
        out_vec[i].base = alloc_buf_ptr;
        out_vec[i].len = msg->out_size[i];
    }

    /* Call the uniform signature API */
    status = sfid_func_table[sfn_id](in_vec, in_len, out_vec, out_len);

    /* Write into the IPC framework outputs from the scratch */
    for (i = 0; i < out_len; i++) {
        psa_write(msg->handle, i, out_vec[i].base, out_vec[i].len);
    }

    /* Clear the allocated internal scratch before returning */
    if (tfm_crypto_clear_scratch() != PSA_SUCCESS) {
        return PSA_ERROR_UNKNOWN_ERROR;
    }

    return status;
}

static psa_status_t tfm_crypto_parse_msg(psa_msg_t *msg,
                                         struct tfm_crypto_pack_iovec *iov,
                                         uint32_t *sfn_id_p)
{
    size_t read_size;

    /* Read the in_vec[0] which holds the IOVEC always */
    read_size = psa_read(msg->handle,
                         0,
                         iov,
                         sizeof(struct tfm_crypto_pack_iovec));

    if (read_size != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_ERROR_UNKNOWN_ERROR;
    }

    if (iov->sfn_id >= TFM_CRYPTO_SFID_MAX) {
        *sfn_id_p = TFM_CRYPTO_SFID_INVALID;
        return PSA_ERROR_UNKNOWN_ERROR;
    }

    *sfn_id_p = iov->sfn_id;

    return PSA_SUCCESS;
}

static void tfm_crypto_ipc_handler(void)
{
    psa_signal_t signals = 0;
    psa_msg_t msg;
    psa_status_t status = PSA_SUCCESS;
    uint32_t sfn_id = TFM_CRYPTO_SFID_INVALID;
    struct tfm_crypto_pack_iovec iov = {0};

    while (1) {
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
        if (signals & TFM_CRYPTO_SIG) {
            /* Extract the message */
            if (psa_get(TFM_CRYPTO_SIG, &msg) != PSA_SUCCESS) {
                /* FIXME: Should be replaced by TF-M error handling */
                while (1) {
                    ;
                }
            }

            /* Process the message type */
            switch (msg.type) {
            case PSA_IPC_CONNECT:
            case PSA_IPC_DISCONNECT:
                psa_reply(msg.handle, PSA_SUCCESS);
                break;
            case PSA_IPC_CALL:
                /* Parse the message */
                status = tfm_crypto_parse_msg(&msg, &iov, &sfn_id);
                /* Call the dispatcher based on the SFID passed as type */
                if (sfn_id != TFM_CRYPTO_SFID_INVALID) {
                    status = tfm_crypto_call_sfn(&msg, &iov, sfn_id);
                } else {
                    status = PSA_ERROR_UNKNOWN_ERROR;
                }
                psa_reply(msg.handle, status);
                break;
            default:
                /* FIXME: Should be replaced by TF-M error handling */
                while (1) {
                    ;
                }
            }
        } else {
            /* FIXME: Should be replaced by TF-M error handling */
            while (1) {
               ;
            }
        }
    }

    /* This is unreachable */
    return;
}
#endif /* TFM_PSA_API */

static psa_status_t tfm_crypto_module_init(void)
{
    psa_status_t status = PSA_SUCCESS;

    /* Init the Key module */
    status = tfm_crypto_init_key();
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Init the Alloc module */
    return tfm_crypto_init_alloc();
}

psa_status_t tfm_crypto_init(void)
{
    psa_status_t status;

    /* Initialise other modules of the service */
    status = tfm_crypto_module_init();
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* Initialise the engine interface module */
    status = tfm_crypto_engine_init();
    if (status != PSA_SUCCESS) {
        return status;
    }

#ifdef TFM_PSA_API
    /* Should not return in normal operations */
    tfm_crypto_ipc_handler();
#endif

    return status;
}
