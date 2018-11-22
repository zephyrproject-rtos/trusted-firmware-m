/*
 * Copyright (c) 2018 ARM Limited
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

#include "platform/include/tfm_plat_device_id.h"

/*!
 * \def DEVICE_ID_SIZE
 *
 * \brief Size of device ID in bytes
 */
#define DEVICE_ID_SIZE (16)

/*!
 * \def DEVICE_ID
 *
 * \brief Fixed value for device ID.
 */
#define DEVICE_ID    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, \
                     0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF

static const uint8_t device_id[DEVICE_ID_SIZE] = {DEVICE_ID};

int32_t tfm_plat_get_device_id(uint32_t size, uint8_t *buf)
{
    /* FixMe: This getter function must be ported per target platform. */

    uint32_t i;
    uint8_t *p_dst = buf;
    const uint8_t *p_src = device_id;

    if (size < DEVICE_ID_SIZE) {
        return -1;
    }

    for (i = DEVICE_ID_SIZE; i > 0; i--) {
        *p_dst = *p_src;
        p_src++;
        p_dst++;
    }

    return DEVICE_ID_SIZE;
}
