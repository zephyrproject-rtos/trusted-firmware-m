/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SPM_SERVICES_H__
#define __TFM_SPM_SERVICES_H__

#include <stdint.h>

typedef uint32_t psa_signal_t;

void tfm_enable_irq(psa_signal_t irq_signal);
void tfm_disable_irq(psa_signal_t irq_signal);

#endif /* __TFM_SPM_SERVICES_H__ */
