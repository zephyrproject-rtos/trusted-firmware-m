/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_INTERNAL_H__
#define __TFM_INTERNAL_H__

/*
 * This function pointer is meant to only hold non secure function pointers.
 * It will be turned into a non-secure one (LSB cleared) before being called
 * whatever happens anyway (unless cast to another function pointer type).
 * Registers will be cleared before branching so that no information leaks
 * from secure to non-secure world.
 */
typedef void __attribute__((cmse_nonsecure_call)) (*nsfptr_t) (void);

extern nsfptr_t ns_entry;

/**
 * \brief Signal that secure partition initialisation is finished
 */
void tfm_secure_api_init_done(void);

/**
 * \brief Jumps to non-secure code.
 */
void jump_to_ns_code(void);

/**
 * \brief Called if veneer is running in thread mode
 */
uint32_t tfm_core_partition_request_svc_handler(
        uint32_t *svc_args, uint32_t lr);

/**
 * \brief Called when secure service returns
 */
uint32_t tfm_core_partition_return_handler(uint32_t lr);

/**
 * \brief Called by secure service to check if client is secure
 */
void tfm_core_validate_secure_caller_handler(const uint32_t svc_args[]);

/**
 * \brief Stores caller's client id in state context
 */
void tfm_core_get_caller_client_id_handler(const uint32_t svc_args[]);

/**
 * \brief Checks if a secure service's access to a memory location is permitted
 */
void tfm_core_memory_permission_check_handler(const uint32_t svc_args[]);

/**
 * \brief Handle an SPM request by a secure service
 */
void tfm_core_spm_request_handler(const struct tfm_exc_stack_t *svc_ctx);

#endif /* __TFM_INTERNAL_H__ */
