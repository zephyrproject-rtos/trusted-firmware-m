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
    sp_init_function partition_init;
};

/**
 * Holds the fields that define a partition for SPM. The fields are further
 * divided to structures, to keep the related fields close to each other.
 */
struct spm_partition_desc_t {
    struct spm_partition_static_data_t static_data;
    struct spm_partition_runtime_data_t runtime_data;
    struct tfm_spm_partition_platform_data_t *platform_data;
#if TFM_LVL != 1
    struct tfm_spm_partition_memory_data_t memory_data;
#endif
};

/* Macros to pick linker symbols and allow to form the partition data base */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#if TFM_LVL == 1
#define REGION_DECLARE(a, b, c)
#else
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)
#define PART_REGION_ADDR(partition, region) \
    (uint32_t)&REGION_NAME(Image$$, partition, region)
#endif

#endif /* __SPM_DB_H__ */
