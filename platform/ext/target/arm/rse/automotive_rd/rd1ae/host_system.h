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

int host_system_prepare_ap_access(void);

/* Prepares SCP access for host system */
int host_system_prepare_scp_access(void);

void host_system_scp_signal_ap_ready(void);

#ifdef __cplusplus
}
#endif
#endif /* __HOST_SYSTEM_H__ */
