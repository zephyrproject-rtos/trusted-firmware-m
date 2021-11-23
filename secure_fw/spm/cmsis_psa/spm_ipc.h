/*
 * Copyright (c) 2020-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SPM_IPC_H__
#define __SPM_IPC_H__

#include <stdint.h>
#include "config_impl.h"
#include "tfm_arch.h"
#include "lists.h"
#include "tfm_secure_api.h"
#include "thread.h"
#include "psa/service.h"
#include "load/interrupt_defs.h"

#define TFM_HANDLE_STATUS_IDLE          0
#define TFM_HANDLE_STATUS_ACTIVE        1
#define TFM_HANDLE_STATUS_CONNECT_ERROR 2

#define TFM_CONN_HANDLE_MAX_NUM         16

/* Privileged definitions for partition thread mode */
#define TFM_PARTITION_UNPRIVILEGED_MODE         (0U)
#define TFM_PARTITION_PRIVILEGED_MODE           (1U)

/*
 * Set a number limit for stateless handle.
 * Valid handle must be positive, set client handle minimum value to 1.
 */
#define STATIC_HANDLE_NUM_LIMIT         32
#define CLIENT_HANDLE_VALUE_MIN         1

#define STAIC_HANDLE_IDX_BIT_WIDTH      8
#define STAIC_HANDLE_IDX_MASK \
    (uint32_t)((1UL << STAIC_HANDLE_IDX_BIT_WIDTH) - 1)
#define GET_INDEX_FROM_STATIC_HANDLE(handle) \
    (uint32_t)((handle) & STAIC_HANDLE_IDX_MASK)

#define STAIC_HANDLE_VER_BIT_WIDTH      8
#define STAIC_HANDLE_VER_OFFSET         8
#define STAIC_HANDLE_VER_MASK \
    (uint32_t)((1UL << STAIC_HANDLE_VER_BIT_WIDTH) - 1)
#define GET_VERSION_FROM_STATIC_HANDLE(handle) \
    (uint32_t)(((handle) >> STAIC_HANDLE_VER_OFFSET) & STAIC_HANDLE_VER_MASK)

/* Validate the static handle indicator bit */
#define STAIC_HANDLE_INDICATOR_OFFSET   30
#define IS_STATIC_HANDLE(handle) \
    ((handle) & (1UL << STAIC_HANDLE_INDICATOR_OFFSET))

/* Valid index should be [0, STATIC_HANDLE_NUM_LIMIT-1] */
#define IS_VALID_STATIC_HANDLE_IDX(index) \
    (((index) >= 0) && ((index) < STATIC_HANDLE_NUM_LIMIT))

#define SPM_INVALID_PARTITION_IDX     (~0U)

#define TFM_MSG_MAGIC                   0x15154343
#define TFM_MSG_MAGIC_SFN               0x21216565

/* Get partition by thread or context data */
#define GET_THRD_OWNER(x)        TO_CONTAINER(x, struct partition_t, thrd)
#define GET_CTX_OWNER(x)         TO_CONTAINER(x, struct partition_t, ctx_ctrl)

/* Message struct to collect parameter from client */
struct tfm_msg_body_t {
    int32_t magic;
    struct partition_t *p_client;      /* Caller partition              */
    struct service_t *service;         /* RoT service pointer           */
    union {
        struct sync_obj_t ack_evnt;    /* IPC - Ack response event       */
        uint32_t sfn_magic;            /* SFN - Indicate a SFN message   */
    };
    psa_msg_t msg;                     /* PSA message body               */
    psa_invec invec[PSA_MAX_IOVEC];    /* Put in/out vectors in msg body */
    psa_outvec outvec[PSA_MAX_IOVEC];
    psa_outvec *caller_outvec;         /*
                                        * Save caller outvec pointer for
                                        * write length update
                                        */
#ifdef TFM_MULTI_CORE_TOPOLOGY
    const void *caller_data;           /*
                                        * Pointer to the private data of the
                                        * caller. It identifies the NSPE PSA
                                        * client calls in multi-core topology
                                        */
#endif
#if PSA_FRAMEWORK_HAS_MM_IOVEC
    uint32_t iovec_status;             /* MM-IOVEC status                */
#endif
    struct bi_list_node_t msg_node;    /* For list operators             */
};

