/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "psa/crypto.h"
#include "cc_ecpki_types.h"
#include "cc_rsa_types.h"

void psa_hash_mode_to_cc_hash_mode(psa_algorithm_t alg,
                                   bool performHashing, void *hash_mode)
{

    psa_algorithm_t hash_alg = PSA_ALG_SIGN_GET_HASH(alg);

    switch (hash_alg) {

    case PSA_ALG_SHA_1:

        if (PSA_ALG_IS_ECDSA(alg)) {
            *(CCEcpkiHashOpMode_t *)hash_mode =
                performHashing ? CC_ECPKI_HASH_SHA1_mode
                               : CC_ECPKI_AFTER_HASH_SHA1_mode;
        } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||
                   PSA_ALG_IS_RSA_PSS(alg)) {
            *(CCRsaHashOpMode_t *)hash_mode =
                performHashing ? CC_RSA_HASH_SHA1_mode : CC_RSA_After_SHA1_mode;
        }
        break;

    case PSA_ALG_SHA_224:

        if (PSA_ALG_IS_ECDSA(alg)) {
            *(CCEcpkiHashOpMode_t *)hash_mode =
                performHashing ? CC_ECPKI_HASH_SHA224_mode
                               : CC_ECPKI_AFTER_HASH_SHA224_mode;
        } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||
                   PSA_ALG_IS_RSA_PSS(alg)) {
            *(CCRsaHashOpMode_t *)hash_mode = performHashing
                                                  ? CC_RSA_HASH_SHA224_mode
                                                  : CC_RSA_After_SHA224_mode;
        }
        break;

    case PSA_ALG_SHA_256:

        if (PSA_ALG_IS_ECDSA(alg)) {
            *(CCEcpkiHashOpMode_t *)hash_mode =
                performHashing ? CC_ECPKI_HASH_SHA256_mode
                               : CC_ECPKI_AFTER_HASH_SHA256_mode;
        } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||
                   PSA_ALG_IS_RSA_PSS(alg)) {
            *(CCRsaHashOpMode_t *)hash_mode = performHashing
                                                  ? CC_RSA_HASH_SHA256_mode
                                                  : CC_RSA_After_SHA256_mode;
        }
        break;

    case PSA_ALG_SHA_384:

        if (PSA_ALG_IS_ECDSA(alg)) {
            *(CCEcpkiHashOpMode_t *)hash_mode =
                performHashing ? CC_ECPKI_HASH_SHA384_mode
                               : CC_ECPKI_AFTER_HASH_SHA384_mode;
        } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||
                   PSA_ALG_IS_RSA_PSS(alg)) {
            *(CCRsaHashOpMode_t *)hash_mode = performHashing
                                                  ? CC_RSA_HASH_SHA384_mode
                                                  : CC_RSA_After_SHA384_mode;
        }
        break;

    case PSA_ALG_SHA_512:

        if (PSA_ALG_IS_ECDSA(alg)) {
            *(CCEcpkiHashOpMode_t *)hash_mode =
                performHashing ? CC_ECPKI_HASH_SHA512_mode
                               : CC_ECPKI_AFTER_HASH_SHA512_mode;
        } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||
                   PSA_ALG_IS_RSA_PSS(alg)) {
            *(CCRsaHashOpMode_t *)hash_mode = performHashing
                                                  ? CC_RSA_HASH_SHA512_mode
                                                  : CC_RSA_After_SHA512_mode;
        }
        break;

    default:

        if (PSA_ALG_IS_ECDSA(alg)) {
            *(CCEcpkiHashOpMode_t *)hash_mode = CC_ECPKI_HASH_OpModeLast;
        } else if (PSA_ALG_IS_RSA_PKCS1V15_SIGN(alg) ||
                   PSA_ALG_IS_RSA_PSS(alg)) {
            *(CCRsaHashOpMode_t *)hash_mode = CC_RSA_HASH_OpModeLast;
        }
        break;
    }
}
