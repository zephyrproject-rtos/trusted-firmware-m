/*
 * Copyright (c) 2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file host_device_definition.c
 * \brief This file defines exports the structures based on the peripheral
 * definitions from device_cfg.h.
 */

#include "host_device_definition.h"
#include "platform_base_address.h"

/* Message Handling Units (MHU) */
#ifdef MHU_V3_AP_MONITOR_TO_RSS
struct mhu_v3_x_dev_t MHU_AP_MONITOR_TO_RSS_DEV = {
    .base = MHU0_RECEIVER_BASE_S,
    .frame = MHU_V3_X_MBX_FRAME,
    .subversion = 0
};
#endif

#ifdef MHU_V3_RSS_TO_AP_MONITOR
struct mhu_v3_x_dev_t MHU_RSS_TO_AP_MONITOR_DEV = {
    .base = MHU0_SENDER_BASE_S,
    .frame = MHU_V3_X_PBX_FRAME,
    .subversion = 0
};
#endif

#ifdef MHU_V3_SCP_TO_RSS
struct mhu_v3_x_dev_t MHU_V3_SCP_TO_RSS_DEV = {
    .base = MHU4_RECEIVER_BASE_S,
    .frame = MHU_V3_X_MBX_FRAME,
    .subversion = 0
};
#endif

#ifdef MHU_V3_RSS_TO_SCP
struct mhu_v3_x_dev_t MHU_V3_RSS_TO_SCP_DEV = {
    .base = MHU4_SENDER_BASE_S,
    .frame = MHU_V3_X_PBX_FRAME,
    .subversion = 0
};
#endif
