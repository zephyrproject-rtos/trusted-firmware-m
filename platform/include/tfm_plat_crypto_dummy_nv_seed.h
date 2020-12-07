/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_PLAT_CRYPTO_DUMMY_NV_SEED_H__
#define __TFM_PLAT_CRYPTO_DUMMY_NV_SEED_H__

#include "tfm_plat_crypto_nv_seed.h"

/**
 * \brief Create a dummy entropy seed when platform doesn't provision the seed.
 *        Do NOT call this function in production.
 *
 * \return Return TFM_CRYPTO_NV_SEED_SUCCESS on success,
 *         or TFM_CRYPTO_NV_SEED_FAILED on failure.
 */
int tfm_plat_crypto_create_entropy_seed(void);

#endif /* __TFM_PLAT_CRYPTO_DUMMY_NV_SEED_H__ */
