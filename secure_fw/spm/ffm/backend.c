/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "config_impl.h"

#if CONFIG_TFM_SPM_BACKEND_SFN == 1
#include "backend_sfn.c"
#elif CONFIG_TFM_SPM_BACKEND_IPC == 1
#include "backend_ipc.c"
#else
#error "Invalid configuration."
#endif
