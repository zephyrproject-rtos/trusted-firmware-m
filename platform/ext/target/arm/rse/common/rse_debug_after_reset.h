/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __RSE_DEBUG_AFTER_RESET_H__
#define __RSE_DEBUG_AFTER_RESET_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief                               Copies the permissions mask to the shared
 *                                      area and trigger reset.
 *
 * \param[in] permissions_mask          Pointer to the input mask to be copied.
 * \param[in] mask_len                  Size of mask.
 *
 * \return                              0 on success, non-zero on failure.
 */
int rse_debug_after_reset(const uint8_t *permissions_mask,
                          size_t mask_len);

#ifdef __cplusplus
}
#endif

#endif /* __RSE_DEBUG_AFTER_RESET_H__ */
