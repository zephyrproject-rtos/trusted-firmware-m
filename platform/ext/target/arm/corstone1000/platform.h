/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

typedef enum {
    PLATFORM_GPT_IMAGE = 0,
    PLATFORM_IMAGE_COUNT,
}platform_image_id_t;

/* Initialize io storage of the platform */
int32_t plat_io_storage_init(void);

/* Return an IO device handle and specification which can be used to access
 * an image. This has to be implemented for the GPT parser. */
int32_t plat_get_image_source(unsigned int image_id, uintptr_t *dev_handle,
                              uintptr_t *image_spec);

#endif /*__PLATFORM_H__*/
