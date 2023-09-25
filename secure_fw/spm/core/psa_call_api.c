/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 * Copyright (c) 2022-2023 Cypress Semiconductor Corporation (an Infineon
 * company) or an affiliate of Cypress Semiconductor Corporation. All rights
 * reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "config_impl.h"
#include "critical_section.h"
#include "ffm/backend.h"
#include "ffm/psa_api.h"
#include "tfm_hal_isolation.h"
#include "tfm_psa_call_pack.h"
#include "utilities.h"

extern struct service_t *stateless_services_ref_tbl[];


static psa_status_t spm_process_io_vectors(struct connection_t *p_connection,
                                           uint32_t            ctrl_param,
                                           const psa_invec     *inptr,
                                           psa_outvec          *outptr)
{
    psa_invec  ivecs_local[PSA_MAX_IOVEC];
    psa_outvec ovecs_local[PSA_MAX_IOVEC];
    int        i, j;
    fih_int    fih_rc      = FIH_FAILURE;
    uint32_t   ns_access   = 0;
    size_t     ivec_num    = PARAM_UNPACK_IN_LEN(ctrl_param);
    size_t     ovec_num    = PARAM_UNPACK_OUT_LEN(ctrl_param);
    struct partition_t *curr_partition = GET_CURRENT_COMPONENT();

    /* in_len + out_len SHOULD <= PSA_MAX_IOVEC */
    if ((ivec_num > SIZE_MAX - ovec_num) ||
        (ivec_num + ovec_num > PSA_MAX_IOVEC)) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    if (PARAM_IS_NS_VEC(ctrl_param)) {
        ns_access = TFM_HAL_ACCESS_NS;
    }

    /*
     * Read client invecs from the wrap input vector. It is a PROGRAMMER ERROR
     * if the memory reference for the wrap input vector is invalid or not
     * readable.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)inptr,
             ivec_num * sizeof(psa_invec), TFM_HAL_ACCESS_READABLE | ns_access);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    spm_memset(ivecs_local, 0, sizeof(ivecs_local));
    spm_memcpy(ivecs_local, inptr, ivec_num * sizeof(psa_invec));

    /*
     * Read client outvecs from the wrap output vector and will update the
     * actual length later. It is a PROGRAMMER ERROR if the memory reference for
     * the wrap output vector is invalid or not read-write.
     */
    FIH_CALL(tfm_hal_memory_check, fih_rc,
             curr_partition->boundary, (uintptr_t)outptr,
             ovec_num * sizeof(psa_outvec), TFM_HAL_ACCESS_READWRITE | ns_access);
    if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    spm_memset(ovecs_local, 0, sizeof(ovecs_local));
    spm_memcpy(ovecs_local, outptr, ovec_num * sizeof(psa_outvec));

    /*
     * Clients must never overlap input parameters because of the risk of a
     * double-fetch inconsistency.
     * Overflow is checked in tfm_hal_memory_check functions.
     */
    for (i = 0; i + 1 < ivec_num; i++) {
        for (j = i + 1; j < ivec_num; j++) {
            if (!((char *) ivecs_local[j].base + ivecs_local[j].len <=
                  (char *) ivecs_local[i].base ||
                  (char *) ivecs_local[j].base >=
                  (char *) ivecs_local[i].base + ivecs_local[i].len)) {
                return PSA_ERROR_PROGRAMMER_ERROR;
            }
        }
    }

    if (PARAM_IS_NS_INVEC(ctrl_param)) {
        /* Vector descriptor is non-secure then vectors are non-secure. */
        ns_access = TFM_HAL_ACCESS_NS;
    }

    /*
     * For client input vector, it is a PROGRAMMER ERROR if the provided payload
     * memory reference was invalid or not readable.
     */
     for (i = 0; i < ivec_num; i++) {
        FIH_CALL(tfm_hal_memory_check, fih_rc,
                  curr_partition->boundary, (uintptr_t)ivecs_local[i].base,
                  ivecs_local[i].len, TFM_HAL_ACCESS_READABLE | ns_access);
        if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        p_connection->msg.in_size[i]    = ivecs_local[i].len;
        p_connection->invec_base[i]     = ivecs_local[i].base;
        p_connection->invec_accessed[i] = 0;
    }

    if (ns_access == TFM_HAL_ACCESS_NS &&
        !PARAM_IS_NS_VEC(ctrl_param)   &&
        !PARAM_IS_NS_OUTVEC(ctrl_param)) {
        ns_access = 0;
    }

    /*
     * For client output vector, it is a PROGRAMMER ERROR if the provided
     * payload memory reference was invalid or not read-write.
     */
    for (i = 0; i < ovec_num; i++) {
        FIH_CALL(tfm_hal_memory_check, fih_rc,
                    curr_partition->boundary, (uintptr_t)ovecs_local[i].base,
                    ovecs_local[i].len, TFM_HAL_ACCESS_READWRITE | ns_access);
        if (fih_not_eq(fih_rc, fih_int_encode(PSA_SUCCESS))) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        p_connection->msg.out_size[i]   = ovecs_local[i].len;
        p_connection->outvec_base[i]    = ovecs_local[i].base;
        p_connection->outvec_written[i] = 0;
    }

    p_connection->caller_outvec = outptr;

    return PSA_SUCCESS;
}

