/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "fih.h"

void tfm_idle_thread(void)
{
    while (1) {
        __WFI();
    }

#ifdef TFM_FIH_PROFILE_ON
    fih_delay();

    while (1) {
        __WFI();
    }
#endif
}
