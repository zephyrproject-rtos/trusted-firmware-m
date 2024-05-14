/*
* Copyright (c) 2024, Arm Limited. All rights reserved.
*
* SPDX-License-Identifier: BSD-3-Clause
*
*/

#ifndef __HOST_SYSTEM_H__
#define __HOST_SYSTEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/* Struct containing fixed data about the host system */
struct host_system_info_t {
    /* chip ID of system */
    uint32_t chip_id;
    /* base address of the ap region on this chip */
    uint64_t chip_ap_phys_base;
};

/* Struct containing data about the current status of the host system */
struct host_system_status_t {
    /* Whether the SCP has finished setting up the SYSTOP power domain */
    volatile bool scp_systop_ready;
};

/* Struct containing all data about the host system */
struct host_system_t {
    /* Struct containing fixed data about the host system */
    struct host_system_info_t info;
    /* Struct containing data about the current status of the host system */
    struct host_system_status_t status;
    /* Whether or not this struct has been initialized */
    bool initialized;
};

/* Initialize host system by collecting fixed data about the host system */
int host_system_init(void);

/* Get info struct containing fixed data about the host system */
int host_system_get_info(struct host_system_info_t **info);

int host_system_prepare_ap_access(void);

/* Prepares MSCP access for host system */
int host_system_prepare_mscp_access(void);

void host_system_scp_signal_ap_ready(void);

/* Finishes host system preparations */
int host_system_finish(void);

#ifdef __cplusplus
}
#endif
#endif /* __HOST_SYSTEM_H__ */
