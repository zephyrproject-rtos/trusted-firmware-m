/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* The file is meant to be included twice */
#ifndef __SPM_DECLARE_USER_PARTITIONS__
#define __SPM_DECLARE_USER_PARTITIONS__

#include "platform_retarget.h"
#include "target_cfg.h"
#include "service_defs.h"

/* TFM_SP_NON_SECURE and TFM_SP_CORE are not real partitions, we
 * only created them to have an entry for them in the database. They don't have
 * their dedicated sections in the scatter file, so no symbols are needed to
 * be declared.
 */
/* PARTITION_DECLARE(TFM_SP_NON_SECURE) */
/* PARTITION_DECLARE(TFM_SP_CORE) */
PARTITION_DECLARE(TFM_SP_STORAGE)

#ifdef TFM_PARTITION_TEST_CORE
PARTITION_DECLARE(TFM_SP_CORE_TEST)
PARTITION_DECLARE(TFM_SP_CORE_TEST_2)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SST
PARTITION_DECLARE(TFM_SP_SST_TEST_PARTITION)
#endif /* TFM_PARTITION_TEST_SST */

#elif !defined(__SPM_ADD_USER_PARTITIONS__) /*__SPM_DECLARE_USER_PARTITIONS__*/
#define __SPM_ADD_USER_PARTITIONS__

/* Order must be same as id!!! */
DUMMY_PARTITION_ADD(TFM_SP_NON_SECURE)
DUMMY_PARTITION_ADD(TFM_SP_CORE)
PARTITION_ADD(TFM_SP_STORAGE)

#ifdef TFM_PARTITION_TEST_CORE
PARTITION_ADD(TFM_SP_CORE_TEST)
PARTITION_ADD(TFM_SP_CORE_TEST_2)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SST
PARTITION_ADD(TFM_SP_SST_TEST_PARTITION)
#endif /* TFM_PARTITION_TEST_SST */

PARTITION_ADD_INIT_FUNC(TFM_SP_STORAGE, sst_am_prepare)

#ifdef TFM_PARTITION_TEST_CORE
PARTITION_ADD_INIT_FUNC(TFM_SP_CORE_TEST, core_test_init)
PARTITION_ADD_PERIPHERAL(TFM_SP_CORE_TEST,
    MPS2_IO_FPGAIO_BASE_S, MPS2_IO_FPGAIO_BASE_S + 0xFFF,
    PPC_SP_APB_PPC_EXP2, CMSDK_FPGA_IO_PPC_POS)
PARTITION_ADD_INIT_FUNC(TFM_SP_CORE_TEST_2, core_test_2_init)
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SST
PARTITION_ADD_INIT_FUNC(TFM_SP_SST_TEST_PARTITION, sst_test_service_init)
#endif /* TFM_PARTITION_TEST_SST */

#else /* __SPM_ADD_USER_PARTITIONS__ */
#error "unexpected inclusion of the file"
#endif /* __SPM_ADD_USER_PARTITIONS__ */
