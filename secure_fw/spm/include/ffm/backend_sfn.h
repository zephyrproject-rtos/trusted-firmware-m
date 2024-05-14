/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __BACKEND_SFN_H__
#define __BACKEND_SFN_H__

/* Calculate the service set. In SFN, nothing is calculated. */
#define BACKEND_SERVICE_SET(set, p_service)

#define BACKEND_SPM_INIT() tfm_spm_init()

#endif /* __BACKEND_SFN_H__ */
