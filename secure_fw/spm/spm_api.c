/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* This file contains the APIs exported by the SPM to tfm core */

#include <stdio.h>
#include "spm_api.h"
#include "spm_db.h"
#include "tfm_api.h"
#include "mpu_armv8m_drv.h"
#include "region_defs.h"
#include "secure_fw/core/tfm_core.h"

struct spm_partition_db_t g_spm_partition_db = {0,};

#define MPU_REGION_VENEERS   0
#define MPU_REGION_TFM_UNPRIV_CODE   1
#define MPU_REGION_TFM_UNPRIV_DATA   2
#define MPU_REGION_NS_DATA      3
#define PARTITION_REGION_RO       4
#define PARTITION_REGION_RW_STACK 5
#define PARTITION_REGION_PERIPH   6
#define PARTITION_REGION_SHARE    7

/* This should move to platform retarget */
struct mpu_armv8m_dev_t dev_mpu_s = { MPU_BASE };

typedef enum {
    TFM_INIT_FAILURE,
} ss_error_type_t;

/*
 * This function is called when a secure partition causes an error.
 * In case of an error in the error handling, a non-zero value have to be
 * returned.
 */
static void tfm_spm_partition_err_handler(
    struct spm_partition_region_t *partition,
    ss_error_type_t err_type,
    int32_t err_code)
{
#ifdef TFM_CORE_DEBUG
    if (err_type == TFM_INIT_FAILURE) {
        printf("Partition init failed for partition id 0x%08X\r\n",
                partition->partition_id);
    } else {
        printf("Unknown partition error %d for partition id 0x%08X\r\n",
            err_type, partition->partition_id);
    }
#endif
    tfm_spm_partition_set_state(partition->partition_id,
            SPM_PARTITION_STATE_CLOSED);
}

enum spm_err_t tfm_spm_db_init(void)
{
    /* This function initialises partition db */
    g_spm_partition_db.is_init = 1;
    g_spm_partition_db.running_partition_id = INVALID_PARITION_ID;

    g_spm_partition_db.partition_count =
        create_user_partition_db(&g_spm_partition_db, SPM_MAX_PARTITIONS);

    return SPM_ERR_OK;
}

#if TFM_LVL != 1
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);
REGION_DECLARE(Image$$, TFM_UNPRIV_RO_DATA, $$RW$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_RO_DATA, $$ZI$$Limit);
REGION_DECLARE(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
REGION_DECLARE(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Limit);

enum spm_err_t tfm_spm_mpu_init(void)
{
    mpu_armv8m_clean(&dev_mpu_s);

    struct mpu_armv8m_region_cfg_t region_cfg;

    /* Veneer region */
    region_cfg.region_nr = MPU_REGION_VENEERS;
    region_cfg.region_base = CMSE_VENEER_REGION_START;
    region_cfg.region_limit = CMSE_VENEER_REGION_LIMIT;
    region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
    mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);

    /* TFM Core unprivileged code region */
    region_cfg.region_nr = MPU_REGION_TFM_UNPRIV_CODE;
    region_cfg.region_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Base);
    region_cfg.region_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_CODE, $$RO$$Limit);
    region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;
    mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);

    /* TFM Core unprivileged data region */
    region_cfg.region_nr = MPU_REGION_TFM_UNPRIV_DATA;
    region_cfg.region_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_RO_DATA, $$RW$$Base);
    region_cfg.region_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_RO_DATA, $$ZI$$Limit);
    region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
    mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);

    /* TFM Core unprivileged non-secure data region */
    region_cfg.region_nr = MPU_REGION_NS_DATA;
    region_cfg.region_base = NS_DATA_START;
    region_cfg.region_limit = NS_DATA_LIMIT;
    region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
    mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);

    mpu_armv8m_enable(&dev_mpu_s, 1, 1);

    return SPM_ERR_OK;
}

/**
 * Set share region to which the partition needs access
 */
