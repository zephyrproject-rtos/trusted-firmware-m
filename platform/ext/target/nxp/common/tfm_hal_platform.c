/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 * Copyright 2020-2022 NXP. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "target_cfg.h"
#include "tfm_hal_platform.h"
#include "tfm_plat_defs.h"
#include "uart_stdout.h"
#include "fih.h"
#include "tfm_spm_log.h"

extern const struct memory_region_limits memory_regions;


#ifdef TFM_FIH_PROFILE_ON
fih_int tfm_hal_platform_init(void)
#else
enum tfm_hal_status_t tfm_hal_platform_init(void)
#endif
{
    enum tfm_plat_err_t plat_err = TFM_PLAT_ERR_SYSTEM_ERR;

    stdio_init();
    plat_err = enable_fault_handlers();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_GENERIC));
    }

    plat_err = system_reset_cfg();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_GENERIC));
    }

    plat_err = init_debug();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_GENERIC));
    }

    __enable_irq();

    plat_err = nvic_interrupt_target_state_cfg();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_GENERIC));
    }

    plat_err = nvic_interrupt_enable();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        FIH_RET(fih_int_encode(TFM_HAL_ERROR_GENERIC));
    }

    FIH_RET(fih_int_encode(TFM_HAL_SUCCESS));
}

uint32_t tfm_hal_get_ns_VTOR(void)
{
    return memory_regions.non_secure_code_start;
}

uint32_t tfm_hal_get_ns_MSP(void)
{
    return *((uint32_t *)memory_regions.non_secure_code_start);
}

uint32_t tfm_hal_get_ns_entry_point(void)
{
    return *((uint32_t *)(memory_regions.non_secure_code_start + 4));
}

#ifdef FIH_ENABLE_DELAY  

/* This implementation is based on MCUBoot */
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

static mbedtls_entropy_context fih_entropy_ctx;
static mbedtls_ctr_drbg_context fih_drbg_ctx;

fih_int tfm_fih_random_init(void)
{
    mbedtls_entropy_init(&fih_entropy_ctx);
    mbedtls_ctr_drbg_init(&fih_drbg_ctx);
    mbedtls_ctr_drbg_seed(&fih_drbg_ctx , mbedtls_entropy_func,
                          &fih_entropy_ctx, NULL, 0);

    return FIH_SUCCESS;
}

void tfm_fih_random_generate(uint8_t *rand)
{
    mbedtls_ctr_drbg_random(&fih_drbg_ctx, (unsigned char*) rand, sizeof(uint8_t));
}

#endif /* FIH_ENABLE_DELAY */
