/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include "compiler_ext_defs.h"
#include "config_spm.h"
#include "ffm/psa_api.h"
#include "spm.h"
#include "svc_num.h"
#include "tfm_psa_call_pack.h"
#include "psa/client.h"
#include "psa/lifecycle.h"
#include "psa/service.h"
#include "runtime_defs.h"

/*
 * R12 stores address of target api which will be called in
 * arch_cross_call.
 */
#define CROSS_CALL_ENTRY(psa_target_api)              \
    __asm volatile(                                   \
        SYNTAX_UNIFIED                                \
        "push   {r4, lr}                   \n"        \
        "ldr    r4, ="M2S(psa_target_api)" \n"        \
        "mov    r12, r4                    \n"        \
        "bl     arch_cross_call            \n"        \
        "pop    {r4, pc}                   \n"        \
    )

__naked
__section(".psa_interface_cross_call")
uint32_t psa_framework_version_cross(void)
{
    CROSS_CALL_ENTRY(tfm_spm_client_psa_framework_version);
}

__naked
__section(".psa_interface_cross_call")
uint32_t psa_version_cross(uint32_t sid)
{
    CROSS_CALL_ENTRY(tfm_spm_client_psa_version);
}

__naked
__section(".psa_interface_cross_call")
psa_status_t tfm_psa_call_pack_cross(psa_handle_t handle,
                                     uint32_t ctrl_param,
                                     const psa_invec *in_vec,
                                     psa_outvec *out_vec)
{
    CROSS_CALL_ENTRY(tfm_spm_client_psa_call);
}

__naked
__section(".psa_interface_cross_call")
psa_signal_t psa_wait_cross(psa_signal_t signal_mask, uint32_t timeout)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_wait);
}

__naked
__section(".psa_interface_cross_call")
psa_status_t psa_get_cross(psa_signal_t signal, psa_msg_t *msg)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_get);
}

__naked
__section(".psa_interface_cross_call")
size_t psa_read_cross(psa_handle_t msg_handle, uint32_t invec_idx,
                      void *buffer, size_t num_bytes)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_read);
}

__naked
__section(".psa_interface_cross_call")
size_t psa_skip_cross(psa_handle_t msg_handle,
                      uint32_t invec_idx, size_t num_bytes)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_skip);
}

__naked
__section(".psa_interface_cross_call")
void psa_write_cross(psa_handle_t msg_handle, uint32_t outvec_idx,
                     const void *buffer, size_t num_bytes)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_write);
}

__naked
__section(".psa_interface_cross_call")
void psa_reply_cross(psa_handle_t msg_handle, psa_status_t status)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_reply);
}

#if CONFIG_TFM_DOORBELL_API == 1
__naked
__section(".psa_interface_cross_call")
void psa_notify_cross(int32_t partition_id)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_notify);
}

__naked
__section(".psa_interface_cross_call")
void psa_clear_cross(void)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_clear);
}
#endif /* CONFIG_TFM_DOORBELL_API == 1 */

__naked
__section(".psa_interface_cross_call")
void psa_panic_cross(void)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_panic);
}

__naked
__section(".psa_interface_cross_call")
uint32_t psa_rot_lifecycle_state_cross(void)
{
    CROSS_CALL_ENTRY(tfm_spm_get_lifecycle_state);
}

/* Following PSA APIs are only needed by connection-based services */
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1

__naked
__section(".psa_interface_cross_call")
psa_handle_t psa_connect_cross(uint32_t sid, uint32_t version)
{
    CROSS_CALL_ENTRY(tfm_spm_client_psa_connect);
}

__naked
__section(".psa_interface_cross_call")
void psa_close_cross(psa_handle_t handle)
{
    CROSS_CALL_ENTRY(tfm_spm_client_psa_close);
}

__naked
__section(".psa_interface_cross_call")
void psa_set_rhandle_cross(psa_handle_t msg_handle, void *rhandle)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_set_rhandle);
}

#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API */

#if CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1
__naked
__section(".psa_interface_cross_call")
void psa_irq_enable_cross(psa_signal_t irq_signal)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_irq_enable);
}

__naked
__section(".psa_interface_cross_call")
psa_irq_status_t psa_irq_disable_cross(psa_signal_t irq_signal)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_irq_disable);
}

/* This API is only used for FLIH. */
#if CONFIG_TFM_FLIH_API == 1
__naked
__section(".psa_interface_cross_call")
void psa_reset_signal_cross(psa_signal_t irq_signal)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_reset_signal);
}
#endif /* CONFIG_TFM_FLIH_API == 1 */

/* This API is only used for SLIH. */
#if CONFIG_TFM_SLIH_API == 1
__naked
__section(".psa_interface_cross_call")
void psa_eoi_cross(psa_signal_t irq_signal)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_eoi);
}
#endif /* CONFIG_TFM_SLIH_API */
#endif /* CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1 */

#if PSA_FRAMEWORK_HAS_MM_IOVEC

__naked
__section(".psa_interface_cross_call")
const void *psa_map_invec_cross(psa_handle_t msg_handle, uint32_t invec_idx)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_map_invec);
}

__naked
__section(".psa_interface_cross_call")
void psa_unmap_invec_cross(psa_handle_t msg_handle, uint32_t invec_idx)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_unmap_invec);
}

__naked
__section(".psa_interface_cross_call")
void *psa_map_outvec_cross(psa_handle_t msg_handle, uint32_t outvec_idx)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_map_outvec);
}

__naked
__section(".psa_interface_cross_call")
void psa_unmap_outvec_cross(psa_handle_t msg_handle, uint32_t outvec_idx,
                            size_t len)
{
    CROSS_CALL_ENTRY(tfm_spm_partition_psa_unmap_outvec);
}

#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC */

const struct psa_api_tbl_t psa_api_cross = {
                                tfm_psa_call_pack_cross,
                                psa_version_cross,
                                psa_framework_version_cross,
                                psa_wait_cross,
                                psa_get_cross,
                                psa_read_cross,
                                psa_skip_cross,
                                psa_write_cross,
                                psa_reply_cross,
                                psa_panic_cross,
                                psa_rot_lifecycle_state_cross,
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1
                                psa_connect_cross,
                                psa_close_cross,
                                psa_set_rhandle_cross,
#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API */
#if CONFIG_TFM_DOORBELL_API == 1
                                psa_notify_cross,
                                psa_clear_cross,
#endif /* CONFIG_TFM_DOORBELL_API == 1 */
#if CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1
                                psa_irq_enable_cross,
                                psa_irq_disable_cross,
#if CONFIG_TFM_FLIH_API == 1
                                psa_reset_signal_cross,
#endif /* CONFIG_TFM_FLIH_API == 1 */
#if CONFIG_TFM_SLIH_API == 1
                                psa_eoi_cross,
#endif /* CONFIG_TFM_SLIH_API == 1 */
#endif /* CONFIG_TFM_FLIH_API == 1 || CONFIG_TFM_SLIH_API == 1 */
#if PSA_FRAMEWORK_HAS_MM_IOVEC
                                psa_map_invec_cross,
                                psa_unmap_invec_cross,
                                psa_map_outvec_cross,
                                psa_unmap_outvec_cross,
#endif /* PSA_FRAMEWORK_HAS_MM_IOVEC */
                            };
