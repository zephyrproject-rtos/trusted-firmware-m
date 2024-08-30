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
 * \file host_device_definition.h
 * \brief The structure definitions in this file are exported based on the
 * peripheral definitions from device_cfg.h.
 */

#ifndef __HOST_DEVICE_DEFINITION_H__
#define __HOST_DEVICE_DEFINITION_H__

#include "host_device_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLATFORM_HAS_NI_TOWER
#include "ni_tower_lib.h"
extern struct ni_tower_dev SYSCTRL_NI_TOWER_DEV;
extern const struct ni_tower_dev PERIPH_NI_TOWER_DEV;
#endif /* PLATFORM_HAS_NI_TOWER */

#ifdef PLATFORM_HOST_HAS_SCP
#include "mscp_drv.h"
extern struct mscp_dev_t HOST_SCP_DEV;
#endif /* PLATFORM_HOST_HAS_SCP */

#ifdef __cplusplus
}
#endif

#endif  /* __HOST_DEVICE_DEFINITION_H__ */
