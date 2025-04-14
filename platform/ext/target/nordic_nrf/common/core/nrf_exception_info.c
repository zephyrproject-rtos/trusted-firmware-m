/*
 * Copyright (c) 2023, Nordic Semiconductor ASA. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include "nrf_exception_info.h"
#include "tfm_spm_log.h"
#include "spu.h"

static struct nrf_exception_info nrf_exc_info;

static void spu_dump_context(struct nrf_exception_info *ctx)
{
    SPMLOG_ERRMSG("Platform Exception: SPU Fault\r\n");

    /* Report which type of violation occured */
    if (ctx->events & SPU_EVENT_RAMACCERR) {
        SPMLOG_DBGMSG("  RAMACCERR\r\n");
    }

    if (ctx->events & SPU_EVENT_PERIPHACCERR) {
        SPMLOG_DBGMSG("  PERIPHACCERR\r\n");
    }

    if (ctx->events & SPU_EVENT_FLASHACCERR) {
        SPMLOG_DBGMSG("  FLASHACCERR\r\n");
    }
}

void nrf_exception_info_store_context(void)
{
    nrf_exc_info.events = spu_events_get();

    spu_dump_context(&nrf_exc_info);
}

void nrf_exception_info_get_context(struct nrf_exception_info *ctx)
{
    memcpy(ctx, &nrf_exc_info, sizeof(struct nrf_exception_info));
}
