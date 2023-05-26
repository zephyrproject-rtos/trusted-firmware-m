/*
 * Copyright (c) 2023 Cypress Semiconductor Corporation (an Infineon company)
 * or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __AGENT_API_H__
#define __AGENT_API_H__

#include <stdint.h>

#include "config_impl.h"
#include "psa/client.h"


struct client_vectors {
    const psa_invec *in_vec;
    psa_outvec      *out_vec;
};


struct client_params {
    int32_t     ns_client_id;
    const void *client_data;
};


/**
 * \brief psa_call() interface for NS agents
 *
 * \param[in] handle            Service handle to the established connection,
 *                              \ref psa_handle_t
 * \param[in] ctrl_param        Parameters combined in uint32_t,
 *                              includes request type, in_num and out_num.
 * \param[in] vecs              Combines the psa_invec and psa_outvec params
 *                              for the psa_call() to be made.
 * \param[in] params            NS agent's client identifier.
 *                              Ignored for connection-based services.
 *                              ns_client_id identifies the NS client.
 *                              Zero or positive values are ignored and SPM
 *                              uses the agent's ID instead.
 *                              client_data is treated as opaque by SPM.
 *
 * \retval PSA_SUCCESS          Success.
 * \retval "Does not return"    The call is invalid, one or more of the
 *                              following are true:
 * \arg                           An invalid handle was passed.
 * \arg                           The connection is already handling a request.
 * \arg                           An invalid memory reference was provided.
 * \arg                           in_num + out_num > PSA_MAX_IOVEC.
 * \arg                           The message is unrecognized by the RoT
 *                                Service or incorrectly formatted.
 */
psa_status_t agent_psa_call(psa_handle_t handle, int32_t ctrl_param,
                            const struct client_vectors *vecs,
                            const struct client_params *params);

#if CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1

/**
 * \brief psa_connect() interface for NS agents
 *
 * \param[in] sid                       RoT Service identity.
 * \param[in] version                   The version of the RoT Service.
 * \param[in] ns_client_id              Which NS client is being represented.
 *                                      Negative value identifies the NS client.
 *                                      Zero or positive values are ignored
 *                                      and SPM uses the agent's ID instead.
 * \param[in] client_data               Caller identifier, treated as opaque
 *                                      by SPM.
 *
 * \retval PSA_SUCCESS                  Success.
 * \retval PSA_ERROR_CONNECTION_REFUSED The SPM or RoT Service has refused the
 *                                      connection.
 * \retval PSA_ERROR_CONNECTION_BUSY    The SPM or RoT Service cannot make the
 *                                      connection at the moment.
 * \retval "Does not return"            The RoT Service ID and version are not
 *                                      supported, or the caller is not
 *                                      permitted to access the service.
 */
psa_handle_t agent_psa_connect(uint32_t sid, uint32_t version,
                               int32_t ns_client_id, const void *client_data);

#else
#define agent_psa_connect    NULL
#endif /* CONFIG_TFM_CONNECTION_BASED_SERVICE_API == 1 */

#endif /* __AGENT_API_H__ */
