/*
 * Copyright (c) 2018 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/* Flash layout on Musca with BL2:
 * 0x0020_0000 BL2 - MCUBoot
 * 0x0021_0000 Flash_area_image_0:
 *    0x0021_0000 Secure     image primary
 *    0x0023_0000 Non-secure image primary
 * 0x0024_0000 Flash_area_image_1:
 *    0x0024_0000 Secure     image secondary
 *    0x0026_0000 Non-secure image secondary
 * 0x0027_0000 Scratch area
 */
#define FLASH_BASE_ADDRESS              (0x00200000)

#define FLASH_ALIGN                     (1)
#define FLASH_PARTITION_SIZE            (0x20000) /* 128KB */
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x1000)


#define FLASH_AREA_BL2_OFFSET           (0x0)
#ifdef BL2
#define FLASH_AREA_BL2_SIZE             (0x10000) /* 64KB */
#else
#define FLASH_AREA_BL2_SIZE             (0x0)
#endif
#define FLASH_AREA_IMAGE_SIZE           ((2 * FLASH_PARTITION_SIZE) -\
                                          FLASH_AREA_BL2_SIZE)

#define FLASH_AREA_IMAGE_0_OFFSET       (FLASH_AREA_BL2_OFFSET + \
                                         FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_IMAGE_0_SIZE         (FLASH_AREA_IMAGE_SIZE)

#define FLASH_AREA_IMAGE_1_OFFSET       (FLASH_AREA_IMAGE_0_OFFSET + \
                                         FLASH_AREA_IMAGE_0_SIZE)
#define FLASH_AREA_IMAGE_1_SIZE         (FLASH_AREA_IMAGE_SIZE)

#define FLASH_AREA_IMAGE_SCRATCH_OFFSET (FLASH_AREA_IMAGE_1_OFFSET + \
                                         FLASH_AREA_IMAGE_1_SIZE)
#define FLASH_AREA_IMAGE_SCRATCH_SIZE   (FLASH_AREA_IMAGE_SIZE)

/* Offset and size definition in flash area, used by assemble.py */
#define SECURE_IMAGE_OFFSET             0x0
#define SECURE_IMAGE_MAX_SIZE           0x20000

#define NON_SECURE_IMAGE_OFFSET         0x20000
#define NON_SECURE_IMAGE_MAX_SIZE       0x20000

#endif /* __FLASH_LAYOUT_H__ */
