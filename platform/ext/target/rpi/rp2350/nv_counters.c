/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#include "tfm_plat_nv_counters.h"
#include "tfm_plat_otp.h"

#if (PS_NS_NV_COUNTER_IN_ITS == 1)
#include "psa/internal_trusted_storage.h"
#endif

#include <limits.h>
#include <string.h>

#define OTP_COUNTER_MAX_SIZE    64
#define NV_COUNTER_SIZE         4
#define OTP_COUNTER_MAGIC       0xAEC7
#define ITS_NV_COUNTER_UID_BASE 0xFFFFFFFFFFFFA000

enum tfm_plat_err_t tfm_plat_init_nv_counter(void)
{
    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t read_otp_counter(enum tfm_otp_element_id_t id,
                                            uint8_t *val)
{
    size_t counter_size;
    enum tfm_plat_err_t err;
    size_t idx;
    uint16_t counter_value[OTP_COUNTER_MAX_SIZE / sizeof(uint16_t)] = {0};
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
    for (idx = 0; idx < counter_size / sizeof(uint16_t); idx++) {
        if (counter_value[idx] == OTP_COUNTER_MAGIC) {
            count += 1;
        } else if (counter_value[idx] == 0) {
            break;
        } else {
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
    }

    memcpy(val, &count, NV_COUNTER_SIZE);

    return TFM_PLAT_ERR_SUCCESS;
}

#if (PS_NS_NV_COUNTER_IN_ITS == 1)
static enum tfm_plat_err_t read_its_nv_counter(enum tfm_otp_element_id_t id,
                                            uint8_t *val)
{
    psa_storage_uid_t uid = (ITS_NV_COUNTER_UID_BASE + id);
    psa_status_t status;
    size_t data_length = 0;

    status = psa_its_get(uid, 0, NV_COUNTER_SIZE, val, &data_length);

    if (status == PSA_ERROR_DOES_NOT_EXIST) {
        memset(val, 0, NV_COUNTER_SIZE);
    }

    if (((status == PSA_SUCCESS) && (data_length == NV_COUNTER_SIZE)) ||
        (status == PSA_ERROR_DOES_NOT_EXIST)) {
        return TFM_PLAT_ERR_SUCCESS;
    } else {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
}
#endif /* (PS_NS_NV_COUNTER_IN_ITS == 1) */

enum tfm_plat_err_t tfm_plat_read_nv_counter(enum tfm_nv_counter_t counter_id,
                                             uint32_t size, uint8_t *val)
{
    if (size != NV_COUNTER_SIZE) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    /* Assumes Platform nv counters are contiguous*/
    if (counter_id >= PLAT_NV_COUNTER_BL2_0 &&
        counter_id < (PLAT_NV_COUNTER_BL2_0 + MCUBOOT_IMAGE_NUMBER)) {
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_BL2_0 +
                                       (counter_id - PLAT_NV_COUNTER_BL2_0),
                                   val);
    }

    switch (counter_id) {
#if (PS_NS_NV_COUNTER_IN_ITS == 0)
    case (PLAT_NV_COUNTER_NS_0):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_0, val);
    case (PLAT_NV_COUNTER_NS_1):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_1, val);
    case (PLAT_NV_COUNTER_NS_2):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_2, val);
    case (PLAT_NV_COUNTER_PS_0):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_0, val);
    case (PLAT_NV_COUNTER_PS_1):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_1, val);
    case (PLAT_NV_COUNTER_PS_2):
        return read_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_2, val);
#else
    case (PLAT_NV_COUNTER_NS_0):
        return read_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_NS_0, val);
    case (PLAT_NV_COUNTER_NS_1):
        return read_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_NS_1, val);
    case (PLAT_NV_COUNTER_NS_2):
        return read_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_NS_2, val);
    case (PLAT_NV_COUNTER_PS_0):
        return read_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_PS_0, val);
    case (PLAT_NV_COUNTER_PS_1):
        return read_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_PS_1, val);
    case (PLAT_NV_COUNTER_PS_2):
        return read_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_PS_2, val);
#endif /* (PS_NS_NV_COUNTER_IN_ITS == 0) */

    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
    }
}

static enum tfm_plat_err_t set_otp_counter(enum tfm_otp_element_id_t id,
                                           uint32_t val)
{
    size_t counter_size;
    enum tfm_plat_err_t err;
    size_t idx;
    uint16_t counter_value[OTP_COUNTER_MAX_SIZE / sizeof(uint16_t)] = {0};

    err = tfm_plat_otp_get_size(id, &counter_size);
    if (err != TFM_PLAT_ERR_SUCCESS) {
        return err;
    }

    counter_size = counter_size > OTP_COUNTER_MAX_SIZE ? OTP_COUNTER_MAX_SIZE
                                                       : counter_size;

    if (val > (counter_size / sizeof(uint16_t))) {
        return TFM_PLAT_ERR_INVALID_INPUT;
    }

    for (idx = 0; idx < val; idx++) {
        counter_value[idx] = OTP_COUNTER_MAGIC;
    }

    err = tfm_plat_otp_write(id, counter_size,
                             (uint8_t *)counter_value);

    return err;
}

#if (PS_NS_NV_COUNTER_IN_ITS == 1)
static enum tfm_plat_err_t set_its_nv_counter(enum tfm_otp_element_id_t id,
                                           uint32_t val)
{
    psa_storage_uid_t uid = (ITS_NV_COUNTER_UID_BASE + id);
    psa_status_t status;

    status = psa_its_set(uid, NV_COUNTER_SIZE, &val, PSA_STORAGE_FLAG_NONE);

    if (status == PSA_SUCCESS) {
        return TFM_PLAT_ERR_SUCCESS;
    } else {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
}
#endif /* (PS_NS_NV_COUNTER_IN_ITS == 1) */

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
#if (PS_NS_NV_COUNTER_IN_ITS == 0)
    case (PLAT_NV_COUNTER_NS_0):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_0, value);
    case (PLAT_NV_COUNTER_NS_1):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_1, value);
    case (PLAT_NV_COUNTER_NS_2):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_NS_2, value);
    case (PLAT_NV_COUNTER_PS_0):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_0, value);
    case (PLAT_NV_COUNTER_PS_1):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_1, value);
    case (PLAT_NV_COUNTER_PS_2):
        return set_otp_counter(PLAT_OTP_ID_NV_COUNTER_PS_2, value);
#else
    case (PLAT_NV_COUNTER_NS_0):
        return set_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_NS_0, value);
    case (PLAT_NV_COUNTER_NS_1):
        return set_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_NS_1, value);
    case (PLAT_NV_COUNTER_NS_2):
        return set_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_NS_2, value);
    case (PLAT_NV_COUNTER_PS_0):
        return set_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_PS_0, value);
    case (PLAT_NV_COUNTER_PS_1):
        return set_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_PS_1, value);
    case (PLAT_NV_COUNTER_PS_2):
        return set_its_nv_counter(PLAT_OTP_ID_NV_COUNTER_PS_2, value);
#endif /* (PS_NS_NV_COUNTER_IN_ITS == 0) */

    default:
        return TFM_PLAT_ERR_UNSUPPORTED;
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
