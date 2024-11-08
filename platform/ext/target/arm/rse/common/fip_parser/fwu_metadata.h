/*
 * SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/**
 * \file    fwu_metadata.h
 * \brief   PSA FWU metadata from DEN0118
 */

#ifndef __FWU_METADATA_H__
#define __FWU_METADATA_H__

#include <stdint.h>

/**
 * \brief                             Parse FWU metadata.
 *
 * \param[in]  md_offset              The host flash offset of the metadata to
 *                                    be parsed.
 * \return                            0 on success, non-zero on failure.
 */
int parse_fwu_metadata(uint64_t md_offset);

#endif /* __FWU_METADATA_H__ */