/* Partition runtime type */
struct partition_t {
    const struct partition_load_info_t *p_ldinf;
    void                               *p_boundaries;
    void                               *p_interrupts;
    void                               *p_metadata;
    union {
        struct thread_t                thrd;            /* IPC model */
        uint32_t                       state;           /* SFN model */
    };
    struct sync_obj_t                  waitobj;
    struct context_ctrl_t              ctx_ctrl;
    union {
        struct bi_list_node_t          msg_list;        /* IPC model */
        struct tfm_msg_body_t          *p_msg;          /* SFN model */
    };
    uint32_t                           signals_allowed;
    uint32_t                           signals_waiting;
    uint32_t                           signals_asserted;
    struct partition_t                 *next;
};

/* RoT Service data */
struct service_t {
    const struct service_load_info_t *p_ldinf;     /* Service load info      */
    struct partition_t *partition;                 /* Owner of the service   */
    struct bi_list_node_t handle_list;             /* Service handle list    */
    struct service_t *next;                        /* For list operation     */
};

/* RoT connection handle list */
struct tfm_conn_handle_t {
    void *rhandle;                      /* Reverse handle value              */
    uint32_t status;                    /*
                                         * Status of handle, three valid
                                         * options:
                                         * TFM_HANDLE_STATUS_ACTIVE,
                                         * TFM_HANDLE_STATUS_IDLE and
                                         * TFM_HANDLE_STATUS_CONNECT_ERROR
                                         */
    int32_t client_id;                  /*
                                         * Partition ID of the sender of the
                                         * message:
                                         *  - secure partition id;
                                         *  - non secure client endpoint id.
                                         */
    struct tfm_msg_body_t internal_msg; /* Internal message for message queue */
    struct bi_list_node_t list;         /* list node                          */
};

enum tfm_memory_access_e {
    TFM_MEMORY_ACCESS_RO = 1,
    TFM_MEMORY_ACCESS_RW = 2,
};

/**
 * \brief                   Get the privileged mode of Partition.
 *
 * \param[in] partition_flags               Flags of the Partition
 *
 * \retval TFM_PARTITION_PRIVILEGED_MODE    Privileged mode
 * \retval TFM_PARTITION_UNPRIVILEGED_MODE  Unprivileged mode
 */
uint32_t tfm_spm_partition_get_privileged_mode(uint32_t partition_flags);

/**
 * \brief   Get the running partition ID.
 *
 * \return  Returns the partition ID
 */
int32_t tfm_spm_partition_get_running_partition_id(void);

/******************** Service handle management functions ********************/

/**
 * \brief                   Create connection handle for client connect
 *
 * \param[in] service       Target service context pointer
 * \param[in] client_id     Partition ID of the sender of the message
 *
 * \retval NULL             Create failed
 * \retval "Not NULL"       Service handle created
 */
struct tfm_conn_handle_t *tfm_spm_create_conn_handle(struct service_t *service,
                                                     int32_t client_id);

/**
 * \brief                   Validate connection handle for client connect
 *
 * \param[in] conn_handle   Handle to be validated
 * \param[in] client_id     Partition ID of the sender of the message
 *
 * \retval SPM_SUCCESS        Success
 * \retval SPM_ERROR_GENERIC  Invalid handle
 */
int32_t tfm_spm_validate_conn_handle(
                                    const struct tfm_conn_handle_t *conn_handle,
                                    int32_t client_id);

/**
 * \brief                   Free connection handle which not used anymore.
 *
 * \param[in] service       Target service context pointer
 * \param[in] conn_handle   Connection handle created by
 *                          tfm_spm_create_conn_handle()
 *
 * \retval SPM_SUCCESS      Success
 * \retval SPM_ERROR_BAD_PARAMETERS  Bad parameters input
 * \retval "Does not return"  Panic for not find service by handle
 */
int32_t tfm_spm_free_conn_handle(struct service_t *service,
                                 struct tfm_conn_handle_t *conn_handle);

/******************** Partition management functions *************************/

/**
 * \brief                   Get the msg context by signal.
 *
 * \param[in] partition     Partition context pointer
 *                          \ref partition_t structures
 * \param[in] signal        Signal associated with inputs to the Secure
 *                          Partition, \ref psa_signal_t
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Target service context pointer,
 *                          \ref tfm_msg_body_t structures
 */
struct tfm_msg_body_t *tfm_spm_get_msg_by_signal(struct partition_t *partition,
                                                 psa_signal_t signal);


/**
 * \brief                   Get partition by Partition ID.
 *
 * \param[in] partition_id  The Partition ID of the partition to get
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Return the parttion context pointer
 *                          \ref partition_t structures
 */
struct partition_t *tfm_spm_get_partition_by_id(int32_t partition_id);

