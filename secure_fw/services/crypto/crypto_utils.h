/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CRYPTO_UTILS_H__
#define __TFM_CRYPTO_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "tfm_secure_api.h"

/**
 * \brief Checks that a given type of access on a memory address with a
 *        a given size is allowed
 *
 * \param[in] addr   Base address to be checked
 * \param[in] size   Size of the buffer to checked
 * \param[in] access Type of access (1: READ ONLY, 2: READ/WRITE)
 *
 * \return Returns values as described in \ref tfm_crypto_err_t
 */
enum tfm_crypto_err_t tfm_crypto_memory_check(void *addr,
                                              uint32_t size,                                
                                              uint32_t access);
#ifdef __cplusplus
}
#endif

#endif /* __TFM_CRYPTO_UTILS_H__ */
