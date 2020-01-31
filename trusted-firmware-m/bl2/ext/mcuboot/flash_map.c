/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: 3c469bc698a9767859ed73cd0201c44161204d5c
 * Modifications are Copyright (c) 2018-2019 Arm Limited.
 */

#include <errno.h>
#include <stdbool.h>

#include "target.h"
#include "bl2_util.h"
#include "Driver_Flash.h"

#include <flash_map/flash_map.h>

#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#include "bootutil/bootutil_log.h"

/* Flash device name must be specified by target */
extern ARM_DRIVER_FLASH FLASH_DEV_NAME;

/*
 * For now, we only support one flash device.
 *
 * Pick a random device ID for it that's unlikely to collide with
 * anything "real".
 */
#define FLASH_DEVICE_ID 100
#define FLASH_DEVICE_BASE FLASH_BASE_ADDRESS

#define FLASH_MAP_ENTRY_MAGIC 0xd00dbeef

struct flash_map_entry {
    const uint32_t magic;
    const struct flash_area area;
    unsigned int ref_count;
};

/*
 * The flash area describes essentially the partition table of the
 * flash.  In this case, it starts with FLASH_AREA_IMAGE_PRIMARY.
 */
static struct flash_map_entry part_map[] = {
    {
        .magic = FLASH_MAP_ENTRY_MAGIC,
        .area = {
            .fa_id = FLASH_AREA_0_ID,
            .fa_device_id = FLASH_DEVICE_ID,
            .fa_off = FLASH_AREA_0_OFFSET,
            .fa_size = FLASH_AREA_0_SIZE,
        },
    },
    {
        .magic = FLASH_MAP_ENTRY_MAGIC,
        .area = {
            .fa_id = FLASH_AREA_2_ID,
            .fa_device_id = FLASH_DEVICE_ID,
            .fa_off = FLASH_AREA_2_OFFSET,
            .fa_size = FLASH_AREA_2_SIZE,
        },
    },
#if (MCUBOOT_IMAGE_NUMBER == 2)
    {
        .magic = FLASH_MAP_ENTRY_MAGIC,
        .area = {
            .fa_id = FLASH_AREA_1_ID,
            .fa_device_id = FLASH_DEVICE_ID,
            .fa_off = FLASH_AREA_1_OFFSET,
            .fa_size = FLASH_AREA_1_SIZE,
        },
    },
    {
        .magic = FLASH_MAP_ENTRY_MAGIC,
        .area = {
            .fa_id = FLASH_AREA_3_ID,
            .fa_device_id = FLASH_DEVICE_ID,
            .fa_off = FLASH_AREA_3_OFFSET,
            .fa_size = FLASH_AREA_3_SIZE,
        },
    },
#endif
    {
        .magic = FLASH_MAP_ENTRY_MAGIC,
        .area = {
            .fa_id = FLASH_AREA_SCRATCH_ID,
            .fa_device_id = FLASH_DEVICE_ID,
            .fa_off = FLASH_AREA_SCRATCH_OFFSET,
            .fa_size = FLASH_AREA_SCRATCH_SIZE,
        },
    }
};

int flash_device_base(uint8_t fd_id, uintptr_t *ret)
{
    if (fd_id != FLASH_DEVICE_ID) {
        BOOT_LOG_ERR("invalid flash ID %d; expected %d",
                     fd_id, FLASH_DEVICE_ID);
        return -1;
    }
    *ret = FLASH_DEVICE_BASE;
    return 0;
}

/*
 * `open` a flash area.  The `area` in this case is not the individual
 * sectors, but describes the particular flash area in question.
 */
int flash_area_open(uint8_t id, const struct flash_area **area)
{
    int i;

    BOOT_LOG_DBG("area %d", id);

    for (i = 0; i < ARRAY_SIZE(part_map); i++) {
        if (id == part_map[i].area.fa_id) {
            break;
        }
    }
    if (i == ARRAY_SIZE(part_map)) {
        return -1;
    }

    *area = &part_map[i].area;
    part_map[i].ref_count++;
    return 0;
}

/*
 * Nothing to do on close.
 */
