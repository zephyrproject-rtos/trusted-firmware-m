/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* NOTE: For the security of the protected storage system, the bootloader
 * rollback protection, and the protection of cryptographic material it is
 * CRITICAL to use an internal (in-die) persistent memory for the implementation
 * of the OTP_NV_COUNTERS flash area.
 */

#include "flash_otp_nv_counters_backend.h"

#include "tfm_plat_defs.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

#include <string.h>

#define OTP_NV_COUNTERS_INITIALIZED 0xC0DE8112U
#define OTP_NV_COUNTERS_IS_VALID    0x3072C0DEU

#ifdef OTP_NV_COUNTERS_RAM_EMULATION

static struct flash_otp_nv_counters_region_t otp_nv_ram_buf = {0};

enum tfm_plat_err_t read_otp_nv_counters_flash(uint32_t offset, void *data, uint32_t cnt)
{
    memcpy(data, ((void*)&otp_nv_ram_buf) + offset, cnt);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t write_otp_nv_counters_flash(uint32_t offset, const void *data, uint32_t cnt)
{
    memcpy(((void*)&otp_nv_ram_buf) + offset, data, cnt);

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t init_otp_nv_counters_flash(void)
{
    if (otp_nv_ram_buf.init_value != OTP_NV_COUNTERS_INITIALIZED) {
        memset(&otp_nv_ram_buf, 0, sizeof(otp_nv_ram_buf));
        otp_nv_ram_buf.init_value = OTP_NV_COUNTERS_INITIALIZED;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

#else /* OTP_NV_COUNTERS_RAM_EMULATION */

#define OTP_NV_COUNTERS_WRITE_BLOCK_SIZE (TFM_HAL_FLASH_PROGRAM_UNIT > 128 ? \
                                          TFM_HAL_FLASH_PROGRAM_UNIT : 128)

/* Compilation time checks to be sure the defines are well defined */
#ifndef TFM_OTP_NV_COUNTERS_AREA_ADDR
#error "TFM_OTP_NV_COUNTERS_AREA_ADDR must be defined in flash_layout.h"
#endif

#ifndef TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR
#error "TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR must be defined in flash_layout.h"
#endif

#ifndef TFM_OTP_NV_COUNTERS_AREA_SIZE
#error "TFM_OTP_NV_COUNTERS_AREA_SIZE must be defined in flash_layout.h"
#endif

#ifndef TFM_OTP_NV_COUNTERS_SECTOR_SIZE
#error "TFM_OTP_NV_COUNTERS_SECTOR_SIZE must be defined in flash_layout.h"
#endif
#ifndef OTP_NV_COUNTERS_FLASH_DEV
    #ifndef TFM_HAL_ITS_FLASH_DRIVER
    #error "OTP_NV_COUNTERS_FLASH_DEV or TFM_HAL_ITS_FLASH_DRIVER must be defined in flash_layout.h"
    #else
    #define OTP_NV_COUNTERS_FLASH_DEV TFM_HAL_ITS_FLASH_DRIVER
    #endif
#endif
/* End of compilation time checks to be sure the defines are well defined */

/* Import the CMSIS flash device driver */
extern ARM_DRIVER_FLASH OTP_NV_COUNTERS_FLASH_DEV;

static inline uint32_t round_down(uint32_t num, uint32_t boundary)
{
    return num - (num % boundary);
}

enum tfm_plat_err_t read_otp_nv_counters_flash(uint32_t offset, void *data, uint32_t cnt)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;

    err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(TFM_OTP_NV_COUNTERS_AREA_ADDR + offset,
                                             data,
                                             cnt);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return err;
}

static enum tfm_plat_err_t erase_flash_region(size_t start, size_t size)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    size_t idx;

    for (idx = round_down(start, TFM_OTP_NV_COUNTERS_SECTOR_SIZE);
         idx < start + size;
         idx += TFM_OTP_NV_COUNTERS_SECTOR_SIZE) {
        err = OTP_NV_COUNTERS_FLASH_DEV.EraseSector(idx);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return err;
}

static enum tfm_plat_err_t copy_flash_region(size_t from, size_t to, size_t size)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    uint8_t block[OTP_NV_COUNTERS_WRITE_BLOCK_SIZE];
    size_t copy_size;
    size_t idx;
    size_t end;

    end = size;
    for(idx = 0; idx < end; idx += copy_size) {
        copy_size = (idx + sizeof(block)) <= end ? sizeof(block) : end - idx;

        err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(from + idx, block, copy_size);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        err = OTP_NV_COUNTERS_FLASH_DEV.ProgramData(to + idx, block, copy_size);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return err;
}

static enum tfm_plat_err_t make_backup(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;

    err = erase_flash_region(TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR,
                             TFM_OTP_NV_COUNTERS_AREA_SIZE);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = copy_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                            TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR,
                            TFM_OTP_NV_COUNTERS_AREA_SIZE);

    return err;
}

enum tfm_plat_err_t write_otp_nv_counters_flash(uint32_t offset, const void *data, uint32_t cnt)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    uint8_t block[OTP_NV_COUNTERS_WRITE_BLOCK_SIZE];
    size_t copy_size;
    size_t idx;
    size_t start;
    size_t end;
    size_t input_idx = 0;
    size_t input_copy_size;