static enum spm_err_t tfm_spm_set_share_region(
            enum tfm_buffer_share_region_e share)
{
    enum spm_err_t res = SPM_ERR_INVALID_CONFIG;
    uint32_t scratch_base =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Base);
    uint32_t scratch_limit =
        (uint32_t)&REGION_NAME(Image$$, TFM_UNPRIV_SCRATCH, $$ZI$$Limit);

    mpu_armv8m_disable(&dev_mpu_s);

    if (share == TFM_BUFFER_SHARE_DISABLE) {
        mpu_armv8m_region_disable(&dev_mpu_s, PARTITION_REGION_SHARE);
    } else {
        struct mpu_armv8m_region_cfg_t region_cfg;

        region_cfg.region_nr = PARTITION_REGION_SHARE;
        region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
        region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
        region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
        switch (share) {
        case TFM_BUFFER_SHARE_SCRATCH:
            /* Use scratch area for SP-to-SP data sharing */
            region_cfg.region_base = scratch_base;
            region_cfg.region_limit = scratch_limit;
            res = SPM_ERR_OK;
            break;
        case TFM_BUFFER_SHARE_NS_CODE:
            region_cfg.region_base = NS_CODE_START;
            region_cfg.region_limit = NS_CODE_LIMIT;
            /* Only allow read access to NS code region and keep
             * exec.never attribute
             */
            region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
            res = SPM_ERR_OK;
            break;
        default:
            res = SPM_ERR_INVALID_CONFIG;
            break;
        }
        if (res == SPM_ERR_OK) {
            mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);
        }
    }
    mpu_armv8m_enable(&dev_mpu_s, 1, 1);

    return res;
}
#endif

enum spm_err_t tfm_spm_partition_init(void)
{
    struct spm_partition_region_t *serv;
    int32_t fail_cnt = 0;
    uint32_t i;

    /* Call the init function for each partition */
    /* FixMe: This implementation only fits level 1 isolation.
     * On higher levels MPU (and PPC) configuration need to be in place to have
     * proper isolation during init.
     */
    for (i = 0; i < g_spm_partition_db.partition_count; ++i) {
        serv = &g_spm_partition_db.partitions[i];
        if (serv->periph_start) {
            ppc_configure_to_secure(serv->periph_ppc_bank,
                                    serv->periph_ppc_loc);
        }
        if (serv->partition_init == NULL) {
            tfm_spm_partition_set_state(serv->partition_id,
                    SPM_PARTITION_STATE_IDLE);
        } else {
            int32_t ret = serv->partition_init();

            if (ret == TFM_SUCCESS) {
                tfm_spm_partition_set_state(
                        serv->partition_id, SPM_PARTITION_STATE_IDLE);
            } else {
                tfm_spm_partition_err_handler(serv, TFM_INIT_FAILURE, ret);
                fail_cnt++;
            }
        }
    }

    if (fail_cnt == 0) {
        return SPM_ERR_OK;
    } else {
        return SPM_ERR_PARTITION_NOT_AVAILABLE;
    }
}

#if TFM_LVL != 1
enum spm_err_t tfm_spm_partition_sandbox_config(uint32_t partition_id)
{
    /* This function takes a partition id and enables the
     * SPM partition for that partition
     */

    struct spm_partition_region_t *serv;
    struct mpu_armv8m_region_cfg_t region_cfg;

    if (!g_spm_partition_db.is_init) {
        return SPM_ERR_PARTITION_DB_NOT_INIT;
    }

    /*brute force id*/
    serv = &g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)];

    mpu_armv8m_disable(&dev_mpu_s);

    /* Configure Regions */

    /* RO region*/
    region_cfg.region_nr = PARTITION_REGION_RO;
    region_cfg.region_base = serv->ro_start;
    region_cfg.region_limit = serv->ro_limit;
    region_cfg.attr_access = MPU_ARMV8M_AP_RO_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_OK;

    mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);

    /* RW, ZI and stack as one region*/
    region_cfg.region_nr = PARTITION_REGION_RW_STACK;
    region_cfg.region_base = serv->rw_start;
    region_cfg.region_limit = serv->stack_top;
    region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
    region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
    region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;

    mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);

    if (serv->periph_start) {
        /* Peripheral */
        region_cfg.region_nr = PARTITION_REGION_PERIPH;
        region_cfg.region_base = serv->periph_start;
        region_cfg.region_limit = serv->periph_limit;
        region_cfg.attr_access = MPU_ARMV8M_AP_RW_PRIV_UNPRIV;
        region_cfg.attr_sh = MPU_ARMV8M_SH_NONE;
        region_cfg.attr_exec = MPU_ARMV8M_XN_EXEC_NEVER;
        mpu_armv8m_region_enable(&dev_mpu_s, &region_cfg);

        ppc_en_secure_unpriv(serv->periph_ppc_bank, serv->periph_ppc_loc);
    }

    mpu_armv8m_enable(&dev_mpu_s, 1, 1);

#ifndef UNPRIV_JUMP_TO_NS
    /* FixMe: if jump_to_ns_code() from unprivileged is solved,
     * this code can be removed
     */
    /* Initialization is done, set thread mode to unprivileged.
     */
    CONTROL_Type ctrl;

    ctrl.w = __get_CONTROL();
    ctrl.b.nPRIV = 1;
    __set_CONTROL(ctrl.w);
    __DSB();
    __ISB();
#endif

    return SPM_ERR_OK;
}

