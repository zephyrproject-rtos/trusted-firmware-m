/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SPM_DB_H__
#define __SPM_DB_H__

typedef int32_t(*sp_init_function)(void);

/**
 * Holds the fields of the partition DB used by the SPM code. The values of
 * these fields are calculated at compile time, and set during initialisation
 * phase.
 */
struct spm_partition_static_data_t {
    uint32_t partition_id;
    uint32_t partition_flags;
#if TFM_LVL != 1
    uint32_t code_start;
    uint32_t code_limit;
    uint32_t ro_start;
    uint32_t ro_limit;
    uint32_t rw_start;
    uint32_t rw_limit;
    uint32_t zi_start;
    uint32_t zi_limit;
    uint32_t stack_bottom;
    uint32_t stack_top;
#endif
    sp_init_function partition_init;
};

/**
 * Holds the fields that define a partition for SPM. The fields are further
 * divided to structures, to keep the related fields close to each other.
 */
struct tfm_spm_partition_desc_t {
    struct spm_partition_static_data_t static_data;
    struct spm_partition_runtime_data_t runtime_data;
    struct tfm_spm_partition_platform_data_t platform_data;
};

#endif /* __SPM_DB_H__ */
