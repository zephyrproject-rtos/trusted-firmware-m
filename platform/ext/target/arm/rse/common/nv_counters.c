/*
 * Copyright (c) 2018-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* NOTE: For the security of the protected storage system, the bootloader
 * rollback protection, and the protection of cryptographic material  it is
 * CRITICAL to use a internal (in-die) persistent memory for the implementation
 * of the OTP_NV_COUNTERS flash area (see flash_otp_nv_layout.c).
 */

#include "tfm_plat_nv_counters.h"

#include "device_definition.h"
#include "tfm_plat_otp.h"
#include "psa_manifest/pid.h"
#include <limits.h>
#include <string.h>

#define OTP_COUNTER_MAX_SIZE    (16u * sizeof(uint32_t))
#define NV_COUNTER_SIZE         4
#define OTP_COUNTER_MAGIC       0x3333CAFE

enum tfm_plat_err_t tfm_plat_init_nv_counter(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

#ifdef RSE_BIT_PROGRAMMABLE_OTP
#ifdef INTEGRITY_CHECKER_S
static enum tfm_plat_err_t count_zero_bits(const uint32_t *addr, uint32_t len,
                                           uint32_t *zero_bits)
{
    enum integrity_checker_error_t ic_err;

    ic_err = integrity_checker_compute_value(&INTEGRITY_CHECKER_DEV_S,
                                             INTEGRITY_CHECKER_MODE_ZERO_COUNT,
                                             addr, len, zero_bits, sizeof(uint32_t),
                                             NULL);

    if (ic_err != INTEGRITY_CHECKER_ERROR_NONE) {
        return ic_err;
    }

    return TFM_PLAT_ERR_SUCCESS;
}
#else
static enum tfm_plat_err_t count_zero_bits(const uint32_t *addr, uint32_t len,
                                           uint32_t *zero_bits)
{
    uint32_t idx;
    uint32_t word;
    uint32_t bit_index;

    *zero_bits = 0;

    for (idx = 0; idx < len / sizeof(uint32_t); idx ++) {
        word = addr[idx];

        for (bit_index = 0; bit_index < sizeof(word) * 8; bit_index++) {
            *zero_bits += 1 - ((word >> bit_index) & 1);
        }
    }

    return TFM_PLAT_ERR_SUCCESS;
}
#endif /* INTEGRITY_CHECKER_DEV_S */

static enum tfm_plat_err_t read_otp_counter(enum tfm_otp_element_id_t id,
                                            uint8_t *val)
{
    size_t counter_size;
    enum tfm_plat_err_t err;
    uint32_t counter_value[OTP_COUNTER_MAX_SIZE / sizeof(uint32_t)] = {0};
    uint32_t count;

    err = tfm_plat_otp_get_size(id, &counter_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    counter_size = counter_size > OTP_COUNTER_MAX_SIZE ? OTP_COUNTER_MAX_SIZE
                                                       : counter_size;

    err = tfm_plat_otp_read(id, counter_size, (uint8_t *)counter_value);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    err = count_zero_bits(counter_value, counter_size, &count);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    count = (counter_size * 8) - count;
    memcpy(val, &count, NV_COUNTER_SIZE);

    return TFM_PLAT_ERR_SUCCESS;
}
#else
static enum tfm_plat_err_t read_otp_counter(enum tfm_otp_element_id_t id,
                                            uint8_t *val)
{
    size_t counter_size;
    enum tfm_plat_err_t err;
    size_t idx;
    uint32_t counter_value[OTP_COUNTER_MAX_SIZE / sizeof(uint32_t)] = {0};
    uint32_t count;

    err = tfm_plat_otp_get_size(id, &counter_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    counter_size = counter_size > OTP_COUNTER_MAX_SIZE ? OTP_COUNTER_MAX_SIZE
                                                       : counter_size;

    err = tfm_plat_otp_read(id, counter_size, (uint8_t *)counter_value);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    count = 0;
    for (idx = 0; idx < counter_size / sizeof(uint32_t); idx++) {
        if (counter_value[idx] == OTP_COUNTER_MAGIC) {
            count += 1;
        } else if (counter_value[idx] == 0) {
            break;
        } else {
            return TFM_PLAT_ERR_READ_OTP_COUNTER_SYSTEM_ERR;
        }
    }

    memcpy(val, &count, NV_COUNTER_SIZE);

    return TFM_PLAT_ERR_SUCCESS;
}
#endif /* RSE_BIT_PROGRAMMABLE_OTP */

enum tfm_plat_err_t tfm_plat_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                             uint32_t size, uint8_t *val)
{
    if (size != NV_COUNTER_SIZE) {
        return TFM_PLAT_ERR_READ_NV_COUNTER_INVALID_COUNTER_SIZE;
    }

    /* Assumes Platform nv counters are contiguous*/
    if (counter_id >= PLAT_NV_COUNTER_BL2_0 &&
        counter_id < (PLAT_NV_COUNTER_BL2_0 + MCUBOOT_IMAGE_NUMBER)) {
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_BL2_0 +
                                       (counter_id - PLAT_NV_COUNTER_BL2_0),
                                   val);
    }

    switch (counter_id) {
#ifdef PLATFORM_HAS_PS_NV_OTP_COUNTERS
    case (PLAT_NV_COUNTER_PS_0):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_0, val);
    case (PLAT_NV_COUNTER_PS_1):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_1, val);
    case (PLAT_NV_COUNTER_PS_2):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_2, val);
