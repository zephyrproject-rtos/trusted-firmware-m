/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RUNTIME_DEFS_H__
#define __RUNTIME_DEFS_H__

#include <stdint.h>

#include "psa/client.h"
#include "psa/error.h"
#include "psa/service.h"

/* SFN defs */
typedef psa_status_t (*service_fn_t)(psa_msg_t *msg);
typedef psa_status_t (*sfn_init_fn_t)(void);

/* PSA API dispatcher for IPC model. */
#if CONFIG_TFM_SPM_BACKEND_IPC == 1

typedef psa_status_t (*psa_call_fn_t)(psa_handle_t, uint32_t,
                                      const psa_invec *in_vec,
                                      psa_outvec *out_vec);
typedef psa_handle_t (*psa_connect_fn_t)(uint32_t, uint32_t);
typedef void         (*psa_close_fn_t)(psa_handle_t);
typedef uint32_t     (*psa_version_fn_t)(uint32_t);
typedef uint32_t     (*psa_framework_version_fn_t)(void);

struct psa_api_tbl_t {
    psa_call_fn_t              psa_call;
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1
    psa_connect_fn_t           psa_connect;
    psa_close_fn_t             psa_close;
#endif
    psa_version_fn_t           psa_version;
    psa_framework_version_fn_t psa_framework_version;
};

struct runtime_metadata_t {
    uintptr_t            entry;      /* Entry function */
    struct psa_api_tbl_t *psa_fns;   /* PSA API entry table */
    uint32_t             n_sfn;      /* Number of Secure FuNctions */
    service_fn_t         sfn_table[];/* Secure FuNctions Table */
};
#endif /* CONFIG_TFM_SPM_BACKEND_IPC == 1 */

#endif /* __RUNTIME_DEFS_H__ */