/**
 * \brief                   Get current running partition context.
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Return the parttion context pointer
 *                          \ref partition_t structures
 */
struct partition_t *tfm_spm_get_running_partition(void);

/**
 * \brief                   Get the service context by service ID.
 *
 * \param[in] sid           RoT Service identity
 *
 * \retval NULL             Failed
 * \retval "Not NULL"       Target service context pointer,
 *                          \ref service_t structures
 */
struct service_t *tfm_spm_get_service_by_sid(uint32_t sid);

/************************ Message functions **********************************/

/**
 * \brief                   Get message context by message handle.
 *
 * \param[in] msg_handle    Message handle which is a reference generated
 *                          by the SPM to a specific message.
 *
 * \return                  The message body context pointer
 *                          \ref tfm_msg_body_t structures
 */
struct tfm_msg_body_t *tfm_spm_get_msg_from_handle(psa_handle_t msg_handle);

/**
 * \brief                   Get message context by connect handle.
 *
 * \param[in] conn_handle   Service connect handle.
 *
 * \return                  The message body context pointer
 *                          \ref msg_body_t structures
 */
struct tfm_msg_body_t *
 tfm_spm_get_msg_buffer_from_conn_handle(struct tfm_conn_handle_t *conn_handle);

/**
 * \brief                   Fill the message for PSA client call.
 *
 * \param[in] msg           Service Message Queue buffer pointer
 * \param[in] service       Target service context pointer, which can be
 *                          obtained by partition management functions
 * \prarm[in] handle        Connect handle return by psa_connect().
 * \param[in] type          Message type, PSA_IPC_CONNECT, PSA_IPC_CALL or
 *                          PSA_IPC_DISCONNECT
 * \param[in] client_id     Partition ID of the sender of the message
 * \param[in] invec         Array of input \ref psa_invec structures
 * \param[in] in_len        Number of input \ref psa_invec structures
 * \param[in] outvec        Array of output \ref psa_outvec structures
 * \param[in] out_len       Number of output \ref psa_outvec structures
 * \param[in] caller_outvec Array of caller output \ref psa_outvec structures
 */
void tfm_spm_fill_msg(struct tfm_msg_body_t *msg,
                      struct service_t *service,
                      psa_handle_t handle,
                      int32_t type, int32_t client_id,
                      psa_invec *invec, size_t in_len,
                      psa_outvec *outvec, size_t out_len,
                      psa_outvec *caller_outvec);

/**
 * \brief                   Check the client version according to
 *                          version policy
 *
 * \param[in] service       Target service context pointer, which can be get
 *                          by partition management functions
 * \param[in] version       Client support version
 *
 * \retval SPM_SUCCESS      Success
 * \retval SPM_ERROR_BAD_PARAMETERS Bad parameters input
 * \retval SPM_ERROR_VERSION Check failed
 */
int32_t tfm_spm_check_client_version(struct service_t *service,
                                     uint32_t version);

/**
 * \brief                   Check the client access authorization
 *
 * \param[in] sid           Target RoT Service identity
 * \param[in] service       Target service context pointer, which can be get
 *                          by partition management functions
 * \param[in] ns_caller     Whether from NS caller
 *
 * \retval SPM_SUCCESS      Success
 * \retval SPM_ERROR_GENERIC Authorization check failed
 */
int32_t tfm_spm_check_authorization(uint32_t sid,
                                    struct service_t *service,
                                    bool ns_caller);

/**
 * \brief                      Check the memory reference is valid.
 *
 * \param[in] buffer           Pointer of memory reference
 * \param[in] len              Length of memory reference in bytes
 * \param[in] ns_caller        From non-secure caller
 * \param[in] access           Type of access specified by the
 *                             \ref tfm_memory_access_e
 * \param[in] privileged       Privileged mode or unprivileged mode:
 *                             \ref TFM_PARTITION_UNPRIVILEGED_MODE
 *                             \ref TFM_PARTITION_PRIVILEGED_MODE
 *
 * \retval SPM_SUCCESS               Success
 * \retval SPM_ERROR_BAD_PARAMETERS  Bad parameters input
 * \retval SPM_ERROR_MEMORY_CHECK    Check failed
 */
int32_t tfm_memory_check(const void *buffer, size_t len, bool ns_caller,
                         enum tfm_memory_access_e access,
                         uint32_t privileged);

/**
 * \brief                       Get the ns_caller info from runtime context.
 *
 * \retval                      - true: the PSA API caller is from non-secure
 *                              - false: the PSA API caller is from secure
 */
bool tfm_spm_is_ns_caller(void);

