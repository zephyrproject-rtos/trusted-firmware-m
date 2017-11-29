/*
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FIXME: this TFM ID manager is only a stub implementation. It is system
 * integrators responsibility to define a way of identifying the app id and
 * based on their non secure side of the threat model. The secure side only
 * checks if this is an ID belonging to NS side entities. The secure side
 * doesn't make any attempt to challenge the app id value, this is left for NS
 * side privileged code to implement.
 */

#include "tfm_id_mngr.h"

#include <string.h>
#include "cmsis_os2.h"

#define INVALID_APP_ID  0

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

/* Translation table pair between OS threads and SST app IDs */
struct thread_sst_appid_pair {
    const char* t_name;  /*!< Task/Thread name */
    uint32_t    app_id;  /*!< Application ID used in assets definition */
};

static struct thread_sst_appid_pair sst_ns_policy_table[] =
{
    {"Thread_A", 9},
    {"Thread_B", 10},
    {"Thread_C", 11},
};

static const char* get_active_task_name(void)
{
    const char* thread_name;

    thread_name = svcRtxThreadGetName(svcRtxThreadGetId());

    return thread_name;
}

uint32_t tfm_sst_get_cur_id(void)
{
    uint32_t i;
    static uint32_t sst_table_size = (sizeof(sst_ns_policy_table) /
                                      sizeof(sst_ns_policy_table[0]));
    const char* p_thread_name;

    p_thread_name = get_active_task_name();

    for (i = 0; i < sst_table_size; i++) {
        if (strcmp(sst_ns_policy_table[i].t_name, p_thread_name) == 0) {
                return sst_ns_policy_table[i].app_id;
        }
    }

    return INVALID_APP_ID;
}
