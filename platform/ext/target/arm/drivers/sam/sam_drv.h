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
 * \file sam_drv.h
 * \brief Driver for Arm Security Alarm Manager (SAM).
 */

#ifndef __SAM_DRV_H__
#define __SAM_DRV_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief SAM Event IDs.
 */
enum sam_event_id_t {
    SAM_EVENT_CONFIG_INTEGRITY_ERROR = 0UL,
    SAM_EVENT_WATCHDOG_TIMER,
    SAM_EVENT_DUPLICATION_ERROR,
    SAM_EVENT_LCM_FATAL_ERROR,
    SAM_EVENT_CPU_LOCKUP,
    SAM_EVENT_ATU_ERROR,
    SAM_EVENT_KMU_PARITY_ERROR,
    SAM_EVENT_CRYPTO_PARITY_ERROR,
    SAM_EVENT_SIC_PARITY_ERROR,
    SAM_EVENT_AES_DFA_ERROR,
    SAM_EVENT_AES_PARITY_ERROR,
    SAM_EVENT_DMA_DCLS_ERROR,
    SAM_EVENT_PSI_PARITY_ERROR,
    SAM_EVENT_BUS_PARITY_ERROR,
    SAM_EVENT_PROCESSOR_DCLS_ERROR,
    SAM_EVENT_PROCESSOR_RAS_SET_0,
    SAM_EVENT_PROCESSOR_RAS_SET_1,
    SAM_EVENT_PROCESSOR_RAS_SET_2,
    SAM_EVENT_SRAM_PARTIAL_WRITE,
    SAM_EVENT_VM0_SINGLE_ECC_ERROR,
    SAM_EVENT_VM1_SINGLE_ECC_ERROR,
    SAM_EVENT_VM2_SINGLE_ECC_ERROR,
    SAM_EVENT_VM3_SINGLE_ECC_ERROR,
    SAM_EVENT_VM0_DOUBLE_ECC_ERROR,
    SAM_EVENT_VM1_DOUBLE_ECC_ERROR,
    SAM_EVENT_VM2_DOUBLE_ECC_ERROR,
    SAM_EVENT_VM3_DOUBLE_ECC_ERROR,
    SAM_EVENT_SRAM_MPC_PARITY_ERROR,
    SAM_EVENT_SIC_MPC_PARITY_ERROR,
    SAM_EVENT_ATU_PARITY_ERROR,
    SAM_EVENT_PPR_PARITY_ERROR,
    SAM_EVENT_SYSCTRL_PARITY_ERROR,
    SAM_EVENT_CPU_PPB_PARITY_ERROR,
    SAM_EVENT_SACFG_PARITY_ERROR,
    SAM_EVENT_NSACFG_PARITY_ERROR,
    SAM_EVENT_INTEGRITY_CHECKER_ALARM,
    SAM_EVENT_TRAM_PARITY_ERROR,
    /* Reserved 37-47 */
    SAM_EVENT_EXTERNAL_SENSOR_0 = 48UL,
    SAM_EVENT_EXTERNAL_SENSOR_1,
    SAM_EVENT_EXTERNAL_SENSOR_2,
    SAM_EVENT_EXTERNAL_SENSOR_3,
    SAM_EVENT_EXTERNAL_SENSOR_4,
    SAM_EVENT_EXTERNAL_SENSOR_5,
    SAM_EVENT_EXTERNAL_SENSOR_6,
    SAM_EVENT_EXTERNAL_SENSOR_7,
    SAM_EVENT_EXTERNAL_SENSOR_8,
    SAM_EVENT_EXTERNAL_SENSOR_9,
    SAM_EVENT_EXTERNAL_SENSOR_10,
    SAM_EVENT_EXTERNAL_SENSOR_11,
    SAM_EVENT_EXTERNAL_SENSOR_12,
    SAM_EVENT_EXTERNAL_SENSOR_13,
    SAM_EVENT_EXTERNAL_SENSOR_14,
    SAM_EVENT_EXTERNAL_SENSOR_15,

    /* Maximum permitted event ID */
    SAM_MAX_EVENT_ID = SAM_EVENT_EXTERNAL_SENSOR_15,
};

/**
 * \brief SAM response action IDs.
 */
enum sam_response_t {
    SAM_RESPONSE_NONE = 0UL,
    SAM_RESPONSE_COLD_RESET = 1UL,
    SAM_RESPONSE_WARM_RESET = 1UL << 1,
    SAM_RESPONSE_NMI = 1UL << 2,
    SAM_RESPONSE_CRITICAL_FAULT_INTERRUPT = 1UL << 3,
    SAM_RESPONSE_FAULT_INTERRUPT = 1UL << 4,
    SAM_RESPONSE_ACTION_5 = 1UL << 5,
    SAM_RESPONSE_ACTION_6 = 1UL << 6,
    SAM_RESPONSE_ACTION_7 = 1UL << 7,

