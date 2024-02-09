/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
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

/**
 * \file tram_drv.c
 * \brief Driver for Arm TRAM.
 */

#include "tram_drv.h"

#include "tfm_hal_device_header.h"

#ifdef TRAM_CONFIG_EXTERNAL_DPA_HARDENED_WORD_COPY
#include "dpa_hardened_word_copy.h"
#endif /* TRAM_CONFIG_EXTERNAL_DPA_HARDENED_WORD_COPY */

struct _tram_reg_map_t {
    __IM  uint32_t trbc;
                /*!< Offset: 0x000 (R/ ) TRAM Build Configuration Register */
    __IOM uint32_t trc;
                /*!< Offset: 0x004 (R/W) TRAM Configuration Register */
    __OM  uint32_t trkey[8];
                /*!< Offset: 0x008 ( /W) TRAM Key Register */
    __IOM uint32_t reserved_0[0x3E9];
                /*!< Offset: 0x28-0xFCC Reserved */
    __IM uint32_t pidr4;
                /*!< Offset: 0xFD0 (R/ ) Peripheral ID 4 */
    __IOM uint32_t reserved_1[3];
                /*!< Offset: 0xFD4-0xFDC Reserved */
    __IM  uint32_t pidr0;
                /*!< Offset: 0xFE0 (R/ ) Peripheral ID 0 */
    __IM  uint32_t pidr1;
                /*!< Offset: 0xFE4 (R/ ) Peripheral ID 1 */
    __IM  uint32_t pidr2;
                /*!< Offset: 0xFE8 (R/ ) Peripheral ID 2 */
    __IM  uint32_t pidr3;
                /*!< Offset: 0xFEC (R/ ) Peripheral ID 3 */
    __IM  uint32_t cidr0;
                /*!< Offset: 0xFF0 (R/ ) Component ID 0 */
    __IM  uint32_t cidr1;
                /*!< Offset: 0xFF4 (R/ ) Component ID 1 */
    __IM  uint32_t cidr2;
                /*!< Offset: 0xFF8 (R/ ) Component ID 2 */
    __IM  uint32_t cidr3;
                /*!< Offset: 0xFFC (R/ ) Component ID 3 */
};

void tram_set_key(struct tram_dev_t *dev, const uint32_t *key)
{
    struct _tram_reg_map_t* p_tram = (struct _tram_reg_map_t*)dev->cfg->base;
    uint32_t idx;

#ifndef TRAM_CONFIG_EXTERNAL_DPA_HARDENED_WORD_COPY
    for (idx = 0; idx < (sizeof(p_tram->trkey) / sizeof(uint32_t)); idx++) {
        p_tram->trkey[idx] = key[idx];
    }
#else
    (void)idx;
    dpa_hardened_word_copy(p_tram->trkey, key, (sizeof(p_tram->trkey) / sizeof(uint32_t));
#endif /* TRAM_CONFIG_EXTERNAL_DPA_HARDENED_WORD_COPY */
}

void tram_wipe_key(struct tram_dev_t *dev)
{
    struct _tram_reg_map_t* p_tram = (struct _tram_reg_map_t*)dev->cfg->base;

    p_tram->trc |= (0b1 << 1);
}

void tram_enable_encryption(struct tram_dev_t *dev)
{
    struct _tram_reg_map_t* p_tram = (struct _tram_reg_map_t*)dev->cfg->base;

    p_tram->trc |= 0b1;
}

void tram_disable_encryption(struct tram_dev_t *dev)
{
    struct _tram_reg_map_t* p_tram = (struct _tram_reg_map_t*)dev->cfg->base;

    p_tram->trc &= ~0b1;
}
