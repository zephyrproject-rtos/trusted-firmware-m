/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <string.h>

#include "tfm_nspm_svc_handler.h"
#include "cmsis_os2.h"
#include "tfm_api.h"
#include "tfm_ns_svc.h"

/* FIXME: following two functions are meant to be internally
 * available to RTX. The header file containing prototype of
 * these functions has complex header inclusion which leads
 * to compiler specific paths in CMSIS, which currently doesn't have
 * clang variant. To simplify this, following functions are directly
 * declared here (as opposed to header inclusion). After clear
 * separation of S and NS builds this will require to be revisited
 */
extern osThreadId_t svcRtxThreadGetId(void);
extern const char *svcRtxThreadGetName(osThreadId_t thread_id);

/* Translation table pair between OS threads and SST client IDs */
struct thread_sst_clientid_pair {
    const char* t_name;     /*!< Task/Thread name */
    int32_t     client_id;  /*!< Client ID used in assets definition */
};

static struct thread_sst_clientid_pair sst_ns_policy_table[] =
{
    {"Thread_A", -9},
    {"Thread_B", -10},
    {"Thread_C", -11},
    {"Thread_D", -12},
    {"seq_task", -13},
    {"mid_task", -14},
    {"pri_task", -15},
};

static const char* get_active_task_name(void)
{
    const char* thread_name;

    thread_name = svcRtxThreadGetName(svcRtxThreadGetId());

    return thread_name;
}

/* SVC function implementations */
uint32_t tfm_nspm_svc_register_client_id()
{
    int32_t client_id_ns;
    uint32_t i;
    static uint32_t sst_table_size = (sizeof(sst_ns_policy_table) /
                                      sizeof(sst_ns_policy_table[0]));
    const char* p_thread_name;

    p_thread_name = get_active_task_name();

    for (i = 0; i < sst_table_size; i++) {
        if (strcmp(sst_ns_policy_table[i].t_name, p_thread_name) == 0) {
            client_id_ns = sst_ns_policy_table[i].client_id;
            if (tfm_register_client_id(client_id_ns) == TFM_SUCCESS) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    return 0;
}
