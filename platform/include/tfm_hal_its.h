/*
 * Copyright (c) 2020, Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_HAL_ITS_H__
#define __TFM_HAL_ITS_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Retrieve the filesystem config for ITS.
 *
 * Note that this function should ensure that the values returned do
 * not result in a security compromise.
 *
 * \param [out] flash_area_addr  Location of the block of flash to use for ITS
 * \param [out] flash_area_size  Number of bytes of flash to use for ITS
 *
 * \return void
 * If an error is detected within this function, is should leave the
 * content of the parameters unchanged.
 */
void tfm_hal_its_fs_info(uint32_t *flash_area_addr, size_t *flash_area_size);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_HAL_ITS_H__ */
