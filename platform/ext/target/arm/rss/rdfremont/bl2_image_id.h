/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BL2_IMAGE_ID_H__
#define __BL2_IMAGE_ID_H__

/*
 * ID of firmware images loaded by BL2
 * Images are loaded by BL2 in reverse order of ID
 * Cannot use enum as this is included in linker files.
 */
/* RSS firmware */
#define RSS_FIRMWARE_SECURE_ID       0
#define RSS_FIRMWARE_NON_SECURE_ID   1
/* Number of firmware loaded by BL2 */
#define RSS_FIRMWARE_COUNT           2

#endif /* __BL2_IMAGE_ID_H__ */
