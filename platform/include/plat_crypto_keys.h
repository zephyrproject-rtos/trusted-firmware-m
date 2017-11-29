/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLAT_CRYPTO_KEYS__
#define __PLAT_CRYPTO_KEYS__
/**
 * \note The interfaces defined in this file must be implemented for each
 *       SoC.
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "plat_defs.h"

/**
 * \brief Gets hardware unique key for encryption
 *
 * \param[out] key   Buf to store the key in
 * \param[in]  size  Size of the buffer
 *
 * \return Returns error code specified in \ref tfm_plat_errno_t
 */
enum tfm_plat_errno_t plat_get_crypto_huk(uint8_t* key, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __PLAT_CRYPTO_KEYS__ */
