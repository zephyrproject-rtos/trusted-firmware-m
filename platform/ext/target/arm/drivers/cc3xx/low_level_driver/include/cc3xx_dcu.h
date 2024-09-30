/*
 * Copyright (c) 2024, The TrustedFirmware-M Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CC3XX_DCU_H
#define CC3XX_DCU_H

#include <stdint.h>
#include <stddef.h>
#include "cc3xx_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the status of the current debug permissions, i.e. host_dcu_en
 *
 * @param[out] val An array of 16 bytes to hold the full status of the host_dcu_en bits
 * @param[in]  len Length of \a val, must be 16 bytes
 *
 * @return cc3xx_err_t CC3XX_ERR_SUCCESS
 */
cc3xx_err_t cc3xx_dcu_get_enabled(uint8_t *val, size_t len);

/**
 * @brief Gets the status of the current debug locks, i.e. host_dcu_lock
 *
 * @param[out] val An array of 16 bytes to hold the full status of the host_dcu_lock bits
 * @param[in]  len Length of \a val, must be 16 bytes
 *
 * @return cc3xx_err_t CC3XX_ERR_SUCCESS
 */
cc3xx_err_t cc3xx_dcu_get_locked(uint8_t *val, size_t len);

/**
 * @brief Sets the debug locks, i.e. current value of host_dcu_lock
 *
 * @param[in] val An array of 16 bytes to hold the value of the debug locks to set
 * @param[in] len Length of \a val, must be 16 bytes
 *
 * @return cc3xx_err_t CC3XX_ERR_SUCCESS
 */
cc3xx_err_t cc3xx_dcu_set_locked(const uint8_t *val, size_t len);

/**
 * @brief Applies the Host DCU enable bits specified through \a permissions_mask
 *        of length \a len on the DCU. The length must always be equal to 16,
 *        which is the entire size of the host_dcu_en array, i.e. it can't be partial
 *
 * @param[in] permissions_mask Array containing the permissions bits to be applied
 * @param[in] len              Length of the \a permissions_mask array
 *
 * @return cc3xx_err_t CC3XX_ERR_SUCESS or CC3XX_ERR_DCU_LOCKED, CC3XX_ERR_DCU_MASK_MISMATCH
 */
cc3xx_err_t cc3xx_dcu_set_enabled(const uint8_t *permissions_mask, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* CC3XX_DCU_H */
