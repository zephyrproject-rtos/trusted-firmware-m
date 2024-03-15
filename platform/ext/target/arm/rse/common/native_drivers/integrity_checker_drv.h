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
 * \file integrity_checker_drv.h
 * \brief Driver for Arm Integrity Checker.
 */

#ifndef __INTEGRITY_CHECKER_DRV_H__
#define __INTEGRITY_CHECKER_DRV_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INTEGRITY_CHECKER_CONFIG_REMAP_REGION_AM 4

#define INTEGRITY_CHECKER_OUTPUT_SIZE_ZERO_COUNT  4
#define INTEGRITY_CHECKER_OUTPUT_SIZE_CRC32       4
#define INTEGRITY_CHECKER_OUTPUT_SIZE_SHA256     32

#define INTEGRITY_CHECKER_REQUIRED_ALIGNMENT     8

enum integrity_checker_error_t {
    INTEGRITY_CHECKER_ERROR_NONE = 0x0u,
    INTEGRITY_CHECKER_ERROR_OUTPUT_BUFFER_TOO_SMALL,
    INTEGRITY_CHECKER_ERROR_VALUE_BUFFER_TOO_SMALL,
    INTEGRITY_CHECKER_ERROR_UNSUPPORTED_MODE,
    INTEGRITY_CHECKER_ERROR_COMPARISON_FAILED,
    INTEGRITY_CHECKER_ERROR_OPERATION_FAILED,
    INTEGRITY_CHECKER_ERROR_INVALID_ALIGNMENT,
    INTEGRITY_CHECKER_ERROR_INVALID_LENGTH,
};

enum integrity_checker_mode_t {
    INTEGRITY_CHECKER_MODE_ZERO_COUNT = 0b00,
    INTEGRITY_CHECKER_MODE_CRC32      = 0b01,
    INTEGRITY_CHECKER_MODE_SHA256     = 0b10,
};

typedef struct {
    uintptr_t region_base; /*!< The base of the region that will be remapped */
    size_t region_size;    /*!< The size of the region that will be remapped */
    uintptr_t remap_base;  /*!< The base of the region that mapped to */
    size_t remap_cpusel_offset; /*!< How much the remap will be incremented per cpu */
} integrity_checker_remap_region_t;

/**
 * \brief ARM Integrity Checker device configuration structure
 */
struct integrity_checker_dev_cfg_t {
    const uintptr_t base;                         /*!< Integrity Checker base address */
    uint32_t remap_cpusel;
    integrity_checker_remap_region_t remap_regions[INTEGRITY_CHECKER_CONFIG_REMAP_REGION_AM];
};

/**
 * \brief ARM Integrity Checker device structure
 */
struct integrity_checker_dev_t {
    const struct integrity_checker_dev_cfg_t *const cfg;       /*!< Integrity Checker configuration */
};

/**
 * \brief                    Compute an integrity value of a given buffer and
 *                           output it.
 *
 * \param[in]  dev           The Integrity Checker device.
 * \param[in]  mode          Which mode the Integrity Checker should use to
 *                           calculate the integrity value. This may change the
 *                           size of the output value.
 * \param[in]  data          The buffer to calculate the integrity value from.
 * \param[in]  size          The size of the data buffer.
 * \param[out] value         The buffer to write the integrity value into.
 * \param[in]  value_size    The size of the buffer to write the integrity value
 *                           into.
 * \param[out] value_len     The length of the outputted integrity value. This
 *                           value can be NULL if the integrity value length
 *                           isn't needed
 *
 * \return                   INTEGRITY_CHECKER_ERROR_NONE on success, otherwise
 *                           a different integrity_checker_error_t.
 */
enum integrity_checker_error_t integrity_checker_compute_value(struct integrity_checker_dev_t *dev,
                                                               enum integrity_checker_mode_t mode,
                                                               const uint32_t *data, size_t size,
                                                               uint32_t *value, size_t value_size,
                                                               size_t *value_len);

/**
 * \brief                    Check the integrity value of a given buffer against
 *                           a given value.
 *
 * \param[in]  dev           The Integrity Checker device.
 * \param[in]  mode          Which mode the Integrity Checker should use to
 *                           calculate the integrity value. This may change the
 *                           size that will be read from the value buffer.
 * \param[in]  data          The buffer to calculate the integrity value from.
 * \param[in]  size          The size of the data buffer.
 * \param[in]  value         The buffer to read the integrity value from.
 * \param[in]  value_size    The size of the buffer to read the integrity value
 *                           from.
 *
 * \return                   INTEGRITY_CHECKER_ERROR_NONE on success,
 *                           INTEGRITY_CHECKER_ERROR_COMPARISON_FAILED if the
 *                           integrity value does not match the computed value,
 *                           otherwise a different integrity_checker_error_t.
 */
enum integrity_checker_error_t integrity_checker_check_value(struct integrity_checker_dev_t *dev,
                                                             enum integrity_checker_mode_t mode,
                                                             const uint32_t *data, size_t size,
                                                             const uint32_t *value, size_t value_size);


#ifdef __cplusplus
}
#endif

#endif /* __INTEGRITY_CHECKER_DRV_H__ */
