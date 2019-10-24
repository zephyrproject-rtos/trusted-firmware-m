/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CRYPTO_HW_H__
#define __CRYPTO_HW_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \brief Initialize the CC312 crypto accelerator
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_init(void);

/**
 * \brief Deallocate the CC312 crypto accelerator
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_finish(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CRYPTO_HW_H__ */
