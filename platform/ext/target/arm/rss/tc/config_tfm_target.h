/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Use stored NV seed to provide entropy */
#if CRYPTO_NV_SEED
#pragma message ("CRYPTO_NV_SEED is redefined to 0.")
#undef CRYPTO_NV_SEED
#endif
#define CRYPTO_NV_SEED                         0

/* Set the initial attestation token profile */
#if ATTEST_TOKEN_PROFILE_PSA_IOT_1
#pragma message ("ATTEST_TOKEN_PROFILE is redefined to ATTEST_TOKEN_PROFILE_ARM_CCA.")
#undef ATTEST_TOKEN_PROFILE_PSA_IOT_1
#elif ATTEST_TOKEN_PROFILE_PSA_2_0_0
#pragma message ("ATTEST_TOKEN_PROFILE is redefined to ATTEST_TOKEN_PROFILE_ARM_CCA.")
#undef ATTEST_TOKEN_PROFILE_PSA_2_0_0
#endif
#if !ATTEST_TOKEN_PROFILE_ARM_CCA
#pragma message ("ATTEST_TOKEN_PROFILE_ARM_CCA is chosen on RSS by default. Please check and set it explicitly.")
#undef ATTEST_TOKEN_PROFILE_ARM_CCA
#define ATTEST_TOKEN_PROFILE_ARM_CCA           1
#endif

#endif /* __CONFIG_TFM_TARGET_H__ */
