/*
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <limits.h>

#include "tfm_crypto_defs.h"

#include "psa_crypto.h"
#include "tfm_crypto_api.h"

#include "tfm_crypto_struct.h"
#include "secure_fw/core/tfm_memory_utils.h"

/**
 * \def LIST_OPERATION_LOOKUP
 *
 * \brief This is an X macro which enforces the correspondence
 *        between backend operation type (through the enum
 *        tfm_crypto_operation_type) and the corresponding frontend type
 */
#define LIST_OPERATION_LOOKUP \
    X(TFM_CRYPTO_CIPHER_OPERATION, psa_cipher_operation_t) \
    X(TFM_CRYPTO_MAC_OPERATION, psa_mac_operation_t) \
    X(TFM_CRYPTO_HASH_OPERATION, psa_hash_operation_t)

/**
 * \def CHECK_ALIGNMENT
 *
 * \brief This macro checks the alignment of the operation object pointer which
 *        receives as input based on the requirement of the front end operation
 *        type. This macro expands in a case statement so it must be used in a
 *        switch-case construct. It sets the handle value it receives in input
 *        with the proper value or TFM_CRYPTO_INVALID_HANDLE in case the oper
 *        pointer does not satisfy alignment requirements of the front end type
 */
#define CHECK_ALIGNMENT(e,t,oper,handle)                           \
    case e:                                                        \
        if ((uintptr_t)oper % offsetof(struct {char c; t x;},x)) { \
            handle = TFM_CRYPTO_INVALID_HANDLE;                    \
        } else {                                                   \
            handle = ((t *)oper)->handle;                          \
        }                                                          \
        break;
/**
 * \def GET_HANDLE_POINTER
 *
 * \brief This macro extracts the pointer to handle value from the object
 *        operation pointer it receives as input. This macro expands in a case
 *        statement so it must be used in a switch case-case construct.
 */
#define GET_HANDLE_POINTER(e,t,oper,handle) \
    case e:                                 \
        handle = &(((t *)oper)->handle);    \
        break;
/**
 * \def TFM_CRYPTO_CONC_OPER_NUM
 *
 * \brief This value defines the maximum number of simultaneous operations
 *        supported by this implementation.
 */
#define TFM_CRYPTO_CONC_OPER_NUM (8)

struct tfm_crypto_operation_s {
    uint32_t in_use;                /*!< Indicates if the operation is in use */
    enum tfm_crypto_operation_type type; /*!< Type of the operation */
    union {
        struct tfm_cipher_operation_s cipher;   /*!< Cipher operation context */
        struct tfm_mac_operation_s mac;         /*!< MAC operation context */
        struct tfm_hash_operation_s hash;       /*!< Hash operation context */
    } operation;
};

static struct tfm_crypto_operation_s operation[TFM_CRYPTO_CONC_OPER_NUM] ={{0}};

/*
 * \brief Function used to clear the memory associated to a backend context
 *
 * \param[in] index Numerical index in the database of the backend contexts
 *
 * \return None
 *
 */
static void memset_operation_context(uint32_t index)
{
    uint32_t mem_size;

    uint8_t *mem_ptr = (uint8_t *) &(operation[index].operation);

    switch(operation[index].type) {
    case TFM_CRYPTO_CIPHER_OPERATION:
        mem_size = sizeof(struct tfm_cipher_operation_s);
        break;
    case TFM_CRYPTO_MAC_OPERATION:
        mem_size = sizeof(struct tfm_mac_operation_s);
        break;
    case TFM_CRYPTO_HASH_OPERATION:
        mem_size = sizeof(struct tfm_hash_operation_s);
        break;
    case TFM_CRYPTO_OPERATION_NONE:
    default:
        mem_size = 0;
        break;
    }

    /* Clear the contents of the backend context */
    (void)tfm_memset(mem_ptr, 0, mem_size);
}

/*
 * \brief Function used to extract the handle value from a pointer to a
 *        frontend operation
 *
 * \param[in] type Type of the operation context to extract from
 * \param[in] oper Pointer to the frontend operation
 *
 * \return handle 4-byte identifier associated to the context,
 *                TFM_CRYPTO_INVALID_HANDLE in case of problems
 *
 */
static uint32_t get_handle(enum tfm_crypto_operation_type type, void *oper)
{
    uint32_t handle = TFM_CRYPTO_INVALID_HANDLE;

    /* Dereference the pointer */
    switch(type) {
    /* Generate the list of cases needed to check alignment for all the
     * possible operation types listed in LIST_OPERATION_LOOKUP. The default
     * case and TFM_CRYPTO_OPERATION_NONE must be created explicitly
     */
#define X(e,t) CHECK_ALIGNMENT(e,t,oper,handle)
LIST_OPERATION_LOOKUP
#undef X
    case TFM_CRYPTO_OPERATION_NONE:
    default:
        break;
    }

    return handle;
}

