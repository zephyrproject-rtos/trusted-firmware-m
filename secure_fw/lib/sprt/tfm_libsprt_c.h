/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_LIBSPRT_C_H__
#define __TFM_LIBSPRT_C_H__

#include <stddef.h>

/**
 * \brief   This function moves 'n' bytes from 'src' to 'dest'.
 *
 * \param[out]  dest        Destination address
 * \param[in]   src         Source address
 * \param[in]   n           Number of bytes to be moved
 *
 * \retval      dest        Destination address
 * \note                    Memory overlap has been taken into consideration
 *                          and processed properly in the function.
 */
void *tfm_sprt_c_memmove(void *dest, const void *src, size_t n);

/**
 * \brief   This function copies 'n' bytes from 'src' to 'dest'.
 *
 * \param[out]  dest        Destination address
 * \param[in]   src         Source address
 * \param[in]   n           Number of bytes to be copied
 *
 * \retval      dest        Destination address
 * \note                    It has the same effect as tfm_sprt_c_memmove().
 */
void *tfm_sprt_c_memcpy(void *dest, const void *src, size_t n);

#endif /* __TFM_LIBSPRT_C_H__ */
