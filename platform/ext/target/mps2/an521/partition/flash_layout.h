/*
 * Copyright (c) 2017 Arm Limited. All rights reserved.
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

/* Flash layout on MPS2 AN521 with BL2:
 *
 * 0x0000_0000 BL2 - MCUBoot
 * 0x0008_0000 Flash_area_image_0:
 *    0x0008_0000 Secure     image primary
 *    0x0010_0000 Non-secure image primary
 * 0x0018_0000 Flash_area_image_1:
 *    0x0018_0000 Secure     image secondary
 *    0x0020_0000 Non-secure image secondary
 * 0x0028_0000 Scratch area
 */
#define FLASH_BASE_ADDRESS              (0x0)

#define FLASH_ALIGN                     (1)
#define FLASH_PARTITION_SIZE            (0x80000)
#define FLASH_AREA_IMAGE_SECTOR_SIZE    (0x4000)


#define FLASH_AREA_BL2_OFFSET           (0x0)
#define FLASH_AREA_BL2_SIZE             (FLASH_PARTITION_SIZE)

#define FLASH_AREA_IMAGE_0_OFFSET       (0x080000)
#define FLASH_AREA_IMAGE_0_SIZE         (2 * FLASH_PARTITION_SIZE)

#define FLASH_AREA_IMAGE_1_OFFSET       (0x180000)
#define FLASH_AREA_IMAGE_1_SIZE         (2 * FLASH_PARTITION_SIZE)

#define FLASH_AREA_IMAGE_SCRATCH_OFFSET (0x280000)
#define FLASH_AREA_IMAGE_SCRATCH_SIZE   (2 * FLASH_PARTITION_SIZE)

/* Offset and size definition in flash area, used by assemble.py */
#define SECURE_IMAGE_OFFSET             0x0
#define SECURE_IMAGE_MAX_SIZE           0x80000

#define NON_SECURE_IMAGE_OFFSET         0x80000
#define NON_SECURE_IMAGE_MAX_SIZE       0x80000

#endif /* __FLASH_LAYOUT_H__ */