/*
 * \brief Function used to set the handle value in a pointer to a
 *        frontend operation
 *
 * \param[in]  type Type of the operation context to extract from
 * \param[out] oper Pointer to the frontend operation
 *
 * \return handle 4-byte identifier associated to the context,
 *                TFM_CRYPTO_INVALID_HANDLE in case of problems
 *
 */
static uint32_t set_handle(enum tfm_crypto_operation_type type,
                           void *oper,
                           uint32_t set_value)
{
    uint32_t *handle = NULL;

    /* Extract the pointer value */
    switch(type) {
    /* Generate the list of cases needed to get the handle pointer for all the
     * possible operation types listed in LIST_OPERATION_LOOKUP. The default
     * case and TFM_CRYPTO_OPERATION_NONE must be created explicitly
     */
#define X(e,t) GET_HANDLE_POINTER(e,t,oper,handle)
LIST_OPERATION_LOOKUP
#undef X
    case TFM_CRYPTO_OPERATION_NONE:
    default:
        break;
    }

    if (handle == NULL || ((uintptr_t)handle % sizeof(uint32_t))) {
        return TFM_CRYPTO_INVALID_HANDLE;
    }

    /* Set the value by derefencing the pointer, alignment is correct */
    *handle = set_value;

    return set_value;
}

/*!
 * \defgroup public Public functions
 *
 */

/*!@{*/
enum tfm_crypto_err_t tfm_crypto_init_alloc(void)
{
    /* Clear the contents of the local contexts */
    (void)tfm_memset(operation, 0, sizeof(operation));
    return TFM_CRYPTO_ERR_PSA_SUCCESS;
}

enum tfm_crypto_err_t tfm_crypto_operation_alloc(
                                        enum tfm_crypto_operation_type type,
                                        void *oper,
                                        void **ctx)
{
    uint32_t i = 0, handle;

    /* Init to invalid values */
    *ctx = NULL;

    for (i=0; i<TFM_CRYPTO_CONC_OPER_NUM; i++) {
        if (operation[i].in_use == TFM_CRYPTO_NOT_IN_USE) {
            operation[i].in_use = TFM_CRYPTO_IN_USE;
            operation[i].type = type;
            handle = set_handle(type, oper, i);
            if (handle == TFM_CRYPTO_INVALID_HANDLE) {
                return TFM_CRYPTO_ERR_PSA_ERROR_NOT_PERMITTED;
            }
            *ctx = (void *) &(operation[i].operation);
            return TFM_CRYPTO_ERR_PSA_SUCCESS;
        }
    }

    return TFM_CRYPTO_ERR_PSA_ERROR_NOT_PERMITTED;
}

enum tfm_crypto_err_t tfm_crypto_operation_release(
                                        enum tfm_crypto_operation_type type,
                                        void *oper)
{
    uint32_t handle = get_handle(type, oper);

    if ( (handle != TFM_CRYPTO_INVALID_HANDLE) &&
         (handle < TFM_CRYPTO_CONC_OPER_NUM) &&
         (operation[handle].in_use == TFM_CRYPTO_IN_USE) ) {
        memset_operation_context(handle);
        operation[handle].in_use = TFM_CRYPTO_NOT_IN_USE;
        operation[handle].type = TFM_CRYPTO_OPERATION_NONE;
        (void)set_handle(type, oper, TFM_CRYPTO_INVALID_HANDLE);
        return TFM_CRYPTO_ERR_PSA_SUCCESS;
    }

    return TFM_CRYPTO_ERR_PSA_ERROR_INVALID_ARGUMENT;
}

enum tfm_crypto_err_t tfm_crypto_operation_lookup(
                                        enum tfm_crypto_operation_type type,
                                        void *oper,
                                        void **ctx)
{
    uint32_t handle = get_handle(type, oper);

    if ( (handle != TFM_CRYPTO_INVALID_HANDLE) &&
         (handle < TFM_CRYPTO_CONC_OPER_NUM) &&
         (operation[handle].in_use == TFM_CRYPTO_IN_USE) &&
         (operation[handle].type == type) ) {
        *ctx = (void *) &(operation[handle].operation);
        return TFM_CRYPTO_ERR_PSA_SUCCESS;
    }

    return TFM_CRYPTO_ERR_PSA_ERROR_BAD_STATE;
}
/*!@}*/