#endif /* PLATFORM_HAS_PS_NV_OTP_COUNTERS */
    case (PLAT_NV_COUNTER_NS_0):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_0, val);
    case (PLAT_NV_COUNTER_NS_1):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_1, val);
    case (PLAT_NV_COUNTER_NS_2):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_2, val);

    case (PLAT_NV_COUNTER_BL1_0):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_BL1_0, val);

    case (PLAT_NV_COUNTER_ATTACK_TRACKING):
        return read_otp_counter(PLAT_OTP_ID_ATTACK_TRACKING_BITS, val);

    default:
        return TFM_PLAT_ERR_READ_NV_COUNTER_UNSUPPORTED;
    }
}


#ifdef RSE_BIT_PROGRAMMABLE_OTP
static enum tfm_plat_err_t set_otp_counter(enum tfm_otp_element_id_t id,
                                           uint32_t val)
{
    size_t counter_size;
    enum tfm_plat_err_t err;
    size_t idx;
    uint32_t counter_value[OTP_COUNTER_MAX_SIZE / sizeof(uint32_t)] = {0};

    err = tfm_plat_otp_get_size(id, &counter_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    counter_size = counter_size > OTP_COUNTER_MAX_SIZE ? OTP_COUNTER_MAX_SIZE
                                                       : counter_size;

    if (val > counter_size * 8) {
        return TFM_PLAT_ERR_SET_OTP_COUNTER_MAX_VALUE;
    }

    for (idx = 0; idx < val / 32; idx++) {
        counter_value[idx] = 0xFFFFFFFFu;
    }

    if (val % 32 != 0) {
        counter_value[idx] = (1 << (val % 32)) - 1;
    }

    err = tfm_plat_otp_write(id, counter_size,
                             (uint8_t *)counter_value);

    return err;
}
#else
static enum tfm_plat_err_t set_otp_counter(enum tfm_otp_element_id_t id,
                                           uint32_t val)
{
    size_t counter_size;
    enum tfm_plat_err_t err;
    size_t idx;
    uint32_t counter_value[OTP_COUNTER_MAX_SIZE / sizeof(uint32_t)] = {0};

    err = tfm_plat_otp_get_size(id, &counter_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    counter_size = counter_size > OTP_COUNTER_MAX_SIZE ? OTP_COUNTER_MAX_SIZE
                                                       : counter_size;

    if (val > (counter_size / sizeof(uint32_t))) {
        return TFM_PLAT_ERR_SET_OTP_COUNTER_MAX_VALUE;
    }

    for (idx = 0; idx < val; idx++) {
        counter_value[idx] = OTP_COUNTER_MAGIC;
    }

    err = tfm_plat_otp_write(id, counter_size,
                             (uint8_t *)counter_value);

    return err;
}
#endif /* RSE_BIT_PROGRAMMABLE_OTP */

enum tfm_plat_err_t tfm_plat_set_nv_counter(enum tfm_nv_counter_t counter_id,
                                            uint32_t value)
{
    /* Assumes Platform nv counters are contiguous*/
    if (counter_id >= PLAT_NV_COUNTER_BL2_0 &&
        counter_id < (PLAT_NV_COUNTER_BL2_0 + MCUBOOT_IMAGE_NUMBER)) {
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_BL2_0 +
                                      (counter_id - PLAT_NV_COUNTER_BL2_0),
                                  value);
    }

    switch (counter_id) {
#ifdef PLATFORM_HAS_PS_NV_OTP_COUNTERS
    case (PLAT_NV_COUNTER_PS_0):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_0, value);
    case (PLAT_NV_COUNTER_PS_1):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_1, value);
    case (PLAT_NV_COUNTER_PS_2):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_2, value);
