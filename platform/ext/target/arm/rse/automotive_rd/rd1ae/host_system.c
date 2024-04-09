/*
* Copyright (c) 2024, Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*
*/

#include <stdbool.h>

#include "device_definition.h"
#include "host_base_address.h"
#include "host_system.h"
#include "tfm_hal_device_header.h"

static volatile bool scp_setup_signal_received = false;

/*
 * Initializes the ATU region before configuring the NI-Tower. This function
 * maps the physical base address of the NI-Tower instance received as the
 * parameter to a logical address HOST_NI_TOWER_BASE.
 */
static int ni_tower_pre_init(uint64_t ni_tower_phys_address)
{
    enum atu_error_t atu_err;
    enum atu_roba_t roba_value;

    atu_err = atu_initialize_region(
                &ATU_DEV_S,
                HOST_NI_TOWER_ATU_ID,
                HOST_NI_TOWER_BASE,
                ni_tower_phys_address,
                HOST_NI_TOWER_SIZE);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    roba_value = ATU_ROBA_SET_1;
    atu_err = set_axnsc(&ATU_DEV_S, roba_value, HOST_NI_TOWER_ATU_ID);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    roba_value = ATU_ROBA_SET_0;
    atu_err = set_axprot1(&ATU_DEV_S, roba_value, HOST_NI_TOWER_ATU_ID);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    return 0;
}

/* Un-initializes the ATU region after configuring the NI-Tower */
static int ni_tower_post_init(void)
{
    enum atu_error_t atu_err;

    atu_err = atu_uninitialize_region(&ATU_DEV_S, HOST_NI_TOWER_ATU_ID);
    if (atu_err != ATU_ERR_NONE) {
        return -1;
    }

    return 0;
}

/*
 * Programs the System control NI-Tower for nodes under Always-On (AON) domain.
 */
static int ni_tower_sysctrl_aon_init(void)
{
    int err;

    err = ni_tower_pre_init(HOST_SYSCTRL_NI_TOWER_PHYS_BASE);
    if (err != 0) {
        return err;
    }

    err = program_sysctrl_ni_tower_aon();
    if (err != 0) {
        return err;
    }

    err = ni_tower_post_init();
    if (err != 0) {
        return err;
    }

    return 0;
}

int host_system_prepare_scp_access(void)
{
    int res;

    /* Configure System Control NI-Tower for nodes under AON power domain */
    res = ni_tower_sysctrl_aon_init();
    if (res != 0) {
        return res;
    }

    return 0;
}

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
