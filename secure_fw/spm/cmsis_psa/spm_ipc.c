/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include <stdbool.h>
#include "psa/client.h"
#include "psa/service.h"
#include "tfm_thread.h"
#include "tfm_wait.h"
#include "tfm_internal_defines.h"
#include "tfm_spm_hal.h"
#include "tfm_irq_list.h"
#include "tfm_api.h"
#include "tfm_secure_api.h"
#include "tfm_memory_utils.h"
#include "tfm_hal_defs.h"
#include "tfm_hal_isolation.h"
#include "spm_ipc.h"
#include "tfm_peripherals_def.h"
#include "tfm_core_utils.h"
#include "tfm_rpc.h"
#include "tfm_core_trustzone.h"
#include "tfm_list.h"
#include "tfm_hal_isolation.h"
#include "tfm_pools.h"
#include "region.h"
#include "region_defs.h"
#include "spm_partition_defs.h"
#include "psa_manifest/pid.h"
#include "tfm/tfm_spm_services.h"

#include "secure_fw/partitions/tfm_service_list.inc"
#include "tfm_spm_db_ipc.inc"

/* Extern service variable */
extern struct tfm_spm_service_t service[];
extern const struct tfm_spm_service_db_t service_db[];

/* Pools */
TFM_POOL_DECLARE(conn_handle_pool, sizeof(struct tfm_conn_handle_t),
                 TFM_CONN_HANDLE_MAX_NUM);

void tfm_irq_handler(uint32_t partition_id, psa_signal_t signal,
                     IRQn_Type irq_line);

#include "tfm_secure_irq_handlers_ipc.inc"

/*********************** Connection handle conversion APIs *******************/

/* Set a minimal value here for feature expansion. */
#define CLIENT_HANDLE_VALUE_MIN        32

#define CONVERSION_FACTOR_BITOFFSET    3
#define CONVERSION_FACTOR_VALUE        (1 << CONVERSION_FACTOR_BITOFFSET)
/* Set 32 as the maximum */
#define CONVERSION_FACTOR_VALUE_MAX    0x20

#if CONVERSION_FACTOR_VALUE > CONVERSION_FACTOR_VALUE_MAX
#error "CONVERSION FACTOR OUT OF RANGE"
#endif

static uint32_t loop_index;

/*
 * A handle instance psa_handle_t allocated inside SPM is actually a memory
 * address among the handle pool. Return this handle to the client directly
 * exposes information of secure memory address. In this case, converting the
 * handle into another value does not represent the memory address to avoid
 * exposing secure memory directly to clients.
 *
 * This function converts the handle instance into another value by scaling the
 * handle in pool offset, the converted value is named as a user handle.
 *
 * The formula:
 *  user_handle = (handle_instance - POOL_START) * CONVERSION_FACTOR_VALUE +
 *                CLIENT_HANDLE_VALUE_MIN + loop_index
 * where:
 *  CONVERSION_FACTOR_VALUE = 1 << CONVERSION_FACTOR_BITOFFSET, and should not
 *  exceed CONVERSION_FACTOR_VALUE_MAX.
 *
 *  handle_instance in RANGE[POOL_START, POOL_END]
 *  user_handle     in RANGE[CLIENT_HANDLE_VALUE_MIN, 0x3FFFFFFF]
 *  loop_index      in RANGE[0, CONVERSION_FACTOR_VALUE - 1]
 *
 *  note:
 *  loop_index is used to promise same handle instance is converted into
 *  different user handles in short time.
 */
psa_handle_t tfm_spm_to_user_handle(struct tfm_conn_handle_t *handle_instance)
{
    psa_handle_t user_handle;

    loop_index = (loop_index + 1) % CONVERSION_FACTOR_VALUE;
    user_handle = (psa_handle_t)((((uintptr_t)handle_instance -
                  (uintptr_t)conn_handle_pool) << CONVERSION_FACTOR_BITOFFSET) +
                  CLIENT_HANDLE_VALUE_MIN + loop_index);

    return user_handle;
}

