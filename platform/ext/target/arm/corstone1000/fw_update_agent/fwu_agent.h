/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef FWU_AGENT_H
#define FWU_AGENT_H

/* Set 1 to enable debug messages */
#define ENABLE_DEBUG_LOGS         1

#if (ENABLE_DEBUG_LOGS == 1)
    #include <stdio.h>
    #define FWU_LOG_MSG(f_, ...) printf((f_), ##__VA_ARGS__)
#else
    #define FWU_LOG_MSG(f_, ...)
#endif

enum fwu_agent_error_t {
        FWU_AGENT_SUCCESS = 0,
        FWU_AGENT_ERROR = (-1)
};

enum fwu_agent_error_t fwu_metadata_provision(void);
enum fwu_agent_error_t fwu_metadata_init(void);


#endif /* FWU_AGENT_H */

