/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HELPERS_H__
#define __HELPERS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "tfm_sst_defs.h"
#include "secure_fw/services/secure_storage/sst_asset_management.h"
#include "test_framework.h"

enum serial_color_t {
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENDA = 5,
    CYAN = 6,
    WHITE = 7,
};

/**
 * \brief Translates tfm_sst_err_t into a string.
 *
 * \param[in] err  tfm_sst_err_t error value.
 *
 * \return tfm_sst_err_t as string.
 */
const char *sst_err_to_str(enum tfm_sst_err_t err);

/**
 * \brief Translates asset permissions into a string.
 *
 * \param[in] permissions  Asset permissions value.
 *
 * \return asset permissions as string.
 */
const char *asset_perms_to_str(uint8_t permissions);

/**
 * \brief Sets the the text color in the serial port.
 *
 * \param[in] color_id  Serial foreground color.
 *
 * \return 0 if the message is send successfully, 1 otherwise.
 */
void printf_set_color(enum serial_color_t color_id);

#ifdef __cplusplus
}
#endif

#endif /* __HELPERS_H__ */