/*
 * This function converts a user handle into a corresponded handle instance.
 * The converted value is validated before returning, an invalid handle instance
 * is returned as NULL.
 *
 * The formula:
 *  handle_instance = ((user_handle - CLIENT_HANDLE_VALUE_MIN) /
 *                    CONVERSION_FACTOR_VALUE) + POOL_START
 * where:
 *  CONVERSION_FACTOR_VALUE = 1 << CONVERSION_FACTOR_BITOFFSET, and should not
 *  exceed CONVERSION_FACTOR_VALUE_MAX.
 *
 *  handle_instance in RANGE[POOL_START, POOL_END]
 *  user_handle     in RANGE[CLIENT_HANDLE_VALUE_MIN, 0x3FFFFFFF]
 *  loop_index      in RANGE[0, CONVERSION_FACTOR_VALUE - 1]
 */
struct tfm_conn_handle_t *tfm_spm_to_handle_instance(psa_handle_t user_handle)
{
    struct tfm_conn_handle_t *handle_instance;

    if (user_handle == PSA_NULL_HANDLE) {
        return NULL;
    }

    handle_instance = (struct tfm_conn_handle_t *)((((uintptr_t)user_handle -
                      CLIENT_HANDLE_VALUE_MIN) >> CONVERSION_FACTOR_BITOFFSET) +
                      (uintptr_t)conn_handle_pool);

    return handle_instance;
}

/* Service handle management functions */
struct tfm_conn_handle_t *tfm_spm_create_conn_handle(
                                        struct tfm_spm_service_t *service,
                                        int32_t client_id)
{
    struct tfm_conn_handle_t *p_handle;

    TFM_CORE_ASSERT(service);

    /* Get buffer for handle list structure from handle pool */
    p_handle = (struct tfm_conn_handle_t *)tfm_pool_alloc(conn_handle_pool);
    if (!p_handle) {
        return NULL;
    }

    p_handle->service = service;
    p_handle->status = TFM_HANDLE_STATUS_IDLE;
    p_handle->client_id = client_id;

    /* Add handle node to list for next psa functions */
    tfm_list_add_tail(&service->handle_list, &p_handle->list);

    return p_handle;
}

int32_t tfm_spm_validate_conn_handle(
                                    const struct tfm_conn_handle_t *conn_handle,
                                    int32_t client_id)
{
    /* Check the handle address is validated */
    if (is_valid_chunk_data_in_pool(conn_handle_pool,
                                    (uint8_t *)conn_handle) != true) {
        return IPC_ERROR_GENERIC;
    }

    /* Check the handle caller is correct */
    if (conn_handle->client_id != client_id) {
        return IPC_ERROR_GENERIC;
    }

    return IPC_SUCCESS;
}

int32_t tfm_spm_free_conn_handle(struct tfm_spm_service_t *service,
                                 struct tfm_conn_handle_t *conn_handle)
{
    TFM_CORE_ASSERT(service);
    TFM_CORE_ASSERT(conn_handle != NULL);

    /* Clear magic as the handler is not used anymore */
    conn_handle->internal_msg.magic = 0;

    /* Remove node from handle list */
    tfm_list_del_node(&conn_handle->list);

    /* Back handle buffer to pool */
    tfm_pool_free(conn_handle);
    return IPC_SUCCESS;
}

int32_t tfm_spm_set_rhandle(struct tfm_spm_service_t *service,
                            struct tfm_conn_handle_t *conn_handle,
                            void *rhandle)
{
    TFM_CORE_ASSERT(service);
    /* Set reverse handle value only be allowed for a connected handle */
    TFM_CORE_ASSERT(conn_handle != NULL);

    conn_handle->rhandle = rhandle;
    return IPC_SUCCESS;
}

/**
 * \brief                   Get reverse handle value from connection hanlde.
 *
 * \param[in] service       Target service context pointer
 * \param[in] conn_handle   Connection handle created by
 *                          tfm_spm_create_conn_handle()
 *
 * \retval void *           Success
 * \retval "Does not return"  Panic for those:
 *                              service pointer are NULL
 *                              hanlde is \ref PSA_NULL_HANDLE
 *                              handle node does not be found
 */
static void *tfm_spm_get_rhandle(struct tfm_spm_service_t *service,
                                 struct tfm_conn_handle_t *conn_handle)
{
    TFM_CORE_ASSERT(service);
    /* Get reverse handle value only be allowed for a connected handle */
    TFM_CORE_ASSERT(conn_handle != NULL);

    return conn_handle->rhandle;
}

/* Partition management functions */

