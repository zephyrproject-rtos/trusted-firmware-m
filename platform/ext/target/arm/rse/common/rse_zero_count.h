/*
 * SPDX-License-Identifier: BSD-3-Clause
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 */

#ifndef __RSE_ZERO_COUNT_H__
#define __RSE_ZERO_COUNT_H__

#include <stdint.h>
#include <stddef.h>
#include "tfm_plat_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

enum tfm_plat_err_t rse_count_zero_bits(uint8_t *buf, size_t buf_len,
                                        uint32_t *zero_count);

enum tfm_plat_err_t rse_check_zero_bit_count(uint8_t *buf, size_t buf_len,
                                             uint32_t expected_zero_count);

#ifdef __cplusplus
}
#endif

#endif /* __RSE_ZERO_COUNT_H__ */
