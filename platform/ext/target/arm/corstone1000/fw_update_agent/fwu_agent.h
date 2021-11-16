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

#define FWU_ASSERT(_c_)                                                        \
                if (!(_c_)) {                                                  \
                    FWU_LOG_MSG("%s:%d assert hit\n\r", __func__, __LINE__);   \
                    while(1) {};                                               \
                }                                                              \


enum fwu_agent_error_t fwu_metadata_provision(void);
enum fwu_agent_error_t fwu_metadata_init(void);

/* host to secure enclave:
 * firwmare update image is sent accross
 */
enum fwu_agent_error_t corstone1000_fwu_flash_image(void);

/* host to secure enclave:
 * host responds with this api to acknowledge its successful
 * boot.
 */
enum fwu_agent_error_t corstone1000_fwu_host_ack(void);

void bl1_get_boot_bank(uint32_t *bank_offset);
void bl2_get_boot_bank(uint32_t *bank_offset);

#endif /* FWU_AGENT_H */
