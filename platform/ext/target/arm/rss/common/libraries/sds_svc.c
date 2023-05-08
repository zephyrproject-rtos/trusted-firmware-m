/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sds.h"
#include "tfm_plat_defs.h"
#include "platform_svc_numbers.h"

__attribute__((naked))
enum tfm_plat_err_t sds_struct_add(const struct sds_structure_desc *struct_desc)
{
    /* Platform SVC handlers can be added for secure world in TF-M */
    __asm("SVC %0\n"
          "BX LR\n"
          : : "I" (TFM_SVC_PLATFORM_SDS_STRUCT_ADD));
}
