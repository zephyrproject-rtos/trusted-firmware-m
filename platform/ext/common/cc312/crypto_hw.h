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

/* The CC312 uses some the MBEDTLS heap buffer, so it needs increasing in size
 * to allow attest crypto operations to still function.
 */
#ifndef TFM_CRYPTO_ENGINE_BUF_SIZE
#define TFM_CRYPTO_ENGINE_BUF_SIZE (0x4000) /* 16KB for EC signing in attest */
#endif

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

/**
 * \brief Write the crypto keys to One-Time-Programmable memory
 *
 * The following keys will be provisioned:
 *  - Hardware Unique Key (HUK)
 *  - Hash of ROTPK
 *
 * \return 0 on success, non-zero otherwise
 */
int crypto_hw_accelerator_otp_provisioning(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CRYPTO_HW_H__ */
