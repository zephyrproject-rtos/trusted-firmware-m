/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* NOTE: This API should be implemented by platform vendor. For the security of
 * the protected storage system's and the bootloader's rollback protection etc.
 * it is CRITICAL to use a internal (in-die) persistent memory for multiple time
 * programmable (MTP) non-volatile counters or use a One-time Programmable (OTP)
 * non-volatile counters solution.
 *
 * This dummy implementation assumes that the NV counters are the only data in
 * the flash sector. To use it, one flash sector should be allocated exclusively
 * for the NV counters.
 *
 * The current software dummy implementation is not resistant to asynchronous
 * power failures and should not be used in production code. It is exclusively
 * for testing purposes.
 */

#include "tfm_plat_nv_counters.h"

#include <limits.h>
#include "Driver_Flash.h"
#include "flash_layout.h"

/* Compilation time checks to be sure the defines are well defined */
#ifndef TFM_OTP_NV_COUNTERS_AREA_ADDR
#error "TFM_OTP_NV_COUNTERS_AREA_ADDR must be defined in flash_layout.h"
#endif

#ifndef TFM_OTP_NV_COUNTERS_AREA_SIZE
#error "TFM_OTP_NV_COUNTERS_AREA_SIZE must be defined in flash_layout.h"
#endif

#ifndef TFM_OTP_NV_COUNTERS_SECTOR_SIZE
#error "TFM_OTP_NV_COUNTERS_SECTOR_SIZE must be defined in flash_layout.h"
#endif

#ifndef OTP_NV_COUNTERS_FLASH_DEV
    #ifndef FLASH_DEV_NAME
    #error "OTP_NV_COUNTERS_FLASH_DEV or FLASH_DEV_NAME must be defined in flash_layout.h"
    #else
    #define OTP_NV_COUNTERS_FLASH_DEV FLASH_DEV_NAME
    #endif
#endif
/* End of compilation time checks to be sure the defines are well defined */

#define NV_COUNTER_SIZE  sizeof(uint32_t)
#define INIT_VALUE_SIZE  NV_COUNTER_SIZE
#define NUM_OTP_NV_COUNTERS  ((TFM_OTP_NV_COUNTERS_AREA_SIZE - INIT_VALUE_SIZE) \
                          / NV_COUNTER_SIZE)

#define NV_COUNTERS_INITIALIZED 0xC0DE0042U

/**
 * \brief Struct representing the NV counter data in flash.
 */
struct nv_counters_t {
    uint32_t counters[NUM_OTP_NV_COUNTERS]; /**< Array of NV counters */
    uint32_t init_value; /**< Watermark to indicate if the NV counters have been
                          *   initialised
                          */
};

/* Import the CMSIS flash device driver */
extern ARM_DRIVER_FLASH OTP_NV_COUNTERS_FLASH_DEV;

enum tfm_plat_err_t tfm_plat_init_nv_counter(void)
{
    int32_t err;
    uint32_t i;
    struct nv_counters_t nv_counters = {{0}};

    err = OTP_NV_COUNTERS_FLASH_DEV.Initialize(NULL);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Read the NV counter area to be able to erase the sector and write later
     * in the flash.
     */
    err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                                             &nv_counters,
                                             TFM_OTP_NV_COUNTERS_AREA_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (nv_counters.init_value == NV_COUNTERS_INITIALIZED) {
        return TFM_PLAT_ERR_SUCCESS;
    }

    /* Add watermark, at the end of the NV counters area, to indicate that NV
     * counters have been initialized.
     */
    nv_counters.init_value = NV_COUNTERS_INITIALIZED;

    /* Initialize all counters to 0 */
    for (i = 0; i < NUM_OTP_NV_COUNTERS; i++) {
        nv_counters.counters[i] = 0;
    }

    /* Erase sector before write in it */
    err = OTP_NV_COUNTERS_FLASH_DEV.EraseSector(TFM_OTP_NV_COUNTERS_AREA_ADDR);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    /* Write in flash the in-memory NV counter content after modification */
    err = OTP_NV_COUNTERS_FLASH_DEV.ProgramData(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                                                &nv_counters,
                                                TFM_OTP_NV_COUNTERS_AREA_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                             uint32_t size, uint8_t *val)
{
    int32_t  err;
    uint32_t flash_addr;

    if (size != NV_COUNTER_SIZE) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    flash_addr = TFM_OTP_NV_COUNTERS_AREA_ADDR + (counter_id * NV_COUNTER_SIZE);

    err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(flash_addr, val, NV_COUNTER_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_set_nv_counter(enum tfm_nv_counter_t counter_id,
                                            uint32_t value)
{
    int32_t err;
    struct nv_counters_t nv_counters = {{0}};

    /* Read the NV counter area to be able to erase the sector and write later
     * in the flash.
     */
    err = OTP_NV_COUNTERS_FLASH_DEV.ReadData(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                                             &nv_counters,
                                             TFM_OTP_NV_COUNTERS_AREA_SIZE);
    if (err != ARM_DRIVER_OK) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    if (value != nv_counters.counters[counter_id]) {

        if (value > nv_counters.counters[counter_id]) {
            nv_counters.counters[counter_id] = value;
        } else {
            return TFM_PLAT_ERR_INVALID_INPUT;
        }

        /* Erase sector before write in it */
        err = OTP_NV_COUNTERS_FLASH_DEV.EraseSector(
                                                TFM_OTP_NV_COUNTERS_AREA_ADDR);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }

        /* Write in flash the in-memory NV counter content after modification */
        err = OTP_NV_COUNTERS_FLASH_DEV.ProgramData(TFM_OTP_NV_COUNTERS_AREA_ADDR,
                                                    &nv_counters,
                                                    TFM_OTP_NV_COUNTERS_AREA_SIZE);
        if (err != ARM_DRIVER_OK) {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t tfm_plat_increment_nv_counter(
                                           enum tfm_nv_counter_t counter_id)
{
    uint32_t security_cnt;
    enum tfm_plat_err_t err;

    err = tfm_plat_read_nv_counter(counter_id,
                                   sizeof(security_cnt),
                                   (uint8_t *)&security_cnt);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    if (security_cnt == UINT32_MAX) {
        return TFM_PLAT_ERR_MAX_VALUE;
    }

    return tfm_plat_set_nv_counter(counter_id, security_cnt + 1u);
}
