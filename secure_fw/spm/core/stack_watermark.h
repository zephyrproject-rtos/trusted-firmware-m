/*
 * Copyright (c) 2022-2024, Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __STACK_WATERMARK_H__
#define __STACK_WATERMARK_H__

#include "spm.h"

#ifdef CONFIG_TFM_STACK_WATERMARKS
#ifndef CONFIG_TFM_USE_TRUSTZONE
void watermark_spm_stack(void);
#endif
void watermark_stack(const struct partition_t *p_pt);
void dump_used_stacks(void);
#else
#define watermark_spm_stack()
#define watermark_stack(p_pt)
#define dump_used_stacks()
#endif

#endif /* __STACK_WATERMARK_H__ */
