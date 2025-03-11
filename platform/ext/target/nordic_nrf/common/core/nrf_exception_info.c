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

static void dump_exception_info(struct nrf_exception_info *ctx)
{
    SPMLOG_ERRMSG("Platform Exception:\r\n");

    /* Report which type of violation occured */
    if (ctx->events & SPU_EVENT_RAMACCERR) {
        SPMLOG_DBGMSG("  SPU.RAMACCERR\r\n");
    }

    if (ctx->events & SPU_EVENT_PERIPHACCERR) {
        SPMLOG_DBGMSG("  SPU.PERIPHACCERR\r\n");
        SPMLOG_DBGMSGVAL(" Target addr: ", ctx->periphaccerr.address);
    }

    if (ctx->events & SPU_EVENT_FLASHACCERR) {
        SPMLOG_DBGMSG("  SPU.FLASHACCERR\r\n");
    }

#if MPC_PRESENT
    if (ctx->events & MPC_EVENT_MEMACCERR) {
        SPMLOG_DBGMSG("  MPC.MEMACCERR\r\n");
        SPMLOG_DBGMSGVAL("  Target addr:          ", ctx->memaccerr.address);
        SPMLOG_DBGMSGVAL("  Access information:   ", ctx->memaccerr.info);
        SPMLOG_DBGMSGVAL("    Owner id:     ", ctx->memaccerr.info & 0xf);
        SPMLOG_DBGMSGVAL("    Masterport:   ", (ctx->memaccerr.info & 0x1f0) >> 4);
        SPMLOG_DBGMSGVAL("    Read:         ", (ctx->memaccerr.info >> 12) & 1);
        SPMLOG_DBGMSGVAL("    Write:        ", (ctx->memaccerr.info >> 13) & 1);
        SPMLOG_DBGMSGVAL("    Execute:      ", (ctx->memaccerr.info >> 14) & 1);
        SPMLOG_DBGMSGVAL("    Secure:       ", (ctx->memaccerr.info >> 15) & 1);
        SPMLOG_DBGMSGVAL("    Error source: ", (ctx->memaccerr.info >> 16) & 1);
    }
#endif
}

void nrf_exception_info_store_context(void)
{
    nrf_exc_info.events = spu_events_get();

#ifdef SPU_PERIPHACCERR_ADDRESS_ADDRESS_Msk
    if (nrf_exc_info.events & SPU_EVENT_PERIPHACCERR){
        nrf_exc_info.periphaccerr.address = spu_get_peri_addr();
    }
#endif

#ifdef MPC_PRESENT
    nrf_exc_info.events |= mpc_events_get();
    if (nrf_exc_info.events & MPC_EVENT_MEMACCERR)
    {
        nrf_exc_info.memaccerr.address = NRF_MPC00->MEMACCERR.ADDRESS;
        nrf_exc_info.memaccerr.info = NRF_MPC00->MEMACCERR.INFO;
    }
#endif

    dump_exception_info(&nrf_exc_info);
}

void nrf_exception_info_get_context(struct nrf_exception_info *ctx)
{
    memcpy(ctx, &nrf_exc_info, sizeof(struct nrf_exception_info));
}