struct tfm_msg_body_t *tfm_spm_get_msg_by_signal(struct partition_t *partition,
                                                 psa_signal_t signal)
{
    struct tfm_list_node_t *node, *head;
    struct tfm_msg_body_t *tmp_msg, *msg = NULL;

    TFM_CORE_ASSERT(partition);

    head = &partition->msg_list;

    if (tfm_list_is_empty(head)) {
        return NULL;
    }

    /*
     * There may be multiple messages for this RoT Service signal, do not clear
     * partition mask until no remaining message. Search may be optimized.
     */
    TFM_LIST_FOR_EACH(node, head) {
        tmp_msg = TFM_GET_CONTAINER_PTR(node, struct tfm_msg_body_t, msg_node);
        if (tmp_msg->service->service_db->signal == signal && msg) {
            return msg;
        } else if (tmp_msg->service->service_db->signal == signal) {
            msg = tmp_msg;
            tfm_list_del_node(node);
        }
    }

    partition->signals_asserted &= ~signal;

    return msg;
}

/**
 * \brief Returns the index of the partition with the given partition ID.
 *
 * \param[in] partition_id     Partition id
 *
 * \return the partition idx if partition_id is valid,
 *         \ref SPM_INVALID_PARTITION_IDX othervise
 */
static uint32_t get_partition_idx(uint32_t partition_id)
{
    uint32_t i;

    if (partition_id == INVALID_PARTITION_ID) {
        return SPM_INVALID_PARTITION_IDX;
    }

    for (i = 0; i < g_spm_partition_db.partition_count; ++i) {
        if (g_spm_partition_db.partitions[i].static_data->partition_id ==
            partition_id) {
            return i;
        }
    }
    return SPM_INVALID_PARTITION_IDX;
}

/**
 * \brief Get the flags associated with a partition
 *
 * \param[in] partition_idx     Partition index
 *
 * \return Flags associated with the partition
 *
 * \note This function doesn't check if partition_idx is valid.
 */
static uint32_t tfm_spm_partition_get_flags(uint32_t partition_idx)
{
    return g_spm_partition_db.partitions[partition_idx].static_data->
           partition_flags;
}

#if TFM_LVL != 1
/**
 * \brief Change the privilege mode for partition thread mode.
 *
 * \param[in] privileged        Privileged mode,
 *                                \ref TFM_PARTITION_PRIVILEGED_MODE
 *                                and \ref TFM_PARTITION_UNPRIVILEGED_MODE
 *
 * \note Barrier instructions are not called by this function, and if
 *       it is called in thread mode, it might be necessary to call
 *       them after this function returns.
 */
static void tfm_spm_partition_change_privilege(uint32_t privileged)
{
    CONTROL_Type ctrl;

    ctrl.w = __get_CONTROL();

    if (privileged == TFM_PARTITION_PRIVILEGED_MODE) {
        ctrl.b.nPRIV = 0;
    } else {
        ctrl.b.nPRIV = 1;
    }

    __set_CONTROL(ctrl.w);
}
#endif /* if(TFM_LVL != 1) */

uint32_t tfm_spm_partition_get_privileged_mode(uint32_t partition_flags)
{
    if (partition_flags & SPM_PART_FLAG_PSA_ROT) {
        return TFM_PARTITION_PRIVILEGED_MODE;
    } else {
        return TFM_PARTITION_UNPRIVILEGED_MODE;
    }
}

bool tfm_is_partition_privileged(uint32_t partition_idx)
{
    uint32_t flags = tfm_spm_partition_get_flags(partition_idx);

    return tfm_spm_partition_get_privileged_mode(flags) ==
           TFM_PARTITION_PRIVILEGED_MODE;
}

struct tfm_spm_service_t *tfm_spm_get_service_by_sid(uint32_t sid)
{
    uint32_t i, num;

    num = sizeof(service) / sizeof(struct tfm_spm_service_t);
    for (i = 0; i < num; i++) {
        if (service[i].service_db->sid == sid) {
            return &service[i];
        }
    }

    return NULL;
}

/**
 * \brief                   Get the partition context by partition ID.
 *
 * \param[in] partition_id  Partition identity
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Target partition context pointer,
 *                          \ref partition_t structures
 */
static struct partition_t *tfm_spm_get_partition_by_id(int32_t partition_id)
{
    uint32_t idx = get_partition_idx(partition_id);

    if (idx != SPM_INVALID_PARTITION_IDX) {
        return &(g_spm_partition_db.partitions[idx]);
    }
    return NULL;
}