void flash_area_close(const struct flash_area *area)
{
    struct flash_map_entry *entry;

    if (!area) {
        return;
    }

    entry = CONTAINER_OF(area, struct flash_map_entry, area);
    if (entry->magic != FLASH_MAP_ENTRY_MAGIC) {
        BOOT_LOG_ERR("invalid area %p (id %u)", area, area->fa_id);
        return;
    }
    if (entry->ref_count == 0) {
        BOOT_LOG_ERR("area %u use count underflow", area->fa_id);
        return;
    }
    entry->ref_count--;
}

void flash_area_warn_on_open(void)
{
    int i;
    struct flash_map_entry *entry;

    for (i = 0; i < ARRAY_SIZE(part_map); i++) {
        entry = &part_map[i];
        if (entry->ref_count) {
            BOOT_LOG_WRN("area %u has %u users",
                         entry->area.fa_id, entry->ref_count);
        }
    }
}

uint8_t flash_area_erased_val(const struct flash_area *area)
{
    (void)area;

    return FLASH_DEV_NAME.GetInfo()->erased_value;
}

int flash_area_read(const struct flash_area *area, uint32_t off, void *dst,
                    uint32_t len)
{
    BOOT_LOG_DBG("read area=%d, off=%#x, len=%#x", area->fa_id, off, len);
    return FLASH_DEV_NAME.ReadData(area->fa_off + off, dst, len);
}

int flash_area_read_is_empty(const struct flash_area *area, uint32_t off,
                             void *dst, uint32_t len)
{
    uint32_t i;
    uint8_t *u8dst;
    int rc;

    BOOT_LOG_DBG("read_is_empty area=%d, off=%#x, len=%#x",
                 area->fa_id, off, len);

    rc = FLASH_DEV_NAME.ReadData(area->fa_off + off, dst, len);
    if(rc != 0) {
        return -1;
    }

    u8dst = (uint8_t*)dst;

    for (i = 0; i < len; i++) {
        if (u8dst[i] != flash_area_erased_val(area)) {
            return 0;
        }
    }

    return 1;
}

int flash_area_write(const struct flash_area *area, uint32_t off,
                     const void *src, uint32_t len)
{
    BOOT_LOG_DBG("write area=%d, off=%#x, len=%#x", area->fa_id, off, len);
    return FLASH_DEV_NAME.ProgramData(area->fa_off + off, src, len);
}

int flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len)
{
    ARM_FLASH_INFO *flash_info;
    uint32_t deleted_len = 0;
    int32_t rc = 0;

    BOOT_LOG_DBG("erase area=%d, off=%#x, len=%#x", area->fa_id, off, len);
    flash_info = FLASH_DEV_NAME.GetInfo();

    if (flash_info->sector_info == NULL) {
        /* Uniform sector layout */
        while (deleted_len < len) {
            rc = FLASH_DEV_NAME.EraseSector(area->fa_off + off);
            if (rc != 0) {
                break;
            }
            deleted_len += flash_info->sector_size;
            off         += flash_info->sector_size;
        }
    } else {
        /* Inhomogeneous sector layout, explicitly defined
         * Currently not supported.
         */
    }

    return rc;
}

uint32_t flash_area_align(const struct flash_area *area)
{
    ARM_FLASH_INFO *flash_info;

    flash_info = FLASH_DEV_NAME.GetInfo();
    return flash_info->program_unit;
}

/*
 * This depends on the mappings defined in sysflash.h, and assumes that the
 * primary slot, the secondary slot, and the scratch area are contiguous.
 */
int flash_area_id_from_image_slot(int slot)
{
#if (MCUBOOT_IMAGE_NUMBER == 1)
    static
#endif
    const int area_id_tab[] = {FLASH_AREA_IMAGE_PRIMARY,
                               FLASH_AREA_IMAGE_SECONDARY,
                               FLASH_AREA_IMAGE_SCRATCH};

    if (slot >= 0 && slot < ARRAY_SIZE(area_id_tab)) {
        return area_id_tab[slot];
    }

    return -EINVAL; /* flash_area_open will fail on that */
}

int flash_area_id_to_image_slot(int area_id)
{
    if (area_id == FLASH_AREA_IMAGE_PRIMARY) {
        return 0;
    }
    if (area_id == FLASH_AREA_IMAGE_SECONDARY) {
        return 1;
    }

    BOOT_LOG_ERR("invalid flash area ID");
    return -1;
}

