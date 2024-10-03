/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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
 * \file host_device_cfg.h
 * \brief This file contains defines of which devices are present in the
 *        platform based on the configurations.
 *        Currently, the file is empty as all devices are mandatory in RD1AE
 *        and the file is required by the RSE common code.
 */

#ifndef __HOST_DEVICE_CFG_H__
#define __HOST_DEVICE_CFG_H__

/* MHU */
#define MHU0_S
#define MHU2_S
#define MHU4_S

#define MHU_RSE_TO_AP_MONITOR_DEV MHU0_SENDER_DEV_S
#define MHU_AP_MONITOR_TO_RSE_DEV MHU0_RECEIVER_DEV_S
#define MHU_RSE_TO_AP_S_DEV       MHU2_SENDER_DEV_S
#define MHU_AP_S_TO_RSE_DEV       MHU2_RECEIVER_DEV_S
#define MHU_RSE_TO_SCP_DEV        MHU4_SENDER_DEV_S
#define MHU_SCP_TO_RSE_DEV        MHU4_RECEIVER_DEV_S

#endif /* __HOST_DEVICE_CFG_H__ */
