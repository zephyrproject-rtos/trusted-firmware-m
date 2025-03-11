/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 */

#ifndef __RSE_ROTPK_POLICY_H__
#define __RSE_ROTPK_POLICY_H__

#ifdef __cplusplus
extern "C" {
#endif

enum rse_rotpk_type {
    RSE_ROTPK_TYPE_ECDSA = 0b00,
    RSE_ROTPK_TYPE_LMS   = 0b01,
};

enum rse_rotpk_policy {
    RSE_ROTPK_POLICY_SIG_OPTIONAL = 0b00,
    RSE_ROTPK_POLICY_SIG_REQUIRED = 0b01,
};

enum rse_rotpk_hash_alg {
    RSE_ROTPK_HASH_ALG_SHA256 = 0b00,
    RSE_ROTPK_HASH_ALG_SHA384 = 0b01,
};

#define RSE_ROTPK_SIZE_FROM_ALG(_alg) ((_alg) == RSE_ROTPK_HASH_ALG_SHA256 ? 32 : \
                                       (_alg) == RSE_ROTPK_HASH_ALG_SHA384 ? 48 : \
                                       0)

/* Currently only support up to SHA384 */
#define RSE_ROTPK_MAX_SIZE (RSE_ROTPK_SIZE_FROM_ALG(RSE_ROTPK_HASH_ALG_SHA384))

#ifdef __cplusplus
}
#endif

#endif /* __RSE_ROTPK_POLICY_H__ */
