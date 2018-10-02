/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

#include "secure_fw/services/tfm_partition_defs.inc"

/* Definitions of the signals of the IRQs */
const struct tfm_core_irq_signal_data_t tfm_core_irq_signals[] = {
};

const size_t tfm_core_irq_signals_count = sizeof(tfm_core_irq_signals) /
                                          sizeof(*tfm_core_irq_signals);

extern void priv_irq_handler_main(uint32_t partition_id,
                                  uint32_t unpriv_handler,
                                  uint32_t irq_signal,
                                  uint32_t irq_line);

/* Forward declarations of unpriv IRQ handlers*/

/* Definitions of privileged IRQ handlers */
