/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SPM_DB_H__
#define __SPM_DB_H__

#include <stdint.h>
#include "platform_retarget.h"
#include "target_cfg.h"
#include "spm_partition_defs.h"

/* This limit is only used to define the size of the database reserved for
 * partitions. There's no requirement that it match the number of partitions
 * that get registered in a specific build
 */
#define SPM_MAX_PARTITIONS (7)

struct spm_partition_desc_t;
struct spm_partition_db_t;

uint32_t get_partition_idx(uint32_t partition_id);

typedef int32_t(*sp_init_function)(void);

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
    uint32_t periph_start;
    uint32_t periph_limit;
    uint16_t periph_ppc_bank;
    uint16_t periph_ppc_loc;
    sp_init_function partition_init;
};

struct spm_partition_desc_t {
    struct spm_partition_static_data_t static_data;
    struct spm_partition_runtime_data_t runtime_data;
};

struct spm_partition_db_t {
    uint32_t is_init;
    uint32_t partition_count;
    uint32_t running_partition_idx;
    struct spm_partition_desc_t partitions[SPM_MAX_PARTITIONS];
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


#if TFM_LVL == 1
#define PARTITION_INIT_STATIC_DATA(data, partition, flags) \
    do {                                                   \
        data.partition_id    = partition##_ID;             \
        data.partition_flags = flags;                      \
    } while (0)
#else
#define PARTITION_INIT_STATIC_DATA(data, partition, flags)                     \
    do {                                                                       \
        data.partition_id    = partition##_ID;                                 \
        data.partition_flags = flags;                                          \
        data.code_start      = PART_REGION_ADDR(partition, $$Base);            \
        data.code_limit      = PART_REGION_ADDR(partition, $$Limit);           \
        data.ro_start        = PART_REGION_ADDR(partition, $$RO$$Base);        \
        data.ro_limit        = PART_REGION_ADDR(partition, $$RO$$Limit);       \
        data.rw_start        = PART_REGION_ADDR(partition, _DATA$$RW$$Base);   \
        data.rw_limit        = PART_REGION_ADDR(partition, _DATA$$RW$$Limit);  \
        data.zi_start        = PART_REGION_ADDR(partition, _DATA$$ZI$$Base);   \
        data.zi_limit        = PART_REGION_ADDR(partition, _DATA$$ZI$$Limit);  \
        data.stack_bottom    = PART_REGION_ADDR(partition, _STACK$$ZI$$Base);  \
        data.stack_top       = PART_REGION_ADDR(partition, _STACK$$ZI$$Limit); \
    } while (0)
#endif

#if TFM_LVL == 1
#define PARTITION_INIT_RUNTIME_DATA(data, partition)            \
    do {                                                        \
        data.partition_state      = SPM_PARTITION_STATE_UNINIT; \
    } while (0)
#else
#define PARTITION_INIT_RUNTIME_DATA(data, partition)                \
    do {                                                            \
        data.partition_state      = SPM_PARTITION_STATE_UNINIT;     \
        data.stack_ptr            =                                 \
                PART_REGION_ADDR(partition, _STACK$$ZI$$Limit);     \
    } while (0)
#endif

#define PARTITION_DECLARE(partition, flags)                                  \
    do {                                                                     \
        REGION_DECLARE(Image$$, partition, $$Base);                          \
        REGION_DECLARE(Image$$, partition, $$Limit);                         \
        REGION_DECLARE(Image$$, partition, $$RO$$Base);                      \
        REGION_DECLARE(Image$$, partition, $$RO$$Limit);                     \
        REGION_DECLARE(Image$$, partition, _DATA$$RW$$Base);                 \
        REGION_DECLARE(Image$$, partition, _DATA$$RW$$Limit);                \
        REGION_DECLARE(Image$$, partition, _DATA$$ZI$$Base);                 \
        REGION_DECLARE(Image$$, partition, _DATA$$ZI$$Limit);                \
        REGION_DECLARE(Image$$, partition, _STACK$$ZI$$Base);                \
        REGION_DECLARE(Image$$, partition, _STACK$$ZI$$Limit);               \
        struct spm_partition_desc_t *part_ptr;                               \
        if (g_spm_partition_db.partition_count >= SPM_MAX_PARTITIONS) {      \
            return SPM_ERR_INVALID_CONFIG;                                   \
        }                                                                    \
        part_ptr = &(g_spm_partition_db.partitions[                          \
            g_spm_partition_db.partition_count]);                            \
        PARTITION_INIT_STATIC_DATA(part_ptr->static_data, partition, flags); \
        PARTITION_INIT_RUNTIME_DATA(part_ptr->runtime_data, partition);      \
        ++g_spm_partition_db.partition_count;                                \
    } while (0)

#define PARTITION_ADD_INIT_FUNC(partition, init_func)                 \
    do {                                                              \
        extern int32_t init_func(void);                               \
        uint32_t partition_idx = get_partition_idx(partition##_ID);   \
        struct spm_partition_desc_t *part_ptr =                       \
            &(g_spm_partition_db.partitions[partition_idx]);          \
        part_ptr->static_data.partition_init = init_func;             \
    } while (0)

#define PARTITION_ADD_PERIPHERAL(partition, start, limit, bank, loc)   \
    do {                                                               \
        uint32_t partition_idx = get_partition_idx(partition##_ID);    \
        struct spm_partition_desc_t *part_ptr =                        \
            &(g_spm_partition_db.partitions[partition_idx]);           \
        part_ptr->static_data.periph_start = start;                    \
        part_ptr->static_data.periph_limit = limit;                    \
        part_ptr->static_data.periph_ppc_bank = bank;                  \
        part_ptr->static_data.periph_ppc_loc = loc;                    \
    } while (0)

#endif /* __SPM_DB_H__ */
