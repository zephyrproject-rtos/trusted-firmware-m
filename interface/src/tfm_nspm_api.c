/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>

#include "tfm_ns_svc.h"

__attribute__ ((naked)) uint32_t tfm_nspm_register_client_id(void)
{
    SVC(SVC_TFM_NSPM_REGISTER_CLIENT_ID);
    __ASM("BX LR");
}


