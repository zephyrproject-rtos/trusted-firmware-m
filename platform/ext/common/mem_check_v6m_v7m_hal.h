/*
 * Copyright (c) 2019-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MEM_CHECK_V6M_V7M_HAL_H__
#define __MEM_CHECK_V6M_V7M_HAL_H__

#include <stddef.h>
#include <stdint.h>

#include "mem_check_v6m_v7m.h"

/**
 * \brief Retrieve the current active security configuration information and
 *        fills the \ref security_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref security_attr_info_t to be filled
 *
 * \return void
 */
void tfm_hal_get_mem_security_attr(const void *p, size_t s,
                                   struct security_attr_info_t *p_attr);

/**
 * \brief Retrieve the secure memory protection configuration information and
 *        fills the \ref mem_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref mem_attr_info_t to be filled
 *
 * \return void
 */
void tfm_hal_get_secure_access_attr(const void *p, size_t s,
                                    struct mem_attr_info_t *p_attr);

/**
 * \brief Retrieve the non-secure memory protection configuration information
 *        and fills the \ref mem_attr_info_t.
 *
 * \param[in]  p               Base address of target memory region
 * \param[in]  s               Size of target memory region
 * \param[out] p_attr          Address of \ref mem_attr_info_t to be filled
 *
 * \return void
 */
void tfm_hal_get_ns_access_attr(const void *p, size_t s,
                                struct mem_attr_info_t *p_attr);

#endif /* __MEM_CHECK_V6M_V7M_HAL_H__ */
