/*
* Copyright (c) 2024, Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*
*/

#include "host_system.h"

#include <stdbool.h>

#include "tfm_hal_device_header.h"

static volatile bool scp_setup_signal_received = false;

int host_system_prepare_ap_access(void)
{
    /*
     * AP cannot be accessed until SCP setup is complete so wait for signal
     * from SCP.
     */
    while (scp_setup_signal_received == false) {
        __WFE();
    }

    return 0;
}

void host_system_scp_signal_ap_ready(void)
{
    scp_setup_signal_received = true;
}