/**
 * \brief                       Get the privilege mode of service caller.
 *
 * \retval                      Privilege mode of the service caller
 *                              \ref TFM_PARTITION_UNPRIVILEGED_MODE
 *                              \ref TFM_PARTITION_PRIVILEGED_MODE
 */
uint32_t tfm_spm_get_caller_privilege_mode(void);

/**
 * \brief               Get ID of current RoT Service client.
 *                      This API ensures the caller gets a valid ID.
 *
 * \param[in] ns_caller If the client is Non-Secure or not.
 *
 * \retval              The client ID
 */
int32_t tfm_spm_get_client_id(bool ns_caller);

/*
 * PendSV specified function.
 *
 * Parameters :
 *  p_actx        -    Architecture context storage pointer
 *
 * Return:
 *  Pointers to context control (sp, splimit, dummy, lr) of the current and
 *  the next thread.
 *  Each takes 32 bits. The context control is used by PendSV_Handler to do
 *  context switch.
 */
uint64_t do_schedule(void);

/**
 * \brief                      SPM initialization implementation
 *
 * \details                    This function must be called under handler mode.
 * \retval                     This function returns an EXC_RETURN value. Other
 *                             faults would panic the execution and never
 *                             returned.
 */
uint32_t tfm_spm_init(void);

/**
 * \brief Converts a handle instance into a corresponded user handle.
 */
psa_handle_t tfm_spm_to_user_handle(struct tfm_conn_handle_t *handle_instance);

/**
 * \brief Converts a user handle into a corresponded handle instance.
 */
struct tfm_conn_handle_t *tfm_spm_to_handle_instance(psa_handle_t user_handle);

/**
 * \brief Move to handler mode by a SVC for specific purpose
 */
void tfm_core_handler_mode(void);

/**
 * \brief                   Set reverse handle value for connection.
 *
 * \param[in] service       Target service context pointer
 * \param[in] conn_handle   Connection handle created by
 *                          tfm_spm_create_conn_handle()
 * \param[in] rhandle       rhandle need to save
 *
 * \retval SPM_SUCCESS      Success
 * \retval SPM_ERROR_BAD_PARAMETERS  Bad parameters input
 * \retval "Does not return"  Panic for not find handle node
 */
int32_t tfm_spm_set_rhandle(struct service_t *service,
                            struct tfm_conn_handle_t *conn_handle,
                            void *rhandle);

void update_caller_outvec_len(struct tfm_msg_body_t *msg);

/*
 * Set partition signal.
 *
 * Assert a signal to given partition.
 */
void spm_assert_signal(void *p_pt, psa_signal_t signal);

/**
 * \brief Return the IRQ load info context pointer associated with a signal
 *
 * \param[in]      p_ldinf      The load info of the partition in which we look
 *                              for the signal.
 * \param[in]      signal       The signal to query for.
 *
 * \retval NULL                 if one of more the following are true:
 *                              - the \ref signal indicates more than one signal
 *                              - the \ref signal does not belong to the
 *                                partition.
 * \retval Any other value      The load info pointer associated with the signal
 */
struct irq_load_info_t *get_irq_info_for_signal(
                                    const struct partition_load_info_t *p_ldinf,
                                    psa_signal_t signal);

/**
 * \brief Entry of Secure interrupt handler. Platforms can call this function to
 *        handle individual interrupts.
 *
 * \param[in] p_pt         The owner Partition of the interrupt to handle
 * \param[in] p_ildi       The irq_load_info_t struct of the interrupt to handle
 *
 * Note:
 *  The input parameters are maintained by platforms and they must be init-ed
 *  in the interrupt init functions.
 */
void spm_handle_interrupt(void *p_pt, struct irq_load_info_t *p_ildi);

#ifdef CONFIG_TFM_PSA_API_THREAD_CALL

/*
 * SPM dispatcher to handle the API call under non-privileged model.
 * This API runs under callers stack, and switch to SPM stack when
 * calling 'p_fn', then switch back to caller stack before returning
 * to the caller.
 *
 * fn_addr      - the target function to be called.
 * frame_addr   - customized ABI frame type for the function call.
 * switch_stack - indicator if need to switch stack.
 */
void spm_interface_thread_dispatcher(uintptr_t fn_addr,
                                     uintptr_t frame_addr,
                                     uint32_t  switch_stack);

/* Execute a customized ABI function in C */
void spcall_execute_c(uintptr_t fn_addr, uintptr_t frame_addr);

#endif

#endif /* __SPM_IPC_H__ */
