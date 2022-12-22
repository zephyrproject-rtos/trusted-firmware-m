/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "platform.h"

#include <stdint.h>

/* Return an IO device handle and specification which can be used to access
 * an image. This has to be implemented for the GPT parser. */
int32_t plat_get_image_source(unsigned int image_id, uintptr_t *dev_handle,
                              uintptr_t *image_spec) {
    (void)image_id;
    *dev_handle = NULL;
    *image_spec = NULL;
    return 0;
}
