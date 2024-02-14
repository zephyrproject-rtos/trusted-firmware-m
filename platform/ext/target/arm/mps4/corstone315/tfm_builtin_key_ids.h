/*
 * Copyright (c) 2022-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_BUILTIN_KEY_IDS_H__
#define __TFM_BUILTIN_KEY_IDS_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief The persistent key identifiers for TF-M builtin keys.
 *
 * The value of TFM_BUILTIN_KEY_ID_MIN (and therefore of the whole range) is
 * completely arbitrary except for being inside the PSA builtin keys range.
 *
 */
enum tfm_key_id_builtin_t {
    TFM_BUILTIN_KEY_ID_MIN = 0x7FFF815Bu,
    TFM_BUILTIN_KEY_ID_HUK,
    TFM_BUILTIN_KEY_ID_IAK,
    TFM_BUILTIN_KEY_ID_MAX = 0x7FFF817Bu,
};

#ifdef __cplusplus
}
#endif
#endif /* __TFM_BUILTIN_KEY_IDS_H__ */
