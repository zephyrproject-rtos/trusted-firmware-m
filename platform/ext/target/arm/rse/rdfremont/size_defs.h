/*
 * Copyright (c) 2023-2024 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* BL2 */
#define SIZE_DEF_BL2_IMAGE          (0x20000) /* 128 KB */

/* Secure Firmware */
#define SIZE_DEF_S_IMAGE            (0x60000) /* 384 KB */

/* Non-Secure Firmware */
#define SIZE_DEF_NS_IMAGE           (0x60000) /* 384 KB */

/* Host Firmware */
#define SIZE_DEF_SCP_IMAGE          (0x80000) /* 512 KB */
#define SIZE_DEF_MCP_IMAGE          (0x80000) /* 512 KB */
#define SIZE_DEF_LCP_IMAGE          (0x10000) /* 64 KB */
#define SIZE_DEF_AP_BL1_IMAGE       (0x80000) /* 512 KB */
