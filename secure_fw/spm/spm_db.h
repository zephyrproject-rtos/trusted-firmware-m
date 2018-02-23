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
 * services. There's no requirement that it match the number of services
 * that get registered in a specific build
 */
#define SPM_MAX_SERVICES (6)

#define SERVICE_ID_GET(id)      (id - TFM_SEC_FUNC_BASE)

typedef int32_t(*ss_init_function)(void);

#if TFM_LVL == 1
struct spm_service_region_t {
    uint32_t service_id;
    uint32_t service_state;
    uint32_t caller_service_id;
    uint32_t orig_psp;
    uint32_t orig_psplim;
    uint32_t orig_lr;
    uint32_t share;
    uint32_t stack_ptr;
    uint32_t periph_start;
    uint32_t periph_limit;
    uint16_t periph_ppc_bank;
    uint16_t periph_ppc_loc;
    ss_init_function service_init;
};
#else
struct spm_service_region_t {
    uint32_t service_id;
    uint32_t service_state;
    uint32_t caller_service_id;
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
    ss_init_function service_init;
};
#endif

struct spm_service_db_t {
    uint32_t is_init;
    uint32_t services_count;
    uint32_t running_service_id;
    struct spm_service_region_t services[SPM_MAX_SERVICES];
};

/* Macros to pick linker symbols and allow to form the service data base */
#define REGION(a, b, c) a##b##c
#define REGION_NAME(a, b, c) REGION(a, b, c)
#if TFM_LVL == 1
#define REGION_DECLARE(a, b, c)
#else
#define REGION_DECLARE(a, b, c) extern uint32_t REGION_NAME(a, b, c)
#endif

#define SERVICE_DECLARE(service)                         \
    REGION_DECLARE(Image$$, service, $$Base);            \
    REGION_DECLARE(Image$$, service, $$Limit);           \
    REGION_DECLARE(Image$$, service, $$RO$$Base);        \
    REGION_DECLARE(Image$$, service, $$RO$$Limit);       \
    REGION_DECLARE(Image$$, service, _DATA$$RW$$Base);   \
    REGION_DECLARE(Image$$, service, _DATA$$RW$$Limit);  \
    REGION_DECLARE(Image$$, service, _DATA$$ZI$$Base);   \
    REGION_DECLARE(Image$$, service, _DATA$$ZI$$Limit);  \
    REGION_DECLARE(Image$$, service, _STACK$$ZI$$Base);  \
    REGION_DECLARE(Image$$, service, _STACK$$ZI$$Limit); \


#if TFM_LVL == 1
#define SERVICE_ADD(service) {                                \
     if (index >= max_services) {                             \
         return max_services;                                 \
     }                                                        \
     db_ptr = (uint32_t *)&(db->services[index]);             \
    *db_ptr++ = service##_ID;                                 \
    *db_ptr++ = SPM_PART_STATE_UNINIT; /* service_state  */   \
    *db_ptr++ = 0U;     /* caller service id */               \
    *db_ptr++ = 0U;     /* original psp */                    \
    *db_ptr++ = 0U;     /* original psplim */                 \
    *db_ptr++ = 0U;     /* original lr */                     \
    *db_ptr++ = 0U;     /* share */                           \
    *db_ptr++ = 0U;     /* stack pointer on service enter */  \
    *db_ptr++ = 0U;     /* peripheral start */                \
    *db_ptr++ = 0U;     /* peripheral limit */                \
    *db_ptr++ = 0U;     /* uint16_t[2] peripheral bank/loc */ \
    *db_ptr++ = 0U;     /* service init function*/            \
    index++;                                                  \
    }