psa_status_t tfm_spm_client_psa_call(psa_handle_t handle,
                                     uint32_t ctrl_param,
                                     const psa_invec *inptr,
                                     psa_outvec *outptr)
{
    struct connection_t *p_connection;
    struct service_t *service;
    int32_t client_id;
    uint32_t sid, version, index;
    struct critical_section_t cs_assert = CRITICAL_SECTION_STATIC_INIT;
    bool ns_caller = tfm_spm_is_ns_caller();
    int32_t type = PARAM_UNPACK_TYPE(ctrl_param);
    psa_status_t status;

    /* The request type must be zero or positive. */
    if (type < 0) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    /* It is a PROGRAMMER ERROR if the handle is a null handle. */
    if (handle == PSA_NULL_HANDLE) {
        return PSA_ERROR_PROGRAMMER_ERROR;
    }

    client_id = tfm_spm_get_client_id(ns_caller);

    /* Allocate space from handle pool for static handle. */
    if (IS_STATIC_HANDLE(handle)) {
        index = GET_INDEX_FROM_STATIC_HANDLE(handle);

        service = stateless_services_ref_tbl[index];
        if (!service) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        sid = service->p_ldinf->sid;

        /*
         * It is a PROGRAMMER ERROR if the caller is not authorized to access
         * the RoT Service.
         */
        if (tfm_spm_check_authorization(sid, service, ns_caller)
            != PSA_SUCCESS) {
            return PSA_ERROR_CONNECTION_REFUSED;
        }

        version = GET_VERSION_FROM_STATIC_HANDLE(handle);

        if (tfm_spm_check_client_version(service, version) != PSA_SUCCESS) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        CRITICAL_SECTION_ENTER(cs_assert);
        p_connection = spm_allocate_connection();
        CRITICAL_SECTION_LEAVE(cs_assert);

        if (!p_connection) {
            return PSA_ERROR_CONNECTION_BUSY;
        }

        spm_init_connection(p_connection, service, client_id);
    } else {
#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1
        /* It is a PROGRAMMER ERROR if an invalid handle was passed. */
        p_connection = spm_get_client_connection(handle, client_id);
        if (!p_connection) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        /*
         * It is a PROGRAMMER ERROR if the connection is currently
         * handling a request.
         */
        if (p_connection->status != TFM_HANDLE_STATUS_IDLE) {
            return PSA_ERROR_PROGRAMMER_ERROR;
        }

        service = p_connection->service;

        if (!service) {
            /* FixMe: Need to implement a mechanism to resolve this failure. */
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
#else
        return PSA_ERROR_PROGRAMMER_ERROR;
#endif
    }

    if (PARAM_HAS_IOVEC(ctrl_param)) {
        status = spm_process_io_vectors(p_connection, ctrl_param,
                                        inptr, outptr);
        if (status != PSA_SUCCESS) {
            if (IS_STATIC_HANDLE(handle)) {
                spm_free_connection(p_connection);
            }
            return status;
        }
    }

    p_connection->msg.type = type;

    return backend_messaging(service, p_connection);
}
