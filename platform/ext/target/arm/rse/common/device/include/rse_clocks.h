/*
 * Copyright (c) 2024 Arm Limited. All rights reserved.
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

#ifndef __RSE_CLOCKS_H__
#define __RSE_CLOCKS_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure the RSE clocks on boot.
 *
 * \return Returns 0 on success, -1 on failure.
 */
int rse_clock_config(void);

/**
 * \brief Get the current RSE SYSCLK/CPU0CLK.
 *
 * \return Returns SYSCLK value.
 */
uint32_t rse_get_sysclk(void);

#ifdef __cplusplus
}
#endif

#endif /* __RSE_CLOCKS_H__ */