#else
#define SERVICE_ADD(service) {                                               \
     if (index >= max_services) {                                            \
         return max_services;                                                \
     }                                                                       \
     db_ptr = (uint32_t *)&(db->services[index]);                            \
    *db_ptr++ = service##_ID;                                                \
    *db_ptr++ = SPM_PART_STATE_UNINIT; /* service_state  */                  \
    *db_ptr++ = 0U;     /* caller service id */                              \
    *db_ptr++ = 0U;     /* original psp */                                   \
    *db_ptr++ = 0U;     /* original psplim */                                \
    *db_ptr++ = 0U;     /* original lr */                                    \
    *db_ptr++ = 0U;     /* share */                                          \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, $$Base);            \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, $$Limit);           \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, $$RO$$Base);        \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, $$RO$$Limit);       \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, _DATA$$RW$$Base);   \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, _DATA$$RW$$Limit);  \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, _DATA$$ZI$$Base);   \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, _DATA$$ZI$$Limit);  \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, _STACK$$ZI$$Base);  \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, _STACK$$ZI$$Limit); \
    *db_ptr++ = (uint32_t)&REGION_NAME(Image$$, service, _STACK$$ZI$$Limit); \
    *db_ptr++ = 0U;     /* peripheral start */                               \
    *db_ptr++ = 0U;     /* peripheral limit */                               \
    *db_ptr++ = 0U;     /* uint16_t[2] peripheral bank/loc */                \
    *db_ptr++ = 0U;     /* service init function*/                           \
    index++;                                                                 \
}
#endif

#if TFM_LVL == 1
#define DUMMY_SERVICE_ADD(service) {                          \
     if (index >= max_services) {                             \
         return max_services;                                 \
     }                                                        \
     db_ptr = (uint32_t *)&(db->services[index]);             \
    *db_ptr++ = service##_ID;                                 \
    *db_ptr++ = SPM_PART_STATE_UNINIT; /* service_state  */   \
    *db_ptr++ = 0U;     /* caller service id */               \
    *db_ptr++ = 0U;     /* original psp */                    \
    *db_ptr++ = 0U;     /* original psplim */                 \
    *db_ptr++ = 0U;     /* original lr */                     \
    *db_ptr++ = 0U;     /* share */                           \
    *db_ptr++ = 0U;     /* stack pointer on service enter */  \
    *db_ptr++ = 0U;     /* peripheral start */                \
    *db_ptr++ = 0U;     /* peripheral limit */                \
    *db_ptr++ = 0U;     /* uint16_t[2] peripheral bank/loc */ \
    *db_ptr++ = 0U;     /* service init function*/            \
    index++;                                                  \
    }
#else
#define DUMMY_SERVICE_ADD(service) {                          \
     if (index >= max_services) {                             \
         return max_services;                                 \
     }                                                        \
     db_ptr = (uint32_t *)&(db->services[index]);             \
    *db_ptr++ = service##_ID;                                 \
    *db_ptr++ = SPM_PART_STATE_UNINIT; /* service_state  */   \
    *db_ptr++ = 0U;     /* caller service id */               \
    *db_ptr++ = 0U;     /* original_psp */                    \
    *db_ptr++ = 0U;     /* original_psplim */                 \
    *db_ptr++ = 0U;     /* original_lr */                     \
    *db_ptr++ = 0U;     /* share */                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;                                           \
    *db_ptr++ = 0U;     /* peripheral start */                \
    *db_ptr++ = 0U;     /* peripheral limit */                \
    *db_ptr++ = 0U;     /* uint16_t[2] peripheral bank/loc */ \
    *db_ptr++ = 0U;     /* service init function*/            \
    index++;                                                  \
}
#endif

#define SERVICE_ADD_PERIPHERAL(service, start, limit, bank, loc) {          \
        db_ptr = (uint32_t *)&(db->services[SERVICE_ID_GET(service##_ID)]); \
        ((struct spm_service_region_t *)db_ptr)->periph_start = start;      \
        ((struct spm_service_region_t *)db_ptr)->periph_limit = limit;      \
        ((struct spm_service_region_t *)db_ptr)->periph_ppc_bank = bank;    \
        ((struct spm_service_region_t *)db_ptr)->periph_ppc_loc = loc;      \
    }

#define SERVICE_ADD_INIT_FUNC(service, init_func) {                         \
        extern int32_t init_func(void);                                     \
        db_ptr = (uint32_t *)&(db->services[SERVICE_ID_GET(service##_ID)]); \
        ((struct spm_service_region_t *)db_ptr)->service_init = init_func;  \
    }

/*This file is meant to be included twice*/
#include "user_service_defines.inc"

struct spm_service_db_t;

uint32_t create_user_service_db(struct spm_service_db_t *db,
                                uint32_t max_services)
{
    uint32_t index = 0;
    uint32_t *db_ptr;

#include "user_service_defines.inc"

    return index;
}

#endif /* __SPM_SPM_DB_H__ */
