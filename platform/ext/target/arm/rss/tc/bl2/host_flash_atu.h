/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __HOST_FLASH_ATU_H__
#define __HOST_FLASH_ATU_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                  Performs ATU setup so that the given image can be
 *                         accessed within the host flash.
 *
 * \param[in]  image_id    The image id to setup the ATU regions for.
 *
 * \return                 0 if The regions have been setup successfully.
 */
int host_flash_atu_pre_load(uint32_t image_id);

/**
 * \brief                  Performs ATU teardown to disable the regions setup
 *                         for the particular image.
 *
 * \param[in]  image_id    The image id to teardown the ATU regions for.
 *
 * \return                 0 if The regions have been disabled successfully.
 */
int host_flash_atu_post_load(uint32_t image_id);

#ifdef __cplusplus
}
#endif

#endif /* __HOST_FLASH_ATU_H__ */
