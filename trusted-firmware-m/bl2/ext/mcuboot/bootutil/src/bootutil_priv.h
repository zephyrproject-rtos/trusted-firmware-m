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

#ifndef H_BOOTUTIL_PRIV_
#define H_BOOTUTIL_PRIV_

#include <stdbool.h>
#include "flash_map/flash_map.h"
#include "bootutil/bootutil.h"
#include "bootutil/image.h"
#include "flash_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MCUBOOT_HAVE_ASSERT_H
#include "mcuboot_config/mcuboot_assert.h"
#else
#define ASSERT assert
#endif

struct flash_area;

#define BOOT_EFLASH      1
#define BOOT_EFILE       2
#define BOOT_EBADIMAGE   3
#define BOOT_EBADVECT    4
#define BOOT_EBADSTATUS  5
#define BOOT_ENOMEM      6
#define BOOT_EBADARGS    7
#define BOOT_EBADMAGIC   8
#define BOOT_EBADVERSION 9

#define BOOT_TMPBUF_SZ  256

/*
 * Maintain state of copy progress.
 */
struct boot_status {
    uint32_t idx;         /* Which area we're operating on */
    uint8_t state;        /* Which part of the swapping process are we at */
    uint8_t use_scratch;  /* Are status bytes ever written to scratch? */
    uint8_t swap_type;    /* The type of swap in effect */
    uint32_t swap_size;   /* Total size of swapped image */
};

#define BOOT_MAGIC_GOOD     1
#define BOOT_MAGIC_BAD      2
#define BOOT_MAGIC_UNSET    3
#define BOOT_MAGIC_ANY      4  /* NOTE: control only, not dependent on sector */
#define BOOT_MAGIC_NOTGOOD  5  /* NOTE: control only, not dependent on sector */

/*
 * NOTE: leave BOOT_FLAG_SET equal to one, this is written to flash!
 */
#define BOOT_FLAG_SET       1
#define BOOT_FLAG_BAD       2
#define BOOT_FLAG_UNSET     3
#define BOOT_FLAG_ANY       4  /* NOTE: control only, not dependent on sector */

#define BOOT_STATUS_IDX_0   1

#define BOOT_STATUS_STATE_0 1
#define BOOT_STATUS_STATE_1 2
#define BOOT_STATUS_STATE_2 3

/**
 * End-of-image slot structure.
 *
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  ~                                                               ~
 *  ~    Swap status (BOOT_MAX_IMG_SECTORS * min-write-size * 3)    ~
 *  ~                                                               ~
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                      Swap size (4 octets)                     |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |   Swap info   |           0xff padding (7 octets)             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |   Copy done   |           0xff padding (7 octets)             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |   Image OK    |           0xff padding (7 octets)             |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |                       MAGIC (16 octets)                       |
 *  |                                                               |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

extern uint8_t current_image;
extern const uint32_t boot_img_magic[4];

struct boot_swap_state {
    uint8_t magic;      /* One of the BOOT_MAGIC_[...] values. */
    uint8_t swap_type;  /* One of the BOOT_SWAP_TYPE_[...] values. */
    uint8_t copy_done;  /* One of the BOOT_FLAG_[...] values. */
    uint8_t image_ok;   /* One of the BOOT_FLAG_[...] values. */
    uint8_t image_num;  /* Boot status belongs to this image */
};

#ifdef MCUBOOT_IMAGE_NUMBER
#define BOOT_IMAGE_NUMBER       MCUBOOT_IMAGE_NUMBER
#else
#define BOOT_IMAGE_NUMBER       1
#endif

/*
 * Extract the swap type and image number from image trailers's swap_info
 * field.
 */
#define SWAP_INFO_SWAP_TYPE_MASK        (0x0Fu)
#define SWAP_INFO_SWAP_TYPE_POS         (0)
#define SWAP_INFO_IMAGE_NUM_MASK        (0xF0u)
#define SWAP_INFO_IMAGE_NUM_POS         (4)

#define BOOT_GET_SWAP_TYPE(swap_info)   ((swap_info) & SWAP_INFO_SWAP_TYPE_MASK)
#define BOOT_GET_IMAGE_NUM(swap_info)   ((swap_info) >> SWAP_INFO_IMAGE_NUM_POS)

/* Construct the swap_info field from swap type and image number */
#define BOOT_SET_SWAP_INFO(swap_info, image, type)  {                          \
                     assert(((image) & (SWAP_INFO_IMAGE_NUM_MASK >>            \
                                        SWAP_INFO_IMAGE_NUM_POS)) == (image)); \
                     assert(((type) & SWAP_INFO_SWAP_TYPE_MASK) == (type));    \
                     (swap_info) = (image) << SWAP_INFO_IMAGE_NUM_POS          \
                                 | (type);                                     \
                     }