struct partition_t *tfm_spm_get_running_partition(void)
{
    struct tfm_core_thread_t *pth = tfm_core_thrd_get_curr_thread();
    struct partition_t *partition;

    partition = TFM_GET_CONTAINER_PTR(pth, struct partition_t, sp_thread);

    return partition;
}

int32_t tfm_spm_check_client_version(struct tfm_spm_service_t *service,
                                     uint32_t version)
{
    TFM_CORE_ASSERT(service);

    switch (service->service_db->version_policy) {
    case TFM_VERSION_POLICY_RELAXED:
        if (version > service->service_db->version) {
            return IPC_ERROR_VERSION;
        }
        break;
    case TFM_VERSION_POLICY_STRICT:
        if (version != service->service_db->version) {
            return IPC_ERROR_VERSION;
        }
        break;
    default:
        return IPC_ERROR_VERSION;
    }
    return IPC_SUCCESS;
}

int32_t tfm_spm_check_authorization(uint32_t sid,
                                    struct tfm_spm_service_t *service,
                                    bool ns_caller)
{
    struct partition_t *partition = NULL;
    int32_t i;

    TFM_CORE_ASSERT(service);

    if (ns_caller) {
        if (!service->service_db->non_secure_client) {
            return IPC_ERROR_GENERIC;
        }
    } else {
        partition = tfm_spm_get_running_partition();
        if (!partition) {
            tfm_core_panic();
        }

        for (i = 0; i < partition->static_data->dependencies_num; i++) {
            if (partition->static_data->p_dependencies[i] == sid) {
                break;
            }
        }

        if (i == partition->static_data->dependencies_num) {
            return IPC_ERROR_GENERIC;
        }
    }
    return IPC_SUCCESS;
}

/* Message functions */

struct tfm_msg_body_t *tfm_spm_get_msg_from_handle(psa_handle_t msg_handle)
{
    /*
     * The message handler passed by the caller is considered invalid in the
     * following cases:
     *   1. Not a valid message handle. (The address of a message is not the
     *      address of a possible handle from the pool
     *   2. Handle not belongs to the caller partition (The handle is either
     *      unused, or owned by anither partition)
     * Check the conditions above
     */
    struct tfm_msg_body_t *p_msg;
    uint32_t partition_id;
    struct tfm_conn_handle_t *p_conn_handle =
                                     tfm_spm_to_handle_instance(msg_handle);

    if (is_valid_chunk_data_in_pool(
        conn_handle_pool, (uint8_t *)p_conn_handle) != 1) {
        return NULL;
    }

    p_msg = &p_conn_handle->internal_msg;

    /*
     * Check that the magic number is correct. This proves that the message
     * structure contains an active message.
     */
    if (p_msg->magic != TFM_MSG_MAGIC) {
        return NULL;
    }

    /* Check that the running partition owns the message */
    partition_id = tfm_spm_partition_get_running_partition_id();
    if (partition_id != p_msg->service->partition->static_data->partition_id) {
        return NULL;
    }

    return p_msg;
}

struct tfm_msg_body_t *
 tfm_spm_get_msg_buffer_from_conn_handle(struct tfm_conn_handle_t *conn_handle)
{
    TFM_CORE_ASSERT(conn_handle != NULL);

    return &(conn_handle->internal_msg);
}

void tfm_spm_fill_msg(struct tfm_msg_body_t *msg,
                      struct tfm_spm_service_t *service,
                      psa_handle_t handle,
                      int32_t type, int32_t client_id,
                      psa_invec *invec, size_t in_len,
                      psa_outvec *outvec, size_t out_len,
                      psa_outvec *caller_outvec)
{
    uint32_t i;
    struct tfm_conn_handle_t *conn_handle;

    TFM_CORE_ASSERT(msg);
    TFM_CORE_ASSERT(service);
    TFM_CORE_ASSERT(!(invec == NULL && in_len != 0));
    TFM_CORE_ASSERT(!(outvec == NULL && out_len != 0));
    TFM_CORE_ASSERT(in_len <= PSA_MAX_IOVEC);
    TFM_CORE_ASSERT(out_len <= PSA_MAX_IOVEC);
    TFM_CORE_ASSERT(in_len + out_len <= PSA_MAX_IOVEC);

    /* Clear message buffer before using it */
    spm_memset(msg, 0, sizeof(struct tfm_msg_body_t));

    tfm_event_init(&msg->ack_evnt);
    msg->magic = TFM_MSG_MAGIC;
    msg->service = service;
    msg->caller_outvec = caller_outvec;
    msg->msg.client_id = client_id;

    /* Copy contents */
    msg->msg.type = type;

    for (i = 0; i < in_len; i++) {
        msg->msg.in_size[i] = invec[i].len;
        msg->invec[i].base = invec[i].base;
    }

    for (i = 0; i < out_len; i++) {
        msg->msg.out_size[i] = outvec[i].len;
        msg->outvec[i].base = outvec[i].base;
        /* Out len is used to record the writed number, set 0 here again */
        msg->outvec[i].len = 0;
    }

    /* Use the user connect handle as the message handle */
    msg->msg.handle = handle;

    conn_handle = tfm_spm_to_handle_instance(handle);
    /* For connected handle, set rhandle to every message */
    if (conn_handle) {
        msg->msg.rhandle = tfm_spm_get_rhandle(service, conn_handle);
    }

    /* Set the private data of NSPE client caller in multi-core topology */
    if (TFM_CLIENT_ID_IS_NS(client_id)) {
        tfm_rpc_set_caller_data(msg, client_id);
    }
}

