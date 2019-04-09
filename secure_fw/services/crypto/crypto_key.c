/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stddef.h>
#include <stdint.h>

/* FixMe: Use PSA_CONNECTION_REFUSED when performing parameter
 *        integrity checks but this will have to be revised
 *        when the full set of error codes mandated by PSA FF
 *        is available.
 */
#include "tfm_mbedcrypto_include.h"

#include "tfm_crypto_api.h"
#include "tfm_crypto_defs.h"

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
psa_status_t tfm_crypto_allocate_key(psa_invec in_vec[],
                                     size_t in_len,
                                     psa_outvec out_vec[],
                                     size_t out_len)
{
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_handle_t))) {
        return PSA_CONNECTION_REFUSED;
    }

    psa_key_handle_t *key_handle = out_vec[0].base;

    return psa_allocate_key(key_handle);
}

psa_status_t tfm_crypto_import_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    (void)out_vec;

    if ((in_len != 2) || (out_len != 0)) {
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_type_t type = iov->type;
    const uint8_t *data = in_vec[1].base;
    size_t data_length = in_vec[1].len;

    return psa_import_key(key, type, data, data_length);
}

psa_status_t tfm_crypto_destroy_key(psa_invec in_vec[],
                                    size_t in_len,
                                    psa_outvec out_vec[],
                                    size_t out_len)
{
    (void)out_vec;

    if ((in_len != 1) || (out_len != 0)) {
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;

    return psa_destroy_key(key);
}

psa_status_t tfm_crypto_get_key_information(psa_invec in_vec[],
                                            size_t in_len,
                                            psa_outvec out_vec[],
                                            size_t out_len)
{
    if ((in_len != 1) || (out_len != 2)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_type_t)) ||
        (out_vec[1].len != sizeof(size_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_type_t *type = out_vec[0].base;
    size_t *bits = out_vec[1].base;

    return psa_get_key_information(key, type, bits);
}

psa_status_t tfm_crypto_export_key(psa_invec in_vec[],
                                   size_t in_len,
                                   psa_outvec out_vec[],
                                   size_t out_len)
{
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if (in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    uint8_t *data = out_vec[0].base;
    size_t data_size = out_vec[0].len;
    size_t *data_length = &(out_vec[0].len);

    return psa_export_key(key, data, data_size, data_length);
}

psa_status_t tfm_crypto_export_public_key(psa_invec in_vec[],
                                          size_t in_len,
                                          psa_outvec out_vec[],
                                          size_t out_len)
{
    (void)in_vec;
    (void)in_len;
    (void)out_vec;
    (void)out_len;

    /* FIXME: This API is not supported yet */
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t tfm_crypto_set_key_policy(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
    (void)out_vec;

    if ((in_len != 2) || (out_len != 0)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (in_vec[1].len != sizeof(psa_key_policy_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    const psa_key_policy_t *policy = in_vec[1].base;

    return psa_set_key_policy(key, policy);
}

psa_status_t tfm_crypto_get_key_policy(psa_invec in_vec[],
                                       size_t in_len,
                                       psa_outvec out_vec[],
                                       size_t out_len)
{
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_policy_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_policy_t *policy = out_vec[0].base;

    return psa_get_key_policy(key, policy);
}

psa_status_t tfm_crypto_get_key_lifetime(psa_invec in_vec[],
                                         size_t in_len,
                                         psa_outvec out_vec[],
                                         size_t out_len)
{
    if ((in_len != 1) || (out_len != 1)) {
        return PSA_CONNECTION_REFUSED;
    }

    if ((in_vec[0].len != sizeof(struct tfm_crypto_pack_iovec)) ||
        (out_vec[0].len != sizeof(psa_key_lifetime_t))) {
        return PSA_CONNECTION_REFUSED;
    }
    const struct tfm_crypto_pack_iovec *iov = in_vec[0].base;

    psa_key_handle_t key = iov->key_handle;
    psa_key_lifetime_t *lifetime = out_vec[0].base;

    return psa_get_key_lifetime(key, lifetime);
}
/*!@}*/
