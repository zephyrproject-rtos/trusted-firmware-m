/*
 * Copyright (c) 2020-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "region.h"

#ifdef CONFIG_TFM_PARTITION_META
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$RW$$Base);
REGION_DECLARE(Image$$, TFM_SP_META_PTR, $$RW$$Limit);

uintptr_t hal_mem_sp_meta_start =
     (uintptr_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$RW$$Base);
uintptr_t hal_mem_sp_meta_end =
     (uintptr_t)&REGION_NAME(Image$$, TFM_SP_META_PTR, $$RW$$Limit);
#endif
