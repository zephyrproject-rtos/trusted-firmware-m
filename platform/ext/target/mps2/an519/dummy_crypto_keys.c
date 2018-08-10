/*
 * Copyright (c) 2017-2018 ARM Limited
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

#include "platform/include/tfm_plat_crypto_keys.h"

#define TFM_KEY_LEN_BYTES  16

static const uint8_t sample_tfm_key[TFM_KEY_LEN_BYTES] =
             {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
              0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

enum tfm_plat_err_t tfm_plat_get_crypto_huk(uint8_t *key, uint32_t size)
{
    /* FIXME: this function should be implemented by platform vendor. For the
     * security of the storage system, it is critical to use a hardware unique
     * key.
     *
     * AN519 does not have any available hardware unique key engine, so a
     * software stub has been implemented in this case.
     */
    uint32_t i;
    uint8_t *p_dst = key;
    const uint8_t *p_huk = sample_tfm_key;

    if(size > TFM_KEY_LEN_BYTES) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }

    for (i = size; i > 0; i--) {
        *p_dst = *p_huk;
        p_huk++;
        p_dst++;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

