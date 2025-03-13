/*
 * Copyright (c) 2023, Nordic Semiconductor ASA. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __NRF_EXCEPTION_INFO_H__
#define __NRF_EXCEPTION_INFO_H__

#include <stdint.h>

struct nrf_exception_info {
    uint32_t events;
    union{
        struct {
            uint32_t address;
        } periphaccerr;

        struct {
            uint32_t address;
            uint32_t info;
        } memaccerr;
    };
};

void nrf_exception_info_store_context(void);

void nrf_exception_info_get_context(struct nrf_exception_info *ctx);

#endif /* __NRF_EXCEPTION_INFO_H__ */
