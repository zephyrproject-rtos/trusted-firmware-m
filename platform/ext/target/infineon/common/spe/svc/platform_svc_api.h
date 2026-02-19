/*
 * Copyright (c) 2023-2026 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef PLATFORM_SVC_API_H
#define PLATFORM_SVC_API_H

#include <stdint.h>
#include "psa/client.h"

/**
 * Structure to hold original input and output vectors and their counts
 */
typedef struct ifx_original_iovec_t {
    psa_invec invec[PSA_MAX_IOVEC];
    psa_outvec outvec[PSA_MAX_IOVEC];
    size_t invec_count;
    size_t outvec_count;
} ifx_original_iovec_t;

/**
 * \brief Enable/disable SysTick for FLIH/SLIH tf-m-tests
 *
 * \param[in]   enable      0 - disable SysTick IRQ,
 *                          != 0 - enable IRQ
 *
 * \retval                  Platform error code, see \ref tfm_platform_err_t
 */
int32_t ifx_call_platform_enable_systick(uint32_t enable);

/**
 * \brief Write string to SPM UART log
 *
 * \param[in]   str         String to output
 * \param[in]   len         String size
 * \param[in]   core_id     Core prefix id, see \ref ifx_stdio_core_id_t
 *
 * \retval                  Platform error code, see \ref tfm_platform_err_t
 */
int32_t ifx_call_platform_uart_log(const char *str, uint32_t len, uint32_t core_id);

/**
 * \brief Resets the system.
 *
 * \details Requests a system reset to reset the MCU.
 */
void ifx_call_platform_system_reset(void);

/**
 * \brief Retrieve the original IO vectors
 */
psa_status_t ifx_call_platform_original_iovec(psa_handle_t msg_handle,
                                              ifx_original_iovec_t *io_vec);

#endif /* PLATFORM_SVC_API_H */
