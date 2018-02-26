/*
 * Copyright (c) 2017-2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SPM_SPM_DB_H__
#define __SPM_SPM_DB_H__

#include <stdint.h>

/* This limit is only used to define the size of the database reserved for
 * partitions. There's no requirement that it match the number of partitions
 * that get registered in a specific build
 */
#define SPM_MAX_PARTITIONS (6)

#define PARTITION_ID_GET(id)      (id - TFM_SP_BASE)

typedef int32_t(*sp_init_function)(void);

struct spm_partition_static_data_t {
    uint32_t partition_id;
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
    uint32_t running_partition_id;
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

#define PARTITION_DECLARE(partition)                       \
    REGION_DECLARE(Image$$, partition, $$Base);            \
    REGION_DECLARE(Image$$, partition, $$Limit);           \
    REGION_DECLARE(Image$$, partition, $$RO$$Base);        \
    REGION_DECLARE(Image$$, partition, $$RO$$Limit);       \
    REGION_DECLARE(Image$$, partition, _DATA$$RW$$Base);   \
    REGION_DECLARE(Image$$, partition, _DATA$$RW$$Limit);  \
    REGION_DECLARE(Image$$, partition, _DATA$$ZI$$Base);   \
    REGION_DECLARE(Image$$, partition, _DATA$$ZI$$Limit);  \
    REGION_DECLARE(Image$$, partition, _STACK$$ZI$$Base);  \
    REGION_DECLARE(Image$$, partition, _STACK$$ZI$$Limit); \


#if TFM_LVL == 1
#define PARTITION_INIT_STATIC_DATA(data, partition) \
    data.partition_id    = partition##_ID;          \
    data.periph_start    = 0U;                      \
    data.periph_limit    = 0U;                      \
    data.periph_ppc_bank = 0U;                      \
    data.periph_ppc_loc  = 0U;                      \
    data.partition_init  = 0U
#else
#define PARTITION_INIT_STATIC_DATA(data, partition)                        \
    data.partition_id    = partition##_ID;                                 \
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
    data.periph_start    = 0U;                                             \
    data.periph_limit    = 0U;                                             \
    data.periph_ppc_bank = 0U;                                             \
    data.periph_ppc_loc  = 0U;                                             \
    data.partition_init  = 0U
#endif

#if TFM_LVL == 1
#define DUMMY_PARTITION_INIT_STATIC_DATA(data, partition)                     \
        /* In case of TFM_LVL1 the static data is initialised the same way */ \
        PARTITION_INIT_STATIC_DATA(data, partition)
#else
#define DUMMY_PARTITION_INIT_STATIC_DATA(data, partition) \
    data.partition_id    = partition##_ID;                \
    data.code_start      = 0U;                            \
    data.code_limit      = 0U;                            \
    data.ro_start        = 0U;                            \
    data.ro_limit        = 0U;                            \
    data.rw_start        = 0U;                            \
    data.rw_limit        = 0U;                            \
    data.zi_start        = 0U;                            \
    data.zi_limit        = 0U;                            \
    data.stack_bottom    = 0U;                            \
    data.stack_top       = 0U;                            \
    data.periph_start    = 0U;                            \
    data.periph_limit    = 0U;                            \
    data.periph_ppc_bank = 0U;                            \
    data.periph_ppc_loc  = 0U;                            \
    data.partition_init  = 0U
#endif


#if TFM_LVL == 1
#define PARTITION_INIT_RUNTIME_DATA(data, partition)                           \
    data.partition_state     = SPM_PARTITION_STATE_UNINIT;                     \
    data.caller_partition_id = 0U;                                             \
    data.orig_psp            = 0U;                                             \
    data.orig_psplim         = 0U;                                             \
    data.orig_lr             = 0U;                                             \
    data.share               = 0U
#else
#define PARTITION_INIT_RUNTIME_DATA(data, partition)                           \
    data.partition_state     = SPM_PARTITION_STATE_UNINIT;                     \
    data.caller_partition_id = 0U;                                             \
    data.orig_psp            = 0U;                                             \
    data.orig_psplim         = 0U;                                             \
    data.orig_lr             = 0U;                                             \
    data.share               = 0U;                                             \
    data.stack_ptr           = PART_REGION_ADDR(partition, _STACK$$ZI$$Limit)
#endif

#if TFM_LVL == 1
#define DUMMY_PARTITION_INIT_RUNTIME_DATA(data, partition) \
    data.partition_state     = SPM_PARTITION_STATE_UNINIT; \
    data.caller_partition_id = 0U;                         \
    data.orig_psp            = 0U;                         \
    data.orig_psplim         = 0U;                         \
    data.orig_lr             = 0U;                         \
    data.share               = 0U
#else
#define DUMMY_PARTITION_INIT_RUNTIME_DATA(data, partition) \
    data.partition_state     = SPM_PARTITION_STATE_UNINIT; \
    data.caller_partition_id = 0U;                         \
    data.orig_psp            = 0U;                         \
    data.orig_psplim         = 0U;                         \
    data.orig_lr             = 0U;                         \
    data.share               = 0U;                         \
    data.stack_ptr           = 0U
#endif

#define PARTITION_ADD(partition) {                                      \
    if (index >= max_partitions) {                                      \
        return max_partitions;                                          \
    }                                                                   \
    part_ptr = (struct spm_partition_desc_t *)&(db->partitions[index]); \
    PARTITION_INIT_STATIC_DATA(part_ptr->static_data, partition);       \
    PARTITION_INIT_RUNTIME_DATA(part_ptr->runtime_data, partition);     \
    index++;                                                            \
    }

#define DUMMY_PARTITION_ADD(partition) {                                  \
    if (index >= max_partitions) {                                        \
        return max_partitions;                                            \
    }                                                                     \
    part_ptr = (struct spm_partition_desc_t *)&(db->partitions[index]);   \
    DUMMY_PARTITION_INIT_STATIC_DATA(part_ptr->static_data, partition);   \
    /* The runtime data is the same for the dummy partitions*/            \
    DUMMY_PARTITION_INIT_RUNTIME_DATA(part_ptr->runtime_data, partition); \
    index++;                                                              \
    }

#define PARTITION_ADD_PERIPHERAL(partition, start, limit, bank, loc) {  \
        part_ptr = &(db->partitions[PARTITION_ID_GET(partition##_ID)]); \
        ((struct spm_partition_desc_t *)part_ptr)->                     \
               static_data.periph_start = start;                        \
        ((struct spm_partition_desc_t *)part_ptr)->                     \
               static_data.periph_limit = limit;                        \
        ((struct spm_partition_desc_t *)part_ptr)->                     \
               static_data.periph_ppc_bank = bank;                      \
        ((struct spm_partition_desc_t *)part_ptr)->                     \
               static_data.periph_ppc_loc = loc;                        \
    }

#define PARTITION_ADD_INIT_FUNC(partition, init_func) {                 \
        extern int32_t init_func(void);                                 \
        part_ptr = &(db->partitions[PARTITION_ID_GET(partition##_ID)]); \
        ((struct spm_partition_desc_t *)part_ptr)->                     \
               static_data.partition_init = init_func;                  \
    }

/*This file is meant to be included twice*/
#include "user_service_defines.inc"

struct spm_partition_db_t;

uint32_t create_user_partition_db(struct spm_partition_db_t *db,
                                uint32_t max_partitions)
{
    uint32_t index = 0;
    struct spm_partition_desc_t *part_ptr;

#include "user_service_defines.inc"

    return index;
}

#endif /* __SPM_SPM_DB_H__ */
