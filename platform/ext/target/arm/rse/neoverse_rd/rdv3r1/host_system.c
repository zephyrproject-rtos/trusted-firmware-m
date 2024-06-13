/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "device_definition.h"
#include "host_base_address.h"
#include "host_system.h"
#include "tfm_hal_device_header.h"
#include "tfm_plat_otp.h"

static struct host_system_t host_system_data = {0};

/* Read Chip ID from OTP */
static int read_chip_id(uint32_t *chip_id)
{
    int err;
    uint32_t otp_chip_id;

    err = tfm_plat_otp_read(PLAT_OTP_ID_RSE_ID,
                            sizeof(otp_chip_id),
                            (uint8_t*)&otp_chip_id);
    if (err != 0)
        return err;

    *chip_id = otp_chip_id;
    return 0;
}

/* Initialize host system by collecting fixed data about the host system */
int host_system_init(void)
{
    int res;

    res = read_chip_id(&host_system_data.info.chip_id);
    if (res != 0) {
        host_system_data.info.chip_id = 0;
        return res;
    }
    host_system_data.info.chip_ap_phys_base =
                    HOST_AP_CHIP_N_PHYS_BASE(host_system_data.info.chip_id);

    host_system_data.initialized = true;
    return 0;
}

/* Get info struct containing fixed data about the host system */
int host_system_get_info(struct host_system_info_t **info)
{
    if (info == NULL) {
        return -1;
    }

    if (host_system_data.initialized == false) {
        return -1;
    }

    *info = &host_system_data.info;
    return 0;
}

int host_system_prepare_mscp_access(void)
{
    return 0;
}

int host_system_prepare_ap_access(void)
{
    int res;

    (void)res;

    /*
     * AP cannot be accessed until SCP setup is complete so wait for signal
     * from SCP.
     */
    while (host_system_data.status.scp_systop_ready == false) {
        __WFE();
    }

    return 0;
}

void host_system_scp_signal_ap_ready(void)
{
    host_system_data.status.scp_systop_ready = true;
}

int host_system_finish(void)
{
    int res;

    (void)res;

    return 0;
}
