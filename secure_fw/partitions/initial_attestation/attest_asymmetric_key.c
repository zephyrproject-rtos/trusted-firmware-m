/*
 * Copyright (c) 2019-2024, Arm Limited. All rights reserved.
 * Copyright (c) 2018-2019, Laurence Lundblade.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>

#include "attest_key.h"
#include "config_tfm.h"
#include "psa/crypto.h"
#include "t_cose/q_useful_buf.h"
#include "t_cose/t_cose_common.h"
#include "t_cose/t_cose_key.h"
#include "tfm_crypto_defs.h"

/**
 * The size of X and Y coordinate in 2 parameter style EC public
 * key. Format is as defined in [COSE (RFC 8152)]
 * (https://tools.ietf.org/html/rfc8152) and [SEC 1: Elliptic Curve
 * Cryptography](http://www.secg.org/sec1-v2.pdf).
 *
 * This size is well-known and documented in public standards.
 */
#define ECC_COORD_SIZE PSA_BITS_TO_BYTES(ATTEST_KEY_BITS)

#define MAX_ENCODED_COSE_KEY_SIZE \
    1 + /* 1 byte to encode map */ \
    2 + /* 2 bytes to encode key type */ \
    2 + /* 2 bytes to encode curve */ \
    2 * /* the X and Y coordinates + encoding */ \
        (ECC_COORD_SIZE + 1 + 2)

/* The instance id is:
 *     0x01 || sha256(COSE_Key(public_attest_key))
 */
#define INSTANCE_ID_SIZE (PSA_HASH_LENGTH(PSA_ALG_SHA_256) + 1)

/* Add UEID type byte 0x01 to the beginning */
static uint8_t instance_id_buf[INSTANCE_ID_SIZE] = { 0x01 };
static struct q_useful_buf attest_key_hash = { .ptr = instance_id_buf + 1,
                                               .len = 0};

/*!
 * \brief Static function to calculate the hash of the attestation public key.
          The public key is encoded as a CBOR serialized COSE_Key object.
 *
 * \return Returns error code as specified in \ref psa_attest_err_t
 */
static enum psa_attest_err_t compute_attest_key_hash(void)
{
    Q_USEFUL_BUF_MAKE_STACK_UB(public_key_buf, MAX_ENCODED_COSE_KEY_SIZE);
    struct t_cose_key attest_key;
    struct q_useful_buf_c cose_key;
    psa_status_t psa_res;
    enum t_cose_err_t cose_res;

    attest_key.key.handle = TFM_BUILTIN_KEY_ID_IAK;

    cose_res = t_cose_key_encode(attest_key,
                                 public_key_buf,
                                 &cose_key);
    if (cose_res != T_COSE_SUCCESS) {
        return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
    }

    psa_res = psa_hash_compute(PSA_ALG_SHA_256,
                               cose_key.ptr,
                               cose_key.len,
                               attest_key_hash.ptr,
                               PSA_HASH_LENGTH(PSA_ALG_SHA_256),
                               &attest_key_hash.len);
    if (psa_res != PSA_SUCCESS) {
        return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
    }

    return PSA_ATTEST_ERR_SUCCESS;
}

enum psa_attest_err_t
attest_get_instance_id(struct q_useful_buf_c *id_buf)
{
    if (id_buf == NULL) {
        return PSA_ATTEST_ERR_INVALID_INPUT;
    }

    if (attest_key_hash.len == 0) {
       /* Not yet computed */
        if (compute_attest_key_hash() != PSA_ATTEST_ERR_SUCCESS) {
            return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
        }
    }

    id_buf->ptr = instance_id_buf;
    id_buf->len = sizeof(instance_id_buf);

    return PSA_ATTEST_ERR_SUCCESS;
}

#if ATTEST_INCLUDE_COSE_KEY_ID

/* The key-id is:
 *      sha256(COSE_Key(public_attest_key))
 */
enum psa_attest_err_t
attest_get_initial_attestation_key_id(struct q_useful_buf_c *attest_key_id)
{
    if (attest_key_id == NULL) {
        return PSA_ATTEST_ERR_INVALID_INPUT;
    }

    if (attest_key_hash.len == 0) {
       /* Not yet computed */
        if (compute_attest_key_hash() != PSA_ATTEST_ERR_SUCCESS) {
            return PSA_ATTEST_ERR_CLAIM_UNAVAILABLE;
        }
    }

    attest_key_id->ptr = attest_key_hash.ptr;
    attest_key_id->len = attest_key_hash.len;

    return PSA_ATTEST_ERR_SUCCESS;
}
#endif /* ATTEST_INCLUDE_COSE_KEY_ID */