/*
 * The current flashmap API does not check the amount of space allocated when
 * loading sector data from the flash device, allowing for smaller counts here
 * would most surely incur in overruns.
 *
 * TODO: make flashmap API receive the current sector array size.
 */
#if BOOT_MAX_IMG_SECTORS < 32
#error "Too few sectors, please increase BOOT_MAX_IMG_SECTORS to at least 32"
#endif

/** Number of image slots in flash; currently limited to two. */
#define BOOT_NUM_SLOTS                  2

/** Maximum number of image sectors supported by the bootloader. */
#define BOOT_STATUS_STATE_COUNT         3

#define BOOT_PRIMARY_SLOT               0
#define BOOT_SECONDARY_SLOT             1

#define BOOT_STATUS_SOURCE_NONE         0
#define BOOT_STATUS_SOURCE_SCRATCH      1
#define BOOT_STATUS_SOURCE_PRIMARY_SLOT 2

extern const uint32_t BOOT_MAGIC_SZ;

/**
 * Compatibility shim for flash sector type.
 *
 * This can be deleted when flash_area_to_sectors() is removed.
 */
#ifdef MCUBOOT_USE_FLASH_AREA_GET_SECTORS
typedef struct flash_sector boot_sector_t;
#else
typedef struct flash_area boot_sector_t;
#endif

/** Private state maintained during boot. */
struct boot_loader_state {
    struct {
        struct image_header hdr;
        const struct flash_area *area;
        boot_sector_t *sectors;
        size_t num_sectors;
        bool is_hdr_valid;
    } imgs[BOOT_IMAGE_NUMBER][BOOT_NUM_SLOTS];

    struct {
        const struct flash_area *area;
        boot_sector_t *sectors;
        size_t num_sectors;
    } scratch;

    uint8_t swap_type[BOOT_IMAGE_NUMBER];
    uint32_t write_sz;
};

uint32_t boot_secure_memequal(const void *s1, const void *s2, size_t n);
int bootutil_verify_sig(uint8_t *hash, uint32_t hlen, uint8_t *sig,
                        size_t slen, uint8_t key_id);

int boot_magic_compatible_check(uint8_t tbl_val, uint8_t val);
uint32_t boot_trailer_sz(uint32_t min_write_sz);
int boot_status_entries(const struct flash_area *fap);
uint32_t boot_status_off(const struct flash_area *fap);
uint32_t boot_swap_info_off(const struct flash_area *fap);
int boot_read_swap_state(const struct flash_area *fap,
                         struct boot_swap_state *state);
int boot_read_swap_state_by_id(int flash_area_id,
                               struct boot_swap_state *state);
int boot_write_magic(const struct flash_area *fap);
int boot_write_status(struct boot_status *bs);
int boot_schedule_test_swap(void);
int boot_write_copy_done(const struct flash_area *fap);
int boot_write_image_ok(const struct flash_area *fap);
int boot_write_swap_info(const struct flash_area *fap, uint8_t swap_type,
                         uint8_t image_num);
int boot_write_swap_size(const struct flash_area *fap, uint32_t swap_size);
int boot_read_swap_size(uint32_t *swap_size);
#if (BOOT_IMAGE_NUMBER > 1)
int boot_is_version_sufficient(struct image_version *req,
                               struct image_version *ver);
#endif
bool boot_add_uint32_overflow_check(uint32_t a, uint32_t b);
bool boot_add_uint16_overflow_check(uint16_t a, uint16_t b);

/*
 * Accessors for the contents of struct boot_loader_state.
 */

/* These are macros so they can be used as lvalues. */
#define BOOT_IMG(state, slot) ((state)->imgs[current_image][(slot)])
#define BOOT_IMG_AREA(state, slot) (BOOT_IMG(state, slot).area)
#define BOOT_IMG_HDR_IS_VALID(state, slot) (BOOT_IMG(state, slot).is_hdr_valid)
#define BOOT_SCRATCH_AREA(state) ((state)->scratch.area)
#define BOOT_WRITE_SZ(state) ((state)->write_sz)
#define BOOT_SWAP_TYPE(state) ((state)->swap_type[current_image])

static inline struct image_header*
boot_img_hdr(struct boot_loader_state *state, size_t slot)
{
    return &BOOT_IMG(state, slot).hdr;
}

static inline size_t
boot_img_num_sectors(struct boot_loader_state *state, size_t slot)
{
    return BOOT_IMG(state, slot).num_sectors;
}

static inline size_t
boot_scratch_num_sectors(struct boot_loader_state *state)
{
    return state->scratch.num_sectors;
}

