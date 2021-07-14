/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <inttypes.h>
#include <stdbool.h>
#include "bitops.h"
#include "fih.h"
#include "psa/client.h"
#include "psa/service.h"
#include "tfm_thread.h"
#include "tfm_wait.h"
#include "internal_errors.h"
#include "tfm_spm_hal.h"
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
#include "lists.h"
#include "tfm_pools.h"
#include "spm_partition_defs.h"
#include "psa_manifest/pid.h"
#include "tfm/tfm_spm_services.h"
#include "load/partition_defs.h"
#include "load/service_defs.h"
#include "load/asset_defs.h"
#include "load/spm_load_api.h"
#include "load/irq_defs.h"

/* Partition and service runtime data list head/runtime data table */
static struct partition_head_t partitions_listhead;
static struct service_head_t services_listhead;
struct service_t *stateless_services_ref_tbl[STATIC_HANDLE_NUM_LIMIT];

/* Pools */
TFM_POOL_DECLARE(conn_handle_pool, sizeof(struct tfm_conn_handle_t),
                 TFM_CONN_HANDLE_MAX_NUM);

void spm_interrupt_handler(struct partition_load_info_t *p_ldinf,
                           psa_signal_t signal,
                           uint32_t irq_line,
                           psa_flih_func flih_func);

#include "tfm_secure_irq_handlers_ipc.inc"

/*********************** Connection handle conversion APIs *******************/

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
struct tfm_conn_handle_t *tfm_spm_create_conn_handle(struct service_t *service,
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
    BI_LIST_INSERT_BEFORE(&service->handle_list, &p_handle->list);

    return p_handle;
}

int32_t tfm_spm_validate_conn_handle(
                                    const struct tfm_conn_handle_t *conn_handle,
                                    int32_t client_id)
{
    /* Check the handle address is validated */
    if (is_valid_chunk_data_in_pool(conn_handle_pool,
                                    (uint8_t *)conn_handle) != true) {
        return SPM_ERROR_GENERIC;
    }

    /* Check the handle caller is correct */
    if (conn_handle->client_id != client_id) {
        return SPM_ERROR_GENERIC;
    }

    return SPM_SUCCESS;
}

int32_t tfm_spm_free_conn_handle(struct service_t *service,
                                 struct tfm_conn_handle_t *conn_handle)
{
    TFM_CORE_ASSERT(service);
    TFM_CORE_ASSERT(conn_handle != NULL);

    /* Clear magic as the handler is not used anymore */
    conn_handle->internal_msg.magic = 0;

    /* Remove node from handle list */
    BI_LIST_REMOVE_NODE(&conn_handle->list);

    /* Back handle buffer to pool */
    tfm_pool_free(conn_handle_pool, conn_handle);
    return SPM_SUCCESS;
}

int32_t tfm_spm_set_rhandle(struct service_t *service,
                            struct tfm_conn_handle_t *conn_handle,
                            void *rhandle)
{
    TFM_CORE_ASSERT(service);
    /* Set reverse handle value only be allowed for a connected handle */
    TFM_CORE_ASSERT(conn_handle != NULL);

    conn_handle->rhandle = rhandle;
    return SPM_SUCCESS;
}

/**
 * \brief                   Get reverse handle value from connection handle.
 *
 * \param[in] service       Target service context pointer
 * \param[in] conn_handle   Connection handle created by
 *                          tfm_spm_create_conn_handle()
 *
 * \retval void *           Success
 * \retval "Does not return"  Panic for those:
 *                              service pointer are NULL
 *                              handle is \ref PSA_NULL_HANDLE
 *                              handle node does not be found
 */
