/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto_hw.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "cc_hal_plat.h"
#include "cc_prod.h"
#include "cc_cmpu.h"
#include "cc_dmpu.h"
#include "cc_pal_types.h"
#include "dx_reg_base_host.h"
#include "mbedtls_cc_mng_int.h"

extern uint8_t rotpk_hash_0[];
extern uint8_t rotpk_hash_1[];

/* Workspace for provisioning internal use */
#define PROVISIONING_MEM_BUF_LEN  \
        (CMPU_WORKSPACE_MINIMUM_SIZE > DMPU_WORKSPACE_MINIMUM_SIZE) ? \
         CMPU_WORKSPACE_MINIMUM_SIZE : DMPU_WORKSPACE_MINIMUM_SIZE
__attribute__((aligned(CC_32BIT_WORD_SIZE)))
static uint8_t provisioning_mem_buf[PROVISIONING_MEM_BUF_LEN];

static int cc312_cmpu_provision(void)
{
    int rc;
    uint8_t *pWorkspaceBuf = provisioning_mem_buf;
    static const uint32_t DCU_DEFAULT_LOCK[PROD_DCU_LOCK_WORD_SIZE];
    static CCCmpuData_t cmpuData;

    /* Populate cmpuData with data */
#if (MCUBOOT_IMAGE_NUMBER == 1)
    /* HBK won't be provisioned */
    cmpuData.uniqueDataType = CMPU_UNIQUE_IS_USER_DATA;
#elif (MCUBOOT_IMAGE_NUMBER == 2)
    cmpuData.uniqueDataType = CMPU_UNIQUE_IS_HBK0;
    memcpy(cmpuData.uniqueBuff.hbk0, rotpk_hash_0, 16);
#else
#error "MCUBOOT_IMAGE_NUMBER is not defined or has invalid value"
#endif

    cmpuData.kpicvDataType = ASSET_NO_KEY;
    cmpuData.kceicvDataType = ASSET_NO_KEY;
    cmpuData.icvMinVersion = 0;
    cmpuData.icvConfigWord = 0;

    memcpy(cmpuData.icvDcuDefaultLock, DCU_DEFAULT_LOCK,
           sizeof(DCU_DEFAULT_LOCK));

    rc = CCProd_Cmpu(DX_BASE_CC, &cmpuData, (unsigned long)pWorkspaceBuf,
                     CMPU_WORKSPACE_MINIMUM_SIZE);

    return rc;
}

static int cc312_dmpu_provision(void)
{
    int rc;
    uint8_t *pWorkspaceBuf = provisioning_mem_buf;
    static const uint32_t DCU_DEFAULT_LOCK[PROD_DCU_LOCK_WORD_SIZE];
    static CCDmpuData_t dmpuData;

    /* Populate dmpuData with data */
#if (MCUBOOT_IMAGE_NUMBER == 1)
    dmpuData.hbkType = DMPU_HBK_TYPE_HBK;
    memcpy(dmpuData.hbkBuff.hbk, rotpk_hash_0, 32);
#elif (MCUBOOT_IMAGE_NUMBER == 2)
    dmpuData.hbkType = DMPU_HBK_TYPE_HBK1;
    memcpy(dmpuData.hbkBuff.hbk1, rotpk_hash_1, 16);
#else
#error "MCUBOOT_IMAGE_NUMBER is not defined or has invalid value"
#endif

    dmpuData.kcpDataType = ASSET_NO_KEY;
    dmpuData.kceDataType = ASSET_NO_KEY;
    dmpuData.oemMinVersion = 0;

    memcpy(dmpuData.oemDcuDefaultLock, DCU_DEFAULT_LOCK,
           sizeof(DCU_DEFAULT_LOCK));

    rc = CCProd_Dmpu(DX_BASE_CC, &dmpuData, (unsigned long)pWorkspaceBuf,
                     DMPU_WORKSPACE_MINIMUM_SIZE);
    return rc;
}

int crypto_hw_accelerator_otp_provisioning(void)
{
    int rc;
    uint32_t lcs;

    gCcRegBase = DX_BASE_CC;

    if (CCProd_Init() != CC_OK) {
        return -1;
    }

    rc = mbedtls_mng_lcsGet(&lcs);
    if (rc) {
        return rc;
    }

    if (lcs == CC_MNG_LCS_CM) {
        rc = cc312_cmpu_provision();
        if (rc) {
            return rc;
        }
        printf("First cycle: HUK is provisioned successfully\r\n");
        printf("Please reset the board to program ROTPK\r\n");
    } else if (lcs == CC_MNG_LCS_DM) {
        rc = cc312_dmpu_provision();
        if (rc) {
            return rc;
        }
        printf("Second cycle: ROTPK is provisioned successfully\r\n");
        printf("Provisioning finished, reset the board to get secure"
               " enabled lifecycle\r\n");
    } else {
        printf("Board is in invalid lifecycle for provisioning: %u\r\n", lcs);
    }

    return 0;
}
