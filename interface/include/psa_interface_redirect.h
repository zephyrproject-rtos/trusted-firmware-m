/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __PSA_INTERFACE_REDIRECT_H__
#define __PSA_INTERFACE_REDIRECT_H__

#if defined(CONFIG_TFM_PSA_API_SUPERVISOR_CALL)

#define psa_framework_version    psa_framework_version_svc
#define psa_version              psa_version_svc
#define psa_connect              psa_connect_svc
#define tfm_psa_call_pack        tfm_psa_call_pack_svc
#define psa_close                psa_close_svc
#define psa_wait                 psa_wait_svc
#define psa_get                  psa_get_svc
#define psa_set_rhandle          psa_set_rhandle_svc
#define psa_read                 psa_read_svc
#define psa_skip                 psa_skip_svc
#define psa_write                psa_write_svc
#define psa_reply                psa_reply_svc
#define psa_notify               psa_notify_svc
#define psa_clear                psa_clear_svc
#define psa_eoi                  psa_eoi_svc
#define psa_panic                psa_panic_svc
#define psa_irq_enable           psa_irq_enable_svc
#define psa_irq_disable          psa_irq_disable_svc
#define psa_reset_signal         psa_reset_signal_svc
#define psa_rot_lifecycle_state  psa_rot_lifecycle_state_svc

#elif defined(CONFIG_TFM_PSA_API_THREAD_CALL)

#define psa_framework_version    psa_framework_version_thread
#define psa_version              psa_version_thread
#define psa_connect              psa_connect_thread
#define tfm_psa_call_pack        tfm_psa_call_pack_thread
#define psa_close                psa_close_thread
#define psa_wait                 psa_wait_thread
#define psa_get                  psa_get_thread
#define psa_set_rhandle          psa_set_rhandle_thread
#define psa_read                 psa_read_thread
#define psa_skip                 psa_skip_thread
#define psa_write                psa_write_thread
#define psa_reply                psa_reply_thread
#define psa_notify               psa_notify_thread
#define psa_clear                psa_clear_thread
#define psa_eoi                  psa_eoi_thread
#define psa_panic                psa_panic_thread
#define psa_irq_enable           psa_irq_enable_thread
#define psa_irq_disable          psa_irq_disable_thread
#define psa_reset_signal         psa_reset_signal_thread
#define psa_rot_lifecycle_state  psa_rot_lifecycle_state_thread

#endif

#endif /* __PSA_INTERFACE_REDIRECT_H__ */
