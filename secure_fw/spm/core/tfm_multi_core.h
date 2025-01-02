/*
 * Copyright (c) 2019-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_MULTI_CORE_H__
#define __TFM_MULTI_CORE_H__

#include <stdint.h>
#include "load/interrupt_defs.h"

#define CLIENT_ID_OWNER_MAGIC           (void *)0xFFFFFFFF

/**
 * \brief Initialization of the multi core communication.
 *
 * \retval 0                    Operation succeeded.
 * \retval Other return code    Operation failed with an error code.
 */
int32_t tfm_inter_core_comm_init(void);

/**
 * \brief Register a non-secure client ID range.
 *
 * \details This function looks for a pre-defined client ID range in
 *          ns_mailbox_client_id_info[] according to \p irq_source value.
 *          If matched, it links this non-secure client ID range to the
 *          \p owner.
 *
 * \note    Each client ID range shall be registered only once.
 *
 * \param[in] owner           Identifier of the non-secure client.
 * \param[in] irq_source      The mailbox interrupt source of the target
 *                            non-secure client ID range
 *
 * \return SPM_SUCCESS if the registration is successful.
 *         SPM_ERROR_BAD_PARAMETERS if owner is null.
 *         SPM_ERROR_GENERIC otherwise.
 */
int32_t tfm_multi_core_register_client_id_range(void *owner,
                                                uint32_t irq_source);

/**
 * \brief Translate a non-secure client ID range.
 *
 * \param[in]  owner         Identifier of the non-secure client.
 * \param[in]  client_id_in  The input client ID.
 * \param[out] client_id_out The translated client ID. Undefined if
 *                           SPM_ERROR_GENERIC is returned by the function
 *
 * \return SPM_SUCCESS if the translation is successful.
 *         SPM_ERROR_BAD_PARAMETERS if a parameter is invalid.
 *         SPM_ERROR_GENERIC otherwise.
 */
int32_t tfm_multi_core_hal_client_id_translate(void *owner,
                                               int32_t client_id_in,
                                               int32_t *client_id_out);

/**
 * \brief Record a mailbox interrupt that will be completed later on.
 *
 * \param[in] p_ildi       The irq_load_info_t struct of the interrupt to record
 */
void tfm_multi_core_set_mbox_irq(const struct irq_load_info_t *p_ildi);

/**
 * \brief Clear all the mailbox interrupts that are to be processed.
 *
 */
void tfm_multi_core_clear_mbox_irq(void);

#endif /* __TFM_MULTI_CORE_H__ */