static void *tfm_spm_get_rhandle(struct service_t *service,
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
    struct bi_list_node_t *node, *head;
    struct tfm_msg_body_t *tmp_msg, *msg = NULL;

    TFM_CORE_ASSERT(partition);

    head = &partition->msg_list;

    if (BI_LIST_IS_EMPTY(head)) {
        return NULL;
    }

    /*
     * There may be multiple messages for this RoT Service signal, do not clear
     * partition mask until no remaining message. Search may be optimized.
     */
    BI_LIST_FOR_EACH(node, head) {
        tmp_msg = TO_CONTAINER(node, struct tfm_msg_body_t, msg_node);
        if (tmp_msg->service->p_ldinf->signal == signal && msg) {
            return msg;
        } else if (tmp_msg->service->p_ldinf->signal == signal) {
            msg = tmp_msg;
            BI_LIST_REMOVE_NODE(node);
        }
    }

    partition->signals_asserted &= ~signal;

    return msg;
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
#if TFM_LVL == 1
    return TFM_PARTITION_PRIVILEGED_MODE;
#else /* TFM_LVL == 1 */
    if (partition_flags & SPM_PART_FLAG_PSA_ROT) {
        return TFM_PARTITION_PRIVILEGED_MODE;
    } else {
        return TFM_PARTITION_UNPRIVILEGED_MODE;
    }
#endif /* TFM_LVL == 1 */
}

struct service_t *tfm_spm_get_service_by_sid(uint32_t sid)
{
    struct service_t *p_serv;

