/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_BUILTIN_KEY_IDS_H__
#define __PLATFORM_BUILTIN_KEY_IDS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The PSA driver location for TF-M builtin keys. Arbitrary within the
 * ranges documented at
 * https://armmbed.github.io/mbed-crypto/html/api/keys/lifetimes.html#c.psa_key_location_t
 */
#define TFM_BUILTIN_KEY_LOADER_KEY_LOCATION ((psa_key_location_t)0x800001)

/**
 * \brief The persistent key identifiers for TF-M builtin keys.
 *
 * The value of TFM_BUILTIN_KEY_ID_MIN (and therefore of the whole range) is
 * completely arbitrary except for being inside the PSA builtin keys range.
 *
 */
enum tfm_key_id_builtin_t {
    TFM_BUILTIN_KEY_ID_MIN = 0x7fff815Bu,
    TFM_BUILTIN_KEY_ID_HUK,
    TFM_BUILTIN_KEY_ID_IAK,
#ifdef TFM_PARTITION_DELEGATED_ATTESTATION
    TFM_BUILTIN_KEY_ID_DAK_SEED,
#endif /* TFM_PARTITION_DELEGATED_ATTESTATION */
    TFM_BUILTIN_KEY_ID_PLAT_SPECIFIC_MIN = 0x7FFF816Bu,
    TFM_BUILTIN_KEY_ID_HOST_S_ROTPK,
    TFM_BUILTIN_KEY_ID_HOST_NS_ROTPK,
    TFM_BUILTIN_KEY_ID_HOST_CCA_ROTPK,
    TFM_BUILTIN_KEY_ID_MAX = 0x7FFF817Bu,
};

#ifdef __cplusplus
}
#endif
#endif /* __PLATFORM_BUILTIN_KEY_IDS_H__ */
