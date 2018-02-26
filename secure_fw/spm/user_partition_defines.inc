/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __USER_PARTITION_DEFINES_INC__
#define __USER_PARTITION_DEFINES_INC__

/* PARTITION_DECLARE have to be called for a service, before calling
 * any of PARTITION_ADD_INIT_FUNC or PARTITION_ADD_PERIPHERAL
 */

#ifdef TFM_PARTITION_TEST_CORE
/****************************** TFM_SP_CORE_TEST ******************************/
PARTITION_DECLARE(TFM_SP_CORE_TEST);
PARTITION_ADD_INIT_FUNC(TFM_SP_CORE_TEST, core_test_init);
PARTITION_ADD_PERIPHERAL(TFM_SP_CORE_TEST,
    MPS2_IO_FPGAIO_BASE_S, MPS2_IO_FPGAIO_BASE_S + 0xFFF,
    PPC_SP_APB_PPC_EXP2, CMSDK_FPGA_IO_PPC_POS);

/***************************** TFM_SP_CORE_TEST_2 *****************************/
PARTITION_DECLARE(TFM_SP_CORE_TEST_2);
PARTITION_ADD_INIT_FUNC(TFM_SP_CORE_TEST_2, core_test_2_init);
#endif /* TFM_PARTITION_TEST_CORE */

/******************************* TFM_SP_STORAGE *******************************/
PARTITION_DECLARE(TFM_SP_STORAGE);
PARTITION_ADD_INIT_FUNC(TFM_SP_STORAGE, sst_am_prepare);

#ifdef TFM_PARTITION_TEST_SST
/**************************** TFM_SP_SST_TEST_PARTITION ***********************/
PARTITION_DECLARE(TFM_SP_SST_TEST_PARTITION);
PARTITION_ADD_INIT_FUNC(TFM_SP_SST_TEST_PARTITION, sst_test_service_init);
#endif /* TFM_PARTITION_TEST_SST */

#endif /* __USER_PARTITION_DEFINES_INC__ */