int32_t tfm_spm_send_event(struct tfm_spm_service_t *service,
                           struct tfm_msg_body_t *msg)
{
    struct partition_t *partition = service->partition;

    TFM_CORE_ASSERT(service);
    TFM_CORE_ASSERT(msg);

    /* Add message to partition message list tail */
    tfm_list_add_tail(&partition->msg_list, &msg->msg_node);

    /* Messages put. Update signals */
    partition->signals_asserted |= service->service_db->signal;

    tfm_event_wake(&partition->event,
                   (partition->signals_asserted & partition->signals_waiting));

    /*
     * If it is a NS request via RPC, it is unnecessary to block current
     * thread.
     */
    if (!is_tfm_rpc_msg(msg)) {
        tfm_event_wait(&msg->ack_evnt);
    }

    return IPC_SUCCESS;
}

uint32_t tfm_spm_partition_get_running_partition_id(void)
{
    struct partition_t *partition;

    partition = tfm_spm_get_running_partition();
    if (partition && partition->static_data) {
        return partition->static_data->partition_id;
    } else {
        return INVALID_PARTITION_ID;
    }
}

int32_t tfm_memory_check(const void *buffer, size_t len, bool ns_caller,
                         enum tfm_memory_access_e access,
                         uint32_t privileged)
{
    enum tfm_hal_status_t err;
    uint32_t attr = 0;

    /* If len is zero, this indicates an empty buffer and base is ignored */
    if (len == 0) {
        return IPC_SUCCESS;
    }

    if (!buffer) {
        return IPC_ERROR_BAD_PARAMETERS;
    }

    if ((uintptr_t)buffer > (UINTPTR_MAX - len)) {
        return IPC_ERROR_MEMORY_CHECK;
    }

    if (access == TFM_MEMORY_ACCESS_RW) {
        attr |= (TFM_HAL_ACCESS_READABLE | TFM_HAL_ACCESS_WRITABLE);
    } else {
        attr |= TFM_HAL_ACCESS_READABLE;
    }

    if (privileged == TFM_PARTITION_UNPRIVILEGED_MODE) {
        attr |= TFM_HAL_ACCESS_UNPRIVILEGED;
    } else {
        attr &= ~TFM_HAL_ACCESS_UNPRIVILEGED;
    }

    if (ns_caller) {
        attr |= TFM_HAL_ACCESS_NS;
    }

    err = tfm_hal_memory_has_access((uintptr_t)buffer, len, attr);

    if (err == TFM_HAL_SUCCESS) {
        return IPC_SUCCESS;
    }

    return IPC_ERROR_MEMORY_CHECK;
}