/*
 * Offset of the slot from the beginning of the flash device.
 */
static inline uint32_t
boot_img_slot_off(struct boot_loader_state *state, size_t slot)
{
    return BOOT_IMG(state, slot).area->fa_off;
}

static inline size_t boot_scratch_area_size(struct boot_loader_state *state)
{
    return BOOT_SCRATCH_AREA(state)->fa_size;
}

#ifndef MCUBOOT_USE_FLASH_AREA_GET_SECTORS

static inline size_t
boot_img_sector_size(struct boot_loader_state *state,
                     size_t slot, size_t sector)
{
    return BOOT_IMG(state, slot).sectors[sector].fa_size;
}

/*
 * Offset of the sector from the beginning of the image, NOT the flash
 * device.
 */
static inline uint32_t
boot_img_sector_off(struct boot_loader_state *state, size_t slot,
                    size_t sector)
{
    return BOOT_IMG(state, slot).sectors[sector].fa_off -
           BOOT_IMG(state, slot).sectors[0].fa_off;
}

static inline int
boot_initialize_area(struct boot_loader_state *state, int flash_area)
{
    int num_sectors = BOOT_MAX_IMG_SECTORS;
    int rc;

    if (flash_area == FLASH_AREA_IMAGE_PRIMARY) {
        rc = flash_area_to_sectors(flash_area, &num_sectors,
                                   BOOT_IMG(state, BOOT_PRIMARY_SLOT).sectors);
        BOOT_IMG(state, BOOT_PRIMARY_SLOT).num_sectors = (size_t)num_sectors;
    } else if (flash_area == FLASH_AREA_IMAGE_SECONDARY) {
        rc = flash_area_to_sectors(flash_area, &num_sectors,
                                 BOOT_IMG(state, BOOT_SECONDARY_SLOT).sectors);
        BOOT_IMG(state, BOOT_SECONDARY_SLOT).num_sectors = (size_t)num_sectors;
    } else if (flash_area == FLASH_AREA_IMAGE_SCRATCH) {
        rc = flash_area_to_sectors(flash_area, &num_sectors,
                                   state->scratch.sectors);
        state->scratch.num_sectors = (size_t)num_sectors;
    } else {
        return BOOT_EFLASH;
    }

    return rc;
}

#else  /* defined(MCUBOOT_USE_FLASH_AREA_GET_SECTORS) */

static inline size_t
boot_img_sector_size(struct boot_loader_state *state,
                     size_t slot, size_t sector)
{
    return BOOT_IMG(state, slot).sectors[sector].fs_size;
}

static inline uint32_t
boot_img_sector_off(struct boot_loader_state *state, size_t slot,
                    size_t sector)
{
    return BOOT_IMG(state, slot).sectors[sector].fs_off -
           BOOT_IMG(state, slot).sectors[0].fs_off;
}

static inline int
boot_initialize_area(struct boot_loader_state *state, int flash_area)
{
    uint32_t num_sectors;
    struct flash_sector *out_sectors;
    size_t *out_num_sectors;
    int rc;

    num_sectors = BOOT_MAX_IMG_SECTORS;

    if (flash_area == FLASH_AREA_IMAGE_PRIMARY) {
        out_sectors = BOOT_IMG(state, BOOT_PRIMARY_SLOT).sectors;
        out_num_sectors = &BOOT_IMG(state, BOOT_PRIMARY_SLOT).num_sectors;
    } else if (flash_area == FLASH_AREA_IMAGE_SECONDARY) {
        out_sectors = BOOT_IMG(state, BOOT_SECONDARY_SLOT).sectors;
        out_num_sectors = &BOOT_IMG(state, BOOT_SECONDARY_SLOT).num_sectors;
    } else if (flash_area == FLASH_AREA_IMAGE_SCRATCH) {
        out_sectors = state->scratch.sectors;
        out_num_sectors = &state->scratch.num_sectors;
    } else {
        return BOOT_EFLASH;
    }

    rc = flash_area_get_sectors(flash_area, &num_sectors, out_sectors);
    if (rc != 0) {
        return rc;
    }
    *out_num_sectors = num_sectors;
    return 0;
}

#endif  /* !defined(MCUBOOT_USE_FLASH_AREA_GET_SECTORS) */

#ifdef MCUBOOT_RAM_LOADING
#define LOAD_IMAGE_DATA(fap, start, output, size)\
    (memcpy((output),(void*)(hdr->ih_load_addr + (start)), (size)) != (output))
#else
#define LOAD_IMAGE_DATA(fap, start, output, size)\
    (flash_area_read((fap), (start), (output), (size)))
#endif /* MCUBOOT_RAM_LOADING */

#ifdef __cplusplus
}
#endif

#endif
