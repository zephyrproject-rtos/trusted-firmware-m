/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLAT_CRYPTO_KEYS_H__
#define __TFM_PLAT_CRYPTO_KEYS_H__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       SoC.
 */

#include <stdint.h>
#include "tfm_plat_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Gets hardware unique key for encryption
 *
 * \param[out] key   Buf to store the key in
 * \param[in]  size  Size of the buffer
 *
 * \return Returns error code specified in \ref tfm_plat_err_t
 */
enum tfm_plat_err_t tfm_plat_get_crypto_huk(uint8_t *key, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PLAT_CRYPTO_KEYS_H__ */