uint32_t tfm_spm_init(void)
{
    uint32_t i, j, num;
    struct partition_t *partition;
    struct tfm_core_thread_t *pth, *p_ns_entry_thread = NULL;
    const struct tfm_spm_partition_platform_data_t **platform_data_p;

    tfm_pool_init(conn_handle_pool,
                  POOL_BUFFER_SIZE(conn_handle_pool),
                  sizeof(struct tfm_conn_handle_t),
                  TFM_CONN_HANDLE_MAX_NUM);

    /* Init partition first for it will be used when init service */
    for (i = 0; i < g_spm_partition_db.partition_count; i++) {
        partition = &g_spm_partition_db.partitions[i];

        if (!partition || !partition->memory_data || !partition->static_data) {
            tfm_core_panic();
        }

        if (!(partition->static_data->partition_flags & SPM_PART_FLAG_IPC)) {
            tfm_core_panic();
        }

        /* Check if the PSA framework version matches. */
        if (partition->static_data->psa_framework_version !=
            PSA_FRAMEWORK_VERSION) {
            ERROR_MSG("Warning: PSA Framework Verison does not match!");
            continue;
        }

        platform_data_p = partition->platform_data_list;
        if (platform_data_p != NULL) {
            while ((*platform_data_p) != NULL) {
                if (tfm_spm_hal_configure_default_isolation(i,
                            *platform_data_p) != TFM_PLAT_ERR_SUCCESS) {
                    tfm_core_panic();
                }
                ++platform_data_p;
            }
        }

        /* Add PSA_DOORBELL signal to assigned_signals */
        partition->signals_allowed |= PSA_DOORBELL;

        /* TODO: This can be optimized by generating the assigned signal
         *       in code generation time.
         */
        for (j = 0; j < tfm_core_irq_signals_count; ++j) {
            if (tfm_core_irq_signals[j].partition_id ==
                                        partition->static_data->partition_id) {
                partition->signals_allowed |=
                                        tfm_core_irq_signals[j].signal_value;
            }
        }

        tfm_event_init(&partition->event);
        tfm_list_init(&partition->msg_list);

        pth = &partition->sp_thread;
        if (!pth) {
            tfm_core_panic();
        }

        tfm_core_thrd_init(pth,
                           (tfm_core_thrd_entry_t)
                                         partition->static_data->partition_init,
                           NULL,
                           (uintptr_t)partition->memory_data->stack_top,
                           (uintptr_t)partition->memory_data->stack_bottom);

        pth->prior = partition->static_data->partition_priority;

        if (partition->static_data->partition_id == TFM_SP_NON_SECURE_ID) {
            p_ns_entry_thread = pth;
            pth->param = (void *)tfm_spm_hal_get_ns_entry_point();
        }

        /* Kick off */
        if (tfm_core_thrd_start(pth) != THRD_SUCCESS) {
            tfm_core_panic();
        }
    }

    /* Init Service */
    num = sizeof(service) / sizeof(struct tfm_spm_service_t);
    for (i = 0; i < num; i++) {
        service[i].service_db = &service_db[i];
        partition =
            tfm_spm_get_partition_by_id(service[i].service_db->partition_id);
        if (!partition) {
            tfm_core_panic();
        }
        service[i].partition = partition;
        partition->signals_allowed |= service[i].service_db->signal;

        tfm_list_init(&service[i].handle_list);
    }

    /*
     * All threads initialized, start the scheduler.
     *
     * NOTE:
     * It is worthy to give the thread object to scheduler if the background
     * context belongs to one of the threads. Here the background thread is the
     * initialization thread who calls SPM SVC, which re-uses the non-secure
     * entry thread's stack. After SPM initialization is done, this stack is
     * cleaned up and the background context is never going to return. Tell
     * the scheduler that the current thread is non-secure entry thread.
     */
    tfm_core_thrd_start_scheduler(p_ns_entry_thread);

    return p_ns_entry_thread->arch_ctx.lr;
}

void tfm_pendsv_do_schedule(struct tfm_arch_ctx_t *p_actx)
{
#if TFM_LVL != 1
    struct partition_t *p_next_partition;
    uint32_t is_privileged;
#endif
    struct tfm_core_thread_t *pth_next = tfm_core_thrd_get_next_thread();
    struct tfm_core_thread_t *pth_curr = tfm_core_thrd_get_curr_thread();

    if (pth_next != NULL && pth_curr != pth_next) {
#if TFM_LVL != 1
        p_next_partition = TFM_GET_CONTAINER_PTR(pth_next,
                                                 struct partition_t,
                                                 sp_thread);

        if (p_next_partition->static_data->partition_flags &
            SPM_PART_FLAG_PSA_ROT) {
            is_privileged = TFM_PARTITION_PRIVILEGED_MODE;
        } else {
            is_privileged = TFM_PARTITION_UNPRIVILEGED_MODE;
        }

        tfm_spm_partition_change_privilege(is_privileged);
#if TFM_LVL == 3
        /*
         * FIXME: To implement isolations among partitions in isolation level 3,
         * each partition needs to run in unprivileged mode. Currently some
         * PRoTs cannot work in unprivileged mode, make them privileged now.
         */
        if (is_privileged == TFM_PARTITION_UNPRIVILEGED_MODE) {
            /* FIXME: only MPU-based implementations are supported currently */
            if (tfm_hal_mpu_update_partition_boundary(
                                      p_next_partition->memory_data->data_start,
                                      p_next_partition->memory_data->data_limit)
                                                           != TFM_HAL_SUCCESS) {
                tfm_core_panic();
            }
        }
#endif /* TFM_LVL == 3 */
#endif /* TFM_LVL != 1 */

        tfm_core_thrd_switch_context(p_actx, pth_curr, pth_next);
    }

    /*
     * Handle pending mailbox message from NS in multi-core topology.
     * Empty operation on single Armv8-M platform.
     */
    tfm_rpc_client_call_handler();
}