#endif /* PLATFORM_HAS_PS_NV_OTP_COUNTERS */

    case (PLAT_NV_COUNTER_NS_0):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_0, value);
    case (PLAT_NV_COUNTER_NS_1):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_1, value);
    case (PLAT_NV_COUNTER_NS_2):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_2, value);

    case (PLAT_NV_COUNTER_BL1_0):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_BL1_0, value);

    case (PLAT_NV_COUNTER_ATTACK_TRACKING):
        return set_otp_counter(PLAT_OTP_ID_ATTACK_TRACKING_BITS, value);

    default:
        return TFM_PLAT_ERR_SET_NV_COUNTER_UNSUPPORTED;
    }
}

enum tfm_plat_err_t tfm_plat_increment_nv_counter(
                                           enum tfm_nv_counter_t counter_id)
{
    uint32_t security_cnt;
    enum tfm_plat_err_t err;

    err = tfm_plat_read_nv_counter(counter_id,
                                   sizeof(security_cnt),
                                   (uint8_t *)&security_cnt);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    return tfm_plat_set_nv_counter(counter_id, security_cnt + 1u);
}

enum tfm_plat_err_t tfm_plat_nv_counter_permissions_check(int32_t client_id,
                                                          enum tfm_nv_counter_t nv_counter_no,
                                                          bool is_read)
{
    (void)is_read;

    switch (nv_counter_no) {
#ifdef TFM_PARTITION_PROTECTED_STORAGE
    case PLAT_NV_COUNTER_PS_0:
    case PLAT_NV_COUNTER_PS_1:
    case PLAT_NV_COUNTER_PS_2:
        if (client_id == TFM_SP_PS) {
            return TFM_PLAT_ERR_SUCCESS;
        } else {
            return TFM_PLAT_ERR_UNSUPPORTED;
        }
#endif /* TFM_PARTITION_PROTECTED_STORAGE */
    case PLAT_NV_COUNTER_NS_0:
    case PLAT_NV_COUNTER_NS_1:
    case PLAT_NV_COUNTER_NS_2:
        /* TODO how does this interact with the ns_ctx extension? */
        if (client_id < 0) {
            return TFM_PLAT_ERR_SUCCESS;
        } else {
            return TFM_PLAT_ERR_UNSUPPORTED;
        }
    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }
}

enum tfm_plat_err_t tfm_plat_ns_counter_idx_to_nv_counter(uint32_t ns_counter_idx,
                                                          enum tfm_nv_counter_t *counter_id)
{
    /* Default NV counters only have PLAT_NV_COUNTERS_NS_0, _1 and _2 */
    if ((ns_counter_idx > 2) || (counter_id == NULL)) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    *counter_id = PLAT_NV_COUNTER_NS_0 + ns_counter_idx;

    return TFM_PLAT_ERR_SUCCESS;
}