    err = erase_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR + offset, cnt);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    start = round_down(offset, TFM_OTP_NV_COUNTERS_SECTOR_SIZE);
    end = round_down(offset + cnt, TFM_OTP_NV_COUNTERS_SECTOR_SIZE)
          + TFM_OTP_NV_COUNTERS_SECTOR_SIZE;
    for(idx = start; idx < end; idx += copy_size) {
        copy_size = (idx + sizeof(block)) <= end ? sizeof(block) : end - idx;

        err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR + idx,
                                                 block,
                                                 copy_size);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        if (idx + copy_size >= offset && idx < offset + cnt) {
            input_copy_size = sizeof(block) - ((offset + input_idx) % sizeof(block));
            if (input_idx + input_copy_size > cnt) {
                input_copy_size = cnt - input_idx;
            }

            memcpy((void*)(block + ((offset + input_idx) % sizeof(block))),
                   (void*)((uint8_t *)data + input_idx),
                   input_copy_size);

            input_idx += input_copy_size;
        }

        err = OTP_NV_COUNTERS_FLASH_DEV.ProgramData(TFM_OTP_NV_COUNTERS_AREA_ADDR + idx,
                                                    block,
                                                    copy_size);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        if (idx >= offset && idx < offset + cnt) {
            memset(block, 0, sizeof(block));
        }
    }

    err = make_backup();
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return err;
}

static enum tfm_plat_err_t restore_backup(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;

    err = erase_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                             TFM_OTP_NV_COUNTERS_AREA_SIZE);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = copy_flash_region(TFM_OTP_NV_COUNTERS_BACKUP_AREA_ADDR,
                            TFM_OTP_NV_COUNTERS_AREA_ADDR,
                            TFM_OTP_NV_COUNTERS_AREA_SIZE);

    return err;
}

enum tfm_plat_err_t init_otp_nv_counters_flash(void)
{
    enum tfm_plat_err_t err = TFM_PLAT_ERR_SUCCESS;
    uint32_t init_value;
    uint32_t is_valid;
    size_t idx;
    size_t end;
    size_t copy_size;
    uint8_t block[OTP_NV_COUNTERS_WRITE_BLOCK_SIZE];

    if ((TFM_OTP_NV_COUNTERS_AREA_SIZE) < sizeof(struct flash_otp_nv_counters_region_t)) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = OTP_NV_COUNTERS_FLASH_DEV.Initialize(NULL);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    err = read_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, init_value),
                                     &init_value, sizeof(init_value));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = read_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, is_valid),
                                     &is_valid, sizeof(is_valid));
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    if (init_value != OTP_NV_COUNTERS_INITIALIZED || is_valid != OTP_NV_COUNTERS_IS_VALID) {
        err = read_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, init_value)
                                         + TFM_OTP_NV_COUNTERS_AREA_SIZE,
                                         &init_value, sizeof(init_value));
        if (err != TFM_PLAT_ERR_SUCCESS) {
            return err;
        }

        if (init_value == OTP_NV_COUNTERS_INITIALIZED) {
            err = restore_backup();
            if (err != TFM_PLAT_ERR_SUCCESS) {
                return err;
            }
        } else {
            err = erase_flash_region(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                                     TFM_OTP_NV_COUNTERS_AREA_SIZE);
            if (err != TFM_PLAT_ERR_SUCCESS) {
                return err;
            }

            memset(block, 0, sizeof(block));
            end = TFM_OTP_NV_COUNTERS_AREA_SIZE;
            for(idx = 0; idx < end; idx += copy_size) {
                copy_size = (idx + sizeof(block)) <= end ? sizeof(block) : end - idx;

                err = OTP_NV_COUNTERS_FLASH_DEV.ProgramData(TFM_OTP_NV_COUNTERS_AREA_ADDR + idx,
                                                            block, copy_size);
                if (err != ARM_DRIVER_OK) {
                    return TFM_PLAT_ERR_SYSTEM_ERR;
                }
            }

            err = make_backup();
            if (err != TFM_PLAT_ERR_SUCCESS) {
                return err;
            }

            init_value = OTP_NV_COUNTERS_INITIALIZED;
            err = write_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, init_value),
                                                       &init_value, sizeof(init_value));
            if (err != ARM_DRIVER_OK) {
                return TFM_PLAT_ERR_SYSTEM_ERR;
            }

            is_valid = OTP_NV_COUNTERS_IS_VALID;
            err = write_otp_nv_counters_flash(offsetof(struct flash_otp_nv_counters_region_t, is_valid),
                                                       &is_valid, sizeof(is_valid));
            if (err != ARM_DRIVER_OK) {
                return TFM_PLAT_ERR_SYSTEM_ERR;
            }
        }
    }

    return err;
}

#endif /* OTP_NV_COUNTERS_RAM_EMULATION */