void update_caller_outvec_len(struct tfm_msg_body_t *msg)
{
    uint32_t i;

    /*
     * FixeMe: abstract these part into dedicated functions to avoid
     * accessing thread context in psa layer
     */
    /* If it is a NS request via RPC, the owner of this message is not set */
    if (!is_tfm_rpc_msg(msg)) {
        TFM_CORE_ASSERT(msg->ack_evnt.owner->state == THRD_STATE_BLOCK);
    }

    for (i = 0; i < PSA_MAX_IOVEC; i++) {
        if (msg->msg.out_size[i] == 0) {
            continue;
        }

        TFM_CORE_ASSERT(msg->caller_outvec[i].base == msg->outvec[i].base);

        msg->caller_outvec[i].len = msg->outvec[i].len;
    }
}

void notify_with_signal(int32_t partition_id, psa_signal_t signal)
{
    struct partition_t *partition = NULL;

    /*
     * The value of partition_id must be greater than zero as the target of
     * notification must be a Secure Partition, providing a Non-secure
     * Partition ID is a fatal error.
     */
    if (!TFM_CLIENT_ID_IS_S(partition_id)) {
        tfm_core_panic();
    }

    /*
     * It is a fatal error if partition_id does not correspond to a Secure
     * Partition.
     */
    partition = tfm_spm_get_partition_by_id(partition_id);
    if (!partition) {
        tfm_core_panic();
    }

    partition->signals_asserted |= signal;

    /*
     * The target partition may be blocked with waiting for signals after
     * called psa_wait(). Set the return value with the available signals
     * before wake it up with tfm_event_signal().
     */
    tfm_event_wake(&partition->event,
                   partition->signals_asserted & partition->signals_waiting);
}

/**
 * \brief assert signal for a given IRQ line.
 *
 * \param[in] partition_id      The ID of the partition which handles this IRQ
 * \param[in] signal            The signal associated with this IRQ
 * \param[in] irq_line          The number of the IRQ line
 *
 * \retval void                 Success.
 * \retval "Does not return"    Partition ID is invalid
 */
void tfm_irq_handler(uint32_t partition_id, psa_signal_t signal,
                     IRQn_Type irq_line)
{
    tfm_spm_hal_disable_irq(irq_line);
    notify_with_signal(partition_id, signal);
}

int32_t get_irq_line_for_signal(int32_t partition_id,
                                psa_signal_t signal,
                                IRQn_Type *irq_line)
{
    size_t i;

    for (i = 0; i < tfm_core_irq_signals_count; ++i) {
        if (tfm_core_irq_signals[i].partition_id == partition_id &&
            tfm_core_irq_signals[i].signal_value == signal) {
            *irq_line = tfm_core_irq_signals[i].irq_line;
            return IPC_SUCCESS;
        }
    }
    return IPC_ERROR_GENERIC;
}