    SAM_MAX_RESPONSE_ACTION = SAM_RESPONSE_ACTION_7,
};

/**
 * \brief SAM error type.
 */
enum sam_error_t {
    SAM_ERROR_NONE = 0,
    SAM_ERROR_INVALID_ARGUMENT,
};

/**
 * \brief SAM event handler function type.
 */
typedef void (*sam_event_handler_t)(void);

/* SAM config covers 24 registers: samem to samicv */
#define SAM_CONFIG_LEN 24

/**
 * \brief SAM device configuration structure.
 */
struct sam_dev_cfg_t {
    const uintptr_t base; /**< SAM base address */
    const uint32_t default_config[SAM_CONFIG_LEN];  /**< Default SAM config */
};

/**
 * \brief SAM device structure.
 */
struct sam_dev_t {
    const struct sam_dev_cfg_t *const cfg; /**< SAM configuration */
    sam_event_handler_t event_handlers[SAM_MAX_EVENT_ID + 1]; /**< SAM event handlers */
};

/**
 * \brief Initialize SAM device.
 *
 * \param[in] dev  Pointer to SAM device struct.
 *
 * \return Error code of enum sam_error_t type.
 */
enum sam_error_t sam_init(const struct sam_dev_t *dev);

/**
 * \brief Enable a SAM event.
 *
 * \param[in] dev       Pointer to SAM device struct.
 * \param[in] event_id  Event ID to enable.
 *
 * \return Error code of enum sam_error_t type.
 */
enum sam_error_t sam_enable_event(const struct sam_dev_t *dev,
                                  enum sam_event_id_t event_id);

/**
 * \brief Disable a SAM event.
 *
 * \param[in] dev       Pointer to SAM device struct.
 * \param[in] event_id  Event ID to disable.
 *
 * \return Error code of enum sam_error_t type.
 */
enum sam_error_t sam_disable_event(const struct sam_dev_t *dev,
                                   enum sam_event_id_t event_id);

/**
 * \brief Set the response action for a SAM event.
 *
 * \param[in] dev              Pointer to SAM device struct.
 * \param[in] event_id         Event ID for which to set response.
 * \param[in] response         Response action to set.
 * \param[in] enable_response  Enable or disable the response.
 *
 * \return Error code of enum sam_error_t type.
 */
enum sam_error_t sam_set_event_response(const struct sam_dev_t *dev,
                                        enum sam_event_id_t event_id,
                                        enum sam_response_t response,
                                        bool enable_response);

/**
 * \brief Set the SAM watchdog counter initial value.
 *
 * \param[in] dev          Pointer to SAM device struct.
 * \param[in] count_value  Number of cycles to count after a SAM event before
 *                         asserting the watchdog event:
 *                         0: watchdog count down disabled,
 *                         1 to (2^26)-1: valid initial count values.
 * \param[in] responses    Bitwise OR of response actions that trigger the
 *                         watchdog counter. SAM_RESPONSE_COLD_RESET and
 *                         SAM_RESPONSE_WARM_RESET are ignored.
 */
void sam_set_watchdog_counter_initial_value(const struct sam_dev_t *dev,
                                            uint32_t count_value,
                                            enum sam_response_t responses);

/**
 * \brief Register a handler function for a SAM event.
 *
 * \param[in] dev            Pointer to SAM device struct.
 * \param[in] event_id       Event ID to handle.
 * \param[in] event_handler  Pointer to event handler function.
 *
 * \return Error code of enum sam_error_t type.
 */
enum sam_error_t sam_register_event_handler(struct sam_dev_t *dev,
                                            enum sam_event_id_t event_id,
                                            sam_event_handler_t event_handler);

/**
 * \brief Handle any outstanding SAM events by calling the corresponding
 *        registered event handler functions.
 *
 * \note This function is intended to be called from an interrupt handler.
 *
 * \param[in] dev  Pointer to SAM device struct.
 */
void sam_handle_event(const struct sam_dev_t *dev);

/**
 * \brief Handle an SRAM partial write event.
 *
 * \note This function is intended to be called from an interrupt handler.
 *
 * \param[in] dev  Pointer to SAM device struct.
 */
void sam_handle_partial_write(const struct sam_dev_t *dev);

/**
 * \brief Handle an SRAM single ECC error event.
 *
 * \note This function is intended to be called from an interrupt handler.
 *
 * \param[in] dev  Pointer to SAM device struct.
 */
void sam_handle_single_ecc_error(const struct sam_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __SAM_DRV_H__ */
