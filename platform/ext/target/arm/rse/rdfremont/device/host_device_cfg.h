/*
 * Copyright (c) 2023-2024 Arm Limited. All rights reserved.
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

#ifndef __HOST_DEVICE_CFG_H__
#define __HOST_DEVICE_CFG_H__

/* MHU */
#define MHU0_S
#define MHU4_S

#define MHU_RSE_TO_AP_MONITOR_DEV MHU0_SENDER_DEV_S
#define MHU_AP_MONITOR_TO_RSE_DEV MHU0_RECEIVER_DEV_S
#define MHU_RSE_TO_SCP_DEV        MHU4_SENDER_DEV_S
#define MHU_SCP_TO_RSE_DEV        MHU4_RECEIVER_DEV_S

/* NI-Tower */
#ifdef PLATFORM_HAS_NI_TOWER
#define RD_SYSCTRL_NI_TOWER
#define RD_PERIPH_NI_TOWER
#endif /* PLATFORM_HAS_NI_TOWER */

/* MSCP */
#ifdef PLATFORM_HOST_HAS_SCP
#define HOST_SCP
#endif /* PLATFORM_HOST_HAS_SCP */
#ifdef PLATFORM_HOST_HAS_MCP
#define HOST_MCP
#endif /* PLATFORM_HOST_HAS_MCP */

/* SMMU */
#ifdef PLATFORM_HAS_SMMU_V3
#define HOST_SMMU
#endif /* PLATFORM_HAS_SMMU_V3 */

#endif /* __HOST_DEVICE_CFG_H__ */
