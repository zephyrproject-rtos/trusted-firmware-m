/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "secure_utilities.h"
#include "psa/error.h"
#include "tfm_utils.h"
#include "tfm_internal.h"

#define DEFAULT_NS_CLIENT_ID ((int32_t)-1)

typedef uint32_t TZ_ModuleId_t;
typedef uint32_t TZ_MemoryId_t;

void tfm_nspm_configure_clients(void)
{
    return;
}

int32_t tfm_nspm_get_current_client_id(void)
{
    return DEFAULT_NS_CLIENT_ID;
}

/* TF-M implementation of the CMSIS TZ RTOS thread context management API */

/// Initialize secure context memory system
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__attribute__((cmse_nonsecure_entry))
uint32_t TZ_InitContextSystem_S(void)
{
    return 1U;
}

/// Allocate context memory for calling secure software modules in TrustZone
/// \param[in]  module   identifies software modules called from non-secure mode
/// \return value != 0 id TrustZone memory slot identifier
/// \return value 0    no memory available or internal error
/* This veneer is TF-M internal, not a secure service */
__attribute__((cmse_nonsecure_entry, noinline))
TZ_MemoryId_t TZ_AllocModuleContext_S (TZ_ModuleId_t module)
{
    /* add attribute 'noinline' to avoid a build error. */
    (void)module;
    return 1U;
}

/// Free context memory that was previously allocated with \ref TZ_AllocModuleContext_S
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__attribute__((cmse_nonsecure_entry, noinline))
uint32_t TZ_FreeModuleContext_S (TZ_MemoryId_t id)
{
    (void)id;
    return 1U;
}

/// Load secure context (called on RTOS thread context switch)
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__attribute__((cmse_nonsecure_entry, noinline))
uint32_t TZ_LoadContext_S (TZ_MemoryId_t id)
{
    (void)id;
    return 1U;
}

/// Store secure context (called on RTOS thread context switch)
/// \param[in]  id  TrustZone memory slot identifier
/// \return execution status (1: success, 0: error)
/* This veneer is TF-M internal, not a secure service */
__attribute__((cmse_nonsecure_entry, noinline))
uint32_t TZ_StoreContext_S (TZ_MemoryId_t id)
{
    (void)id;
    return 1U;
}

__attribute__((section("SFN")))
psa_status_t tfm_nspm_thread_entry(void)
{
#ifdef TFM_CORE_DEBUG
    /* Jumps to non-secure code */
    LOG_MSG("Jumping to non-secure code...");
#endif

    jump_to_ns_code();

    /* Should not run here */
    TFM_ASSERT(false);
    return PSA_SUCCESS;
}
