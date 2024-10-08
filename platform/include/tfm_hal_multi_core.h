/*
 * Copyright (c) 2021-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_HAL_MULTI_CORE_H__
#define __TFM_HAL_MULTI_CORE_H__

#include <stdint.h>

/**
 * \brief Performs the necessary actions to start the non-secure CPU running
 *        the code at the specified address.
 *
 * \param[in] start_addr       The entry point address of non-secure code.
 */
void tfm_hal_boot_ns_cpu(uintptr_t start_addr);

/**
 * \brief Called on the secure CPU.
 *        Flags that the secure CPU has completed its initialization
 *        Waits, if necessary, for the non-secure CPU to flag that
 *        it has completed its initialisation
 */
void tfm_hal_wait_for_ns_cpu_ready(void);

#endif /* __TFM_HAL_MULTI_CORE_H__ */