static int validate_idx(int idx, uint32_t *off, uint32_t *len)
{
    /*
     * This simple layout has uniform slots, so just fill in the
     * right one.
     */

    switch (idx) {
    case FLASH_AREA_0_ID:
        *off = FLASH_AREA_0_OFFSET;
        *len = FLASH_AREA_0_SIZE;
        break;
    case FLASH_AREA_2_ID:
        *off = FLASH_AREA_2_OFFSET;
        *len = FLASH_AREA_2_SIZE;
        break;
#if (MCUBOOT_IMAGE_NUMBER == 2)
    case FLASH_AREA_1_ID:
        *off = FLASH_AREA_1_OFFSET;
        *len = FLASH_AREA_1_SIZE;
        break;
    case FLASH_AREA_3_ID:
        *off = FLASH_AREA_3_OFFSET;
        *len = FLASH_AREA_3_SIZE;
        break;
#endif
    case FLASH_AREA_SCRATCH_ID:
        *off = FLASH_AREA_SCRATCH_OFFSET;
        *len = FLASH_AREA_SCRATCH_SIZE;
        break;
    default:
        BOOT_LOG_ERR("unknown flash area %d", idx);
        return -1;
    }

    BOOT_LOG_DBG("area %d: offset=0x%x, length=0x%x, sector size=0x%x",
                 idx, *off, *len, FLASH_AREA_IMAGE_SECTOR_SIZE);
    return 0;
}

int flash_area_to_sectors(int idx, int *cnt, struct flash_area *ret)
{
    uint32_t off;
    uint32_t len;
    uint32_t max_cnt = *cnt;
    uint32_t rem_len;

    if (validate_idx(idx, &off, &len)) {
        return -1;
    }

    if (*cnt < 1) {
        return -1;
    }

    rem_len = len;
    *cnt = 0;
    while (rem_len > 0 && *cnt < max_cnt) {
        if (rem_len < FLASH_AREA_IMAGE_SECTOR_SIZE) {
            BOOT_LOG_ERR("area %d size 0x%x not divisible by sector size 0x%x",
                     idx, len, FLASH_AREA_IMAGE_SECTOR_SIZE);
            return -1;
        }

        ret[*cnt].fa_id = idx;
        ret[*cnt].fa_device_id = 0;
        ret[*cnt].pad16 = 0;
        ret[*cnt].fa_off = off + (FLASH_AREA_IMAGE_SECTOR_SIZE * (*cnt));
        ret[*cnt].fa_size = FLASH_AREA_IMAGE_SECTOR_SIZE;
        *cnt = *cnt + 1;
        rem_len -= FLASH_AREA_IMAGE_SECTOR_SIZE;
    }

    if (*cnt > max_cnt) {
        BOOT_LOG_ERR("flash area %d sector count overflow", idx);
        return -1;
    }

    return 0;
}

/*
 * Lookup the sector map for a given flash area.  This should fill in
 * `ret` with all of the sectors in the area.  `*cnt` will be set to
 * the storage at `ret` and should be set to the final number of
 * sectors in this area.
 */
int flash_area_get_sectors(int idx, uint32_t *cnt, struct flash_sector *ret)
{
    uint32_t off;
    uint32_t len;
    uint32_t max_cnt = *cnt;
    uint32_t rem_len;

    if (validate_idx(idx, &off, &len)) {
        return -1;
    }

    if (*cnt < 1) {
        return -1;
    }

    rem_len = len;
    *cnt = 0;
    while (rem_len > 0 && *cnt < max_cnt) {
        if (rem_len < FLASH_AREA_IMAGE_SECTOR_SIZE) {
            BOOT_LOG_ERR("area %d size 0x%x not divisible by sector size 0x%x",
                         idx, len, FLASH_AREA_IMAGE_SECTOR_SIZE);
            return -1;
        }

        ret[*cnt].fs_off = FLASH_AREA_IMAGE_SECTOR_SIZE * (*cnt);
        ret[*cnt].fs_size = FLASH_AREA_IMAGE_SECTOR_SIZE;
        *cnt = *cnt + 1;
        rem_len -= FLASH_AREA_IMAGE_SECTOR_SIZE;
    }

    if (*cnt > max_cnt) {
        BOOT_LOG_ERR("flash area %d sector count overflow", idx);
        return -1;
    }

    return 0;
}