    UNI_LIST_FOR_EACH(p_serv, &services_listhead) {
        if (p_serv->p_ldinf->sid == sid) {
            return p_serv;
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
struct partition_t *tfm_spm_get_partition_by_id(int32_t partition_id)
{
    struct partition_t *p_part;

    UNI_LIST_FOR_EACH(p_part, &partitions_listhead) {
        if (p_part->p_ldinf->pid == partition_id) {
            return p_part;
        }
    }

    return NULL;
}

struct partition_t *tfm_spm_get_running_partition(void)
{
    struct tfm_core_thread_t *pth = tfm_core_thrd_get_curr();
    struct partition_t *partition;

    partition = TO_CONTAINER(pth, struct partition_t, sp_thread);

    return partition;
}

int32_t tfm_spm_check_client_version(struct service_t *service,
                                     uint32_t version)
{
    TFM_CORE_ASSERT(service);

    switch (SERVICE_GET_VERSION_POLICY(service->p_ldinf->flags)) {
    case SERVICE_VERSION_POLICY_RELAXED:
        if (version > service->p_ldinf->version) {
            return SPM_ERROR_VERSION;
        }
        break;
    case SERVICE_VERSION_POLICY_STRICT:
        if (version != service->p_ldinf->version) {
            return SPM_ERROR_VERSION;
        }
        break;
    default:
        return SPM_ERROR_VERSION;
    }
    return SPM_SUCCESS;
}

int32_t tfm_spm_check_authorization(uint32_t sid,
                                    struct service_t *service,
                                    bool ns_caller)
{
    struct partition_t *partition = NULL;
    uint32_t *dep;
    int32_t i;

    TFM_CORE_ASSERT(service);

    if (ns_caller) {
        if (!SERVICE_IS_NS_ACCESSIBLE(service->p_ldinf->flags)) {
            return SPM_ERROR_GENERIC;
        }
    } else {
        partition = tfm_spm_get_running_partition();
        if (!partition) {
            tfm_core_panic();
        }

        dep = (uint32_t *)LOAD_INFO_DEPS(partition->p_ldinf);
        for (i = 0; i < partition->p_ldinf->ndeps; i++) {
            if (dep[i] == sid) {
                break;
            }
        }

        if (i == partition->p_ldinf->ndeps) {
            return SPM_ERROR_GENERIC;
        }
    }
    return SPM_SUCCESS;
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
    if (partition_id != p_msg->service->partition->p_ldinf->pid) {
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
                      struct service_t *service,
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

void tfm_spm_send_event(struct service_t *service,
                        struct tfm_msg_body_t *msg)
{
    struct partition_t *partition = NULL;
    psa_signal_t signal = 0;

    if (!msg || !service || !service->p_ldinf || !service->partition) {
        tfm_core_panic();
    }

    partition = service->partition;
    signal = service->p_ldinf->signal;

    /* Add message to partition message list tail */
    BI_LIST_INSERT_BEFORE(&partition->msg_list, &msg->msg_node);

    /* Messages put. Update signals */
    partition->signals_asserted |= signal;

    if (partition->signals_waiting & signal) {
        tfm_event_wake(
                    &partition->event,
                    (partition->signals_asserted & partition->signals_waiting));
        partition->signals_waiting &= ~signal;
    }

    /*
     * If it is a NS request via RPC, it is unnecessary to block current
     * thread.
     */
    if (!is_tfm_rpc_msg(msg)) {
        tfm_event_wait(&msg->ack_evnt);
    }
}

uint32_t tfm_spm_partition_get_running_partition_id(void)
{
    struct partition_t *partition;

    partition = tfm_spm_get_running_partition();
    if (partition && partition->p_ldinf) {
        return partition->p_ldinf->pid;
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
        return SPM_SUCCESS;
    }

    if (!buffer) {
        return SPM_ERROR_BAD_PARAMETERS;
    }

    if ((uintptr_t)buffer > (UINTPTR_MAX - len)) {
        return SPM_ERROR_MEMORY_CHECK;
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
        return SPM_SUCCESS;
    }

    return SPM_ERROR_MEMORY_CHECK;
}

uint32_t tfm_spm_init(void)
{
    uint32_t i;
    bool privileged;
    struct partition_t *partition;
    struct tfm_core_thread_t *pth, *p_ns_entry_thread = NULL;
    const struct platform_data_t *platform_data_p;
    const struct partition_load_info_t *p_ldinf;
    struct asset_desc_t *p_asset_load;
#ifdef TFM_FIH_PROFILE_ON
    fih_int fih_rc = FIH_FAILURE;
#endif

    tfm_pool_init(conn_handle_pool,
                  POOL_BUFFER_SIZE(conn_handle_pool),
                  sizeof(struct tfm_conn_handle_t),
                  TFM_CONN_HANDLE_MAX_NUM);

    UNI_LISI_INIT_HEAD(&partitions_listhead);
    UNI_LISI_INIT_HEAD(&services_listhead);

    while (1) {
        partition = load_a_partition_assuredly(&partitions_listhead);
        if (partition == NULL) {
            break;
        }

        p_ldinf = partition->p_ldinf;

        if (p_ldinf->nservices) {
            load_services_assuredly(partition, &services_listhead,
                                    stateless_services_ref_tbl,
                                    sizeof(stateless_services_ref_tbl));
        }

        if (p_ldinf->nirqs) {
            load_irqs_assuredly(partition);
        }

        /* Init mmio assets */
        if (p_ldinf->nassets > 0) {
            if (tfm_spm_partition_get_privileged_mode(p_ldinf->flags) ==
                TFM_PARTITION_PRIVILEGED_MODE) {
                privileged = true;
            } else {
                privileged = false;
            }
        }

        p_asset_load = (struct asset_desc_t *)LOAD_INFO_ASSET(p_ldinf);
        for (i = 0; i < p_ldinf->nassets; i++) {
            /* Skip the memory-based asset */
            if (!(p_asset_load[i].attr & ASSET_DEV_REF_BIT)) {
                continue;
            }

            platform_data_p = REFERENCE_TO_PTR(p_asset_load[i].dev.addr_ref,
                                               struct platform_data_t *);

            /*
             * TODO: some partitions declare MMIO not exist on specific
             * platforms, and the platform defines a dummy NULL reference
             * for these MMIO items, which cause 'nassets' to contain several
             * NULL items. Skip these NULL items initialization temporarily to
             * avoid HAL API panic.
             * Eventually, these platform-specific partitions need to be moved
             * into a platform-specific folder. Then this workaround can be
             * removed.
             */
            if (!platform_data_p) {
                continue;
            }

#ifdef TFM_FIH_PROFILE_ON
            FIH_CALL(tfm_spm_hal_configure_default_isolation, fih_rc,
                     privileged, platform_data_p);
            if (fih_not_eq(fih_rc, fih_int_encode(TFM_PLAT_ERR_SUCCESS))) {
                tfm_core_panic();
            }
#else /* TFM_FIH_PROFILE_ON */
            if (tfm_spm_hal_configure_default_isolation(privileged,
                platform_data_p) != TFM_PLAT_ERR_SUCCESS) {
                tfm_core_panic();
            }
#endif /* TFM_FIH_PROFILE_ON */
        }

        partition->signals_allowed |= PSA_DOORBELL;

        tfm_event_init(&partition->event);
        BI_LIST_INIT_NODE(&partition->msg_list);

        pth = &partition->sp_thread;
        if (!pth) {
            tfm_core_panic();
        }

        /* Extendable partition load info is right after p_ldinf. */
        tfm_core_thrd_init(
                    pth,
                    POSITION_TO_ENTRY(p_ldinf->entry, tfm_core_thrd_entry_t),
                    NULL,
                    LOAD_ALLOCED_STACK_ADDR(p_ldinf) + p_ldinf->stack_size,
                    LOAD_ALLOCED_STACK_ADDR(p_ldinf));

        pth->prior = TO_THREAD_PRIORITY(PARTITION_PRIORITY(p_ldinf->flags));

        if (p_ldinf->pid == TFM_SP_NON_SECURE_ID) {
            p_ns_entry_thread = pth;
            pth->param = (void *)tfm_spm_hal_get_ns_entry_point();
        }

        /* Kick off */
        if (tfm_core_thrd_start(pth) != THRD_SUCCESS) {
            tfm_core_panic();
        }
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

#if TFM_LVL != 1
static void set_up_boundary(const struct partition_load_info_t *p_ldinf)
{
#if TFM_LVL == 3
#if defined(TFM_FIH_PROFILE_ON) && (TFM_LVL == 3)
    fih_int fih_rc = FIH_FAILURE;
#endif
    /*
     * FIXME: To implement isolations among partitions in isolation level 3,
     * each partition needs to run in unprivileged mode. Currently some
     * PRoTs cannot work in unprivileged mode, make them privileged now.
     */
    if (!(p_ldinf->flags & SPM_PART_FLAG_PSA_ROT)) {
        struct asset_desc_t *p_asset =
            (struct asset_desc_t *)LOAD_INFO_ASSET(p_ldinf);
        /* Partition must have private data as the first asset in LVL3 */
        if (p_ldinf->nassets == 0) {
            tfm_core_panic();
        }
        if (p_asset->attr & ASSET_DEV_REF_BIT) {
            tfm_core_panic();
        }
        /* FIXME: only MPU-based implementations are supported currently */
#ifdef TFM_FIH_PROFILE_ON
        FIH_CALL(tfm_hal_mpu_update_partition_boundary, fih_rc,
                    p_asset->mem.addr_x, p_asset->mem.addr_y);
        if (fih_not_eq(fih_rc, fih_int_encode(TFM_HAL_SUCCESS))) {
            tfm_core_panic();
        }
#else /* TFM_FIH_PROFILE_ON */
        if (tfm_hal_mpu_update_partition_boundary(p_asset->mem.addr_x,
                                                  p_asset->mem.addr_y)
                                                           != TFM_HAL_SUCCESS) {
            tfm_core_panic();
        }
#endif /* TFM_FIH_PROFILE_ON */
    }
#else /* TFM_LVL == 3 */
    (void)p_ldinf;
#endif /* TFM_LVL == 3 */
}
#endif /* TFM_LVL != 1 */

void tfm_set_up_isolation_boundary(const struct partition_t *partition)
{
#if TFM_LVL != 1
    const struct partition_load_info_t *p_ldinf;
    uint32_t is_privileged;

    p_ldinf = partition->p_ldinf;
    is_privileged = p_ldinf->flags & SPM_PART_FLAG_PSA_ROT ?
                                                TFM_PARTITION_PRIVILEGED_MODE :
                                                TFM_PARTITION_UNPRIVILEGED_MODE;

    tfm_spm_partition_change_privilege(is_privileged);

    set_up_boundary(p_ldinf);
#else /* TFM_LVL != 1 */
    (void)partition;
#endif /* TFM_LVL != 1 */
}

void tfm_pendsv_do_schedule(struct tfm_arch_ctx_t *p_actx)
{
    struct partition_t *p_next_partition;
    struct tfm_core_thread_t *pth_next = tfm_core_thrd_get_next();
    struct tfm_core_thread_t *pth_curr = tfm_core_thrd_get_curr();

    if (pth_next != NULL && pth_curr != pth_next) {
        p_next_partition = TO_CONTAINER(pth_next,
                                        struct partition_t,
                                        sp_thread);
        tfm_set_up_isolation_boundary(p_next_partition);

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

    if (partition->signals_waiting & signal) {
        tfm_event_wake(
                      &partition->event,
                      partition->signals_asserted & partition->signals_waiting);
        partition->signals_waiting &= ~signal;
    }
}

__attribute__((naked))
static void tfm_flih_deprivileged_handling(uint32_t p_ldinf,
                                           psa_flih_func flih_func,
                                           psa_signal_t signal)
{
    __ASM volatile("SVC %0           \n"
                   "BX LR            \n"
                   : : "I" (TFM_SVC_PREPARE_DEPRIV_FLIH));
}

void spm_interrupt_handler(struct partition_load_info_t *p_ldinf,
                           psa_signal_t signal,
                           uint32_t irq_line,
                           psa_flih_func flih_func)
{
    uint32_t pid;
    psa_flih_result_t flih_result;

    pid = p_ldinf->pid;

    if (flih_func == NULL) {
        /* SLIH Model Handling */
        __disable_irq();
        tfm_spm_hal_disable_irq(irq_line);
        notify_with_signal(pid, signal);
        __enable_irq();
        return;
    }

    /* FLIH Model Handling */
    if (tfm_spm_partition_get_privileged_mode(p_ldinf->flags) ==
                                                TFM_PARTITION_PRIVILEGED_MODE) {
        flih_result = flih_func();
        if (flih_result == PSA_FLIH_SIGNAL) {
            __disable_irq();
            notify_with_signal(pid, signal);
            __enable_irq();
        } else if (flih_result != PSA_FLIH_NO_SIGNAL) {
            /*
             * Nothing needed to do for PSA_FLIH_NO_SIGNAL
             * But if the flih_result is invalid, should panic.
             */
            tfm_core_panic();
        }
    } else {
        tfm_flih_deprivileged_handling((uint32_t)p_ldinf, flih_func, signal);
    }
}

struct irq_load_info_t *get_irq_info_for_signal(
                                    const struct partition_load_info_t *p_ldinf,
                                    psa_signal_t signal)
{
    size_t i;
    struct irq_load_info_t *irq_info;

    if (!IS_ONLY_ONE_BIT_IN_UINT32(signal)) {
        return NULL;
    }

    irq_info = (struct irq_load_info_t *)LOAD_INFO_IRQ(p_ldinf);
    for (i = 0; i < p_ldinf->nirqs; i++) {
        if (irq_info[i].signal == signal) {
            return &irq_info[i];
        }
    }

    return NULL;
}

#if !defined(__ARM_ARCH_8_1M_MAIN__)
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
        if (p_cur_sp->p_ldinf->pid != TFM_SP_NON_SECURE_ID) {
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
#if defined(__FPU_USED) && (__FPU_USED == 1U)
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
    } else if (p_cur_sp->p_ldinf->pid <= 0) {
        tfm_core_panic();
    }
}
#endif

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
        running_partition_flags = partition->p_ldinf->flags;

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
