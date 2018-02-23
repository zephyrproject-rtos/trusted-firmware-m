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

#if TFM_LVL == 1
struct spm_partition_region_t {
    uint32_t partition_id;
    uint32_t partition_state;
    uint32_t caller_partition_id;
    uint32_t orig_psp;
    uint32_t orig_psplim;
    uint32_t orig_lr;
    uint32_t share;
    uint32_t stack_ptr;
    uint32_t periph_start;
    uint32_t periph_limit;
    uint16_t periph_ppc_bank;
    uint16_t periph_ppc_loc;
    sp_init_function partition_init;
};
#else
struct spm_partition_region_t {
    uint32_t partition_id;
    uint32_t partition_state;
    uint32_t caller_partition_id;
    uint32_t orig_psp;
    uint32_t orig_psplim;
    uint32_t orig_lr;
    uint32_t share;
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
    uint32_t stack_ptr;
    uint32_t periph_start;
    uint32_t periph_limit;
    uint16_t periph_ppc_bank;
    uint16_t periph_ppc_loc;
    sp_init_function partition_init;
};
#endif

struct spm_partition_db_t {
    uint32_t is_init;
    uint32_t partition_count;
    uint32_t running_partition_id;
    struct spm_partition_region_t partitions[SPM_MAX_PARTITIONS];
};

/* Macros to pick linker symbols and allow to form the partition data base */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#if TFM_LVL == 1
#define REGION_DECLARE(a, b, c)
#else
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)
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
#define PARTITION_ADD(partition) {                                  \
     if (index >= max_partitions) {                                 \
         return max_partitions;                                     \
     }                                                              \
     db_ptr = (uint32_t *)&(db->partitions[index]);                 \
    *db_ptr++ = partition##_ID;                                     \
    *db_ptr++ = SPM_PARTITION_STATE_UNINIT; /* partition_state  */  \
    *db_ptr++ = 0U;     /* caller partition id */                   \
    *db_ptr++ = 0U;     /* original psp */                          \
    *db_ptr++ = 0U;     /* original psplim */                       \
    *db_ptr++ = 0U;     /* original lr */                           \
    *db_ptr++ = 0U;     /* share */                                 \
    *db_ptr++ = 0U;     /* stack pointer on partition enter */      \
    *db_ptr++ = 0U;     /* peripheral start */                      \
    *db_ptr++ = 0U;     /* peripheral limit */                      \
    *db_ptr++ = 0U;     /* uint16_t[2] peripheral bank/loc */       \
    *db_ptr++ = 0U;     /* partition init function*/                \
    index++;                                                        \
    }
#else
#define PARTITION_ADD(partition) {                                             \
     if (index >= max_partitions) {                                            \
         return max_partitions;                                                \
     }                                                                         \
     db_ptr = (uint32_t *)&(db->partitions[index]);                            \
    *db_ptr++ = partition##_ID;                                                \
    *db_ptr++ = SPM_PARTITION_STATE_UNINIT; /* partition_state  */             \
    *db_ptr++ = 0U;     /* caller partition id */                              \
    *db_ptr++ = 0U;     /* original psp */                                     \
    *db_ptr++ = 0U;     /* original psplim */                                  \
    *db_ptr++ = 0U;     /* original lr */                                      \
    *db_ptr++ = 0U;     /* share */                                            \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, $$Base);            \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, $$Limit);           \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, $$RO$$Base);        \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, $$RO$$Limit);       \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, _DATA$$RW$$Base);   \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, _DATA$$RW$$Limit);  \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, _DATA$$ZI$$Base);   \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, _DATA$$ZI$$Limit);  \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, _STACK$$ZI$$Base);  \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, _STACK$$ZI$$Limit); \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, partition, _STACK$$ZI$$Limit); \
    *db_ptr++ = 0U;     /* peripheral start */                                 \
    *db_ptr++ = 0U;     /* peripheral limit */                                 \
    *db_ptr++ = 0U;     /* uint16_t[2] peripheral bank/loc */                  \
    *db_ptr++ = 0U;     /* partition init function*/                           \
    index++;                                                                   \
}
#endif

#if TFM_LVL == 1
#define DUMMY_PARTITION_ADD(partition) {                            \
     if (index >= max_partitions) {                                 \
         return max_partitions;                                     \
     }                                                              \
     db_ptr = (uint32_t *)&(db->partitions[index]);                 \
    *db_ptr++ = partition##_ID;                                     \
    *db_ptr++ = SPM_PARTITION_STATE_UNINIT; /* partition_state  */  \
    *db_ptr++ = 0U;     /* caller partition id */                   \
    *db_ptr++ = 0U;     /* original psp */                          \
    *db_ptr++ = 0U;     /* original psplim */                       \
    *db_ptr++ = 0U;     /* original lr */                           \
    *db_ptr++ = 0U;     /* share */                                 \
    *db_ptr++ = 0U;     /* stack pointer on partition enter */      \
    *db_ptr++ = 0U;     /* peripheral start */                      \
    *db_ptr++ = 0U;     /* peripheral limit */                      \
    *db_ptr++ = 0U;     /* uint16_t[2] peripheral bank/loc */       \
    *db_ptr++ = 0U;     /* partition init function*/                \
    index++;                                                        \
    }
#else
#define DUMMY_PARTITION_ADD(partition) {                           \
     if (index >= max_partitions) {                                \
         return max_partitions;                                    \
     }                                                             \
     db_ptr = (uint32_t *)&(db->partitions[index]);                \
    *db_ptr++ = partition##_ID;                                    \
    *db_ptr++ = SPM_PARTITION_STATE_UNINIT; /* partition_state  */ \
    *db_ptr++ = 0U;     /* caller partition id */                  \
    *db_ptr++ = 0U;     /* original_psp */                         \
    *db_ptr++ = 0U;     /* original_psplim */                      \
    *db_ptr++ = 0U;     /* original_lr */                          \
    *db_ptr++ = 0U;     /* share */                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;                                                \
    *db_ptr++ = 0U;     /* peripheral start */                     \
    *db_ptr++ = 0U;     /* peripheral limit */                     \
    *db_ptr++ = 0U;     /* uint16_t[2] peripheral bank/loc */      \
    *db_ptr++ = 0U;     /* partition init function*/               \
    index++;                                                       \
}
#endif

#define PARTITION_ADD_PERIPHERAL(partition, start, limit, bank, loc) {         \
        db_ptr =                                                               \
               (uint32_t *)&(db->partitions[PARTITION_ID_GET(partition##_ID)]);\
        ((struct spm_partition_region_t *)db_ptr)->periph_start = start;       \
        ((struct spm_partition_region_t *)db_ptr)->periph_limit = limit;       \
        ((struct spm_partition_region_t *)db_ptr)->periph_ppc_bank = bank;     \
        ((struct spm_partition_region_t *)db_ptr)->periph_ppc_loc = loc;       \
    }

#define PARTITION_ADD_INIT_FUNC(partition, init_func) {                        \
        extern int32_t init_func(void);                                        \
        db_ptr =                                                               \
               (uint32_t *)&(db->partitions[PARTITION_ID_GET(partition##_ID)]);\
        ((struct spm_partition_region_t *)db_ptr)->partition_init = init_func; \
    }

/*This file is meant to be included twice*/
#include "user_service_defines.inc"

struct spm_partition_db_t;

uint32_t create_user_partition_db(struct spm_partition_db_t *db,
                                uint32_t max_partitions)
{
    uint32_t index = 0;
    uint32_t *db_ptr;

#include "user_service_defines.inc"

    return index;
}

#endif /* __SPM_SPM_DB_H__ */