enum spm_err_t tfm_spm_partition_sandbox_deconfig(uint32_t partition_id)
{
    /* This function takes a partition id and disables the
     * SPM partition for that partition
     */

#ifndef UNPRIV_JUMP_TO_NS
    /* FixMe: if jump_to_ns_code() from unprivileged is solved,
     * this code can be removed
     */
    CONTROL_Type ctrl;

    ctrl.w = __get_CONTROL();
    ctrl.b.nPRIV = 0;
    __set_CONTROL(ctrl.w);
    __DSB();
    __ISB();
#endif

    struct spm_partition_region_t *serv;

    serv = &g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)];

    if (serv->periph_start) {
        /* Peripheral */
        ppc_clr_secure_unpriv(serv->periph_ppc_bank, serv->periph_ppc_loc);
    }

    mpu_armv8m_disable(&dev_mpu_s);
    mpu_armv8m_region_disable(&dev_mpu_s, PARTITION_REGION_RO);
    mpu_armv8m_region_disable(&dev_mpu_s, PARTITION_REGION_RW_STACK);
    mpu_armv8m_region_disable(&dev_mpu_s, PARTITION_REGION_PERIPH);
    mpu_armv8m_region_disable(&dev_mpu_s, PARTITION_REGION_SHARE);
    mpu_armv8m_enable(&dev_mpu_s, 1, 1);

    return SPM_ERR_OK;
}

uint32_t tfm_spm_partition_get_stack(uint32_t partition_id)
{
    return g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].
            stack_ptr;
}

uint32_t tfm_spm_partition_get_stack_bottom(uint32_t partition_id)
{
    return g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].
            stack_bottom;
}

uint32_t tfm_spm_partition_get_stack_top(uint32_t partition_id)
{
    return
      g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].stack_top;
}

void tfm_spm_partition_set_stack(uint32_t partition_id, uint32_t stack_ptr)
{
    g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].stack_ptr =
            stack_ptr;
}
#endif

uint32_t tfm_spm_partition_get_state(uint32_t partition_id)
{
    return g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].
            partition_state;
}

uint32_t tfm_spm_partition_get_caller_partition_id(uint32_t partition_id)
{
    return g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].
            caller_partition_id;
}

uint32_t tfm_spm_partition_get_orig_psp(uint32_t partition_id)
{
    return
      g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].orig_psp;
}

uint32_t tfm_spm_partition_get_orig_psplim(uint32_t partition_id)
{
    return g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].
            orig_psplim;
}

uint32_t tfm_spm_partition_get_orig_lr(uint32_t partition_id)
{
    return g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].
            orig_lr;
}

uint32_t tfm_spm_partition_get_share(uint32_t partition_id)
{
    return g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].share;
}

void tfm_spm_partition_set_state(uint32_t partition_id, uint32_t state)
{
    g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].
                       partition_state = state;
    if (state == SPM_PARTITION_STATE_RUNNING) {
        g_spm_partition_db.running_partition_id = partition_id;
    }
}

void tfm_spm_partition_set_caller_partition_id(uint32_t partition_id,
                                               uint32_t caller_partition_id)
{
    g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].
            caller_partition_id = caller_partition_id;
}

void tfm_spm_partition_set_orig_psp(uint32_t partition_id,
                                    uint32_t orig_psp)
{
    g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].orig_psp =
            orig_psp;
}

void tfm_spm_partition_set_orig_psplim(uint32_t partition_id,
                                       uint32_t orig_psplim)
{
    g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].orig_psplim =
            orig_psplim;
}

void tfm_spm_partition_set_orig_lr(uint32_t partition_id, uint32_t orig_lr)
{
    g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].orig_lr =
            orig_lr;
}

enum spm_err_t tfm_spm_partition_set_share(uint32_t partition_id,
                                           uint32_t share)
{
    enum spm_err_t ret = SPM_ERR_OK;

#if TFM_LVL != 1
    /* Only need to set configuration on levels higher than 1 */
    ret = tfm_spm_set_share_region(share);
#endif

    if (ret == SPM_ERR_OK) {
        g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)].share =
                share;
    }
    return ret;
}

uint32_t tfm_spm_partition_get_running_partition_id(void)
{
    return g_spm_partition_db.running_partition_id;
}

void tfm_spm_partition_cleanup_context(uint32_t partition_id)
{
    struct spm_partition_region_t *partition =
            &g_spm_partition_db.partitions[PARTITION_ID_GET(partition_id)];
    partition->partition_state = 0;
    partition->caller_partition_id = 0;
    partition->orig_psp = 0;
    partition->orig_psplim = 0;
    partition->orig_lr = 0;
    partition->share = 0;
}
