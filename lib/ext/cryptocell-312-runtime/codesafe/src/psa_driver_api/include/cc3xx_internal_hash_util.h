/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_INTERNAL_HASH_UTIL_H
#define CC3XX_INTERNAL_HASH_UTIL_H

#include "psa/crypto.h"

void psa_hash_mode_to_cc_hash_mode(psa_algorithm_t alg,
                                   bool performHashing,
                                   void *hash_mode);

#endif /* CC3XX_INTERNAL_HASH_UTIL_H */
