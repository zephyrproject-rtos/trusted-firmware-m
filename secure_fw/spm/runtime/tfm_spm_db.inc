/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SPM_DB_INC__
#define __TFM_SPM_DB_INC__

#ifdef TFM_PSA_API
#include "tfm_spm_db_ipc.inc"
#else
#include "tfm_spm_db_func.inc"
#endif

#endif /* __TFM_SPM_DB_INC__ */