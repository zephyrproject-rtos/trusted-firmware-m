/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __IRQ_DEFS_H__
#define __IRQ_DEFS_H__

#include "psa/service.h"

struct irq_load_info_t {
    uint32_t source;                       /* IRQ source (number/index)       */
    uint32_t flih_func;                    /* FLIH function - 0 if SLIH       */
    psa_signal_t signal;                   /* The signal assigned for IRQ     */
};

#endif /* __IRQ_DEFS_H__ */