void tfm_spm_enable_irq(uint32_t *args)
{
    struct tfm_state_context_t *svc_ctx = (struct tfm_state_context_t *)args;
    psa_signal_t irq_signal = svc_ctx->r0;
    IRQn_Type irq_line = (IRQn_Type) 0;
    int32_t ret;
    struct partition_t *partition = NULL;

    /* It is a fatal error if passed signal indicates more than one signals. */
    if (!tfm_is_one_bit_set(irq_signal)) {
        tfm_core_panic();
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    ret = get_irq_line_for_signal(partition->static_data->partition_id,
                                  irq_signal, &irq_line);
    /* It is a fatal error if passed signal is not an interrupt signal. */
    if (ret != IPC_SUCCESS) {
        tfm_core_panic();
    }

    tfm_spm_hal_enable_irq(irq_line);
}

void tfm_spm_disable_irq(uint32_t *args)
{
    struct tfm_state_context_t *svc_ctx = (struct tfm_state_context_t *)args;
    psa_signal_t irq_signal = svc_ctx->r0;
    IRQn_Type irq_line = (IRQn_Type) 0;
    int32_t ret;
    struct partition_t *partition = NULL;

    /* It is a fatal error if passed signal indicates more than one signals. */
    if (!tfm_is_one_bit_set(irq_signal)) {
        tfm_core_panic();
    }

    partition = tfm_spm_get_running_partition();
    if (!partition) {
        tfm_core_panic();
    }

    ret = get_irq_line_for_signal(partition->static_data->partition_id,
                                  irq_signal, &irq_line);
    /* It is a fatal error if passed signal is not an interrupt signal. */
    if (ret != IPC_SUCCESS) {
        tfm_core_panic();
    }

    tfm_spm_hal_disable_irq(irq_line);
}

void tfm_spm_validate_caller(struct partition_t *p_cur_sp, uint32_t *p_ctx,
                             uint32_t exc_return, bool ns_caller)
{
    uintptr_t stacked_ctx_pos;

    if (ns_caller) {
        /*
         * The background IRQ can't be supported, since if SP is executing,
         * the preempted context of SP can be different with the one who
         * preempts veneer.
         */
        if (p_cur_sp->static_data->partition_id != TFM_SP_NON_SECURE_ID) {
            tfm_core_panic();
        }

        /*
         * It is non-secure caller, check if veneer stack contains
         * multiple contexts.
         */
        stacked_ctx_pos = (uintptr_t)p_ctx +
                          sizeof(struct tfm_state_context_t) +
                          TFM_STACK_SEALED_SIZE;

        if (is_stack_alloc_fp_space(exc_return)) {
#if defined (__FPU_USED) && (__FPU_USED == 1U)
            if (FPU->FPCCR & FPU_FPCCR_TS_Msk) {
                stacked_ctx_pos += TFM_ADDTIONAL_FP_CONTEXT_WORDS *
                                   sizeof(uint32_t);
            }
#endif
            stacked_ctx_pos += TFM_BASIC_FP_CONTEXT_WORDS * sizeof(uint32_t);
        }

        if (stacked_ctx_pos != p_cur_sp->sp_thread.stk_top) {
            tfm_core_panic();
        }
    } else if (p_cur_sp->static_data->partition_id <= 0) {
        tfm_core_panic();
    }
}

void tfm_spm_request_handler(const struct tfm_state_context_t *svc_ctx)
{
    uint32_t *res_ptr = (uint32_t *)&svc_ctx->r0;
    uint32_t running_partition_flags = 0;
    const struct partition_t *partition = NULL;

    /* Check permissions on request type basis */

    switch (svc_ctx->r0) {
    case TFM_SPM_REQUEST_RESET_VOTE:
        partition = tfm_spm_get_running_partition();
        if (!partition) {
            tfm_core_panic();
        }
        running_partition_flags = partition->static_data->partition_flags;

        /* Currently only PSA Root of Trust services are allowed to make Reset
         * vote request
         */
        if ((running_partition_flags & SPM_PART_FLAG_PSA_ROT) == 0) {
            *res_ptr = (uint32_t)TFM_ERROR_GENERIC;
        }

        /* FixMe: this is a placeholder for checks to be performed before
         * allowing execution of reset
         */
        *res_ptr = (uint32_t)TFM_SUCCESS;

        break;
    default:
        *res_ptr = (uint32_t)TFM_ERROR_INVALID_PARAMETER;
    }
}

enum spm_err_t tfm_spm_db_init(void)
{
    uint32_t i;

    /* This function initialises partition db */

    for (i = 0; i < g_spm_partition_db.partition_count; i++) {
        g_spm_partition_db.partitions[i].static_data = &static_data_list[i];
        g_spm_partition_db.partitions[i].platform_data_list =
                                                     platform_data_list_list[i];
        g_spm_partition_db.partitions[i].memory_data = &memory_data_list[i];
    }
    g_spm_partition_db.is_init = 1;

    return SPM_ERR_OK;
}
