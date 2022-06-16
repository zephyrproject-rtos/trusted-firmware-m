/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "tfm_platform_api.h"
#include "tfm_ns_interface.h"
#include "tfm_veneers.h"

enum tfm_platform_err_t tfm_platform_system_reset(void)
{
    return (enum tfm_platform_err_t) tfm_ns_interface_dispatch(
                                (veneer_fn)tfm_platform_sp_system_reset_veneer,
                                0,
                                0,
                                0,
                                0);
}

enum tfm_platform_err_t
tfm_platform_ioctl(tfm_platform_ioctl_req_t request,
                   psa_invec *input, psa_outvec *output)
{
    tfm_platform_ioctl_req_t req = request;
    struct psa_invec in_vec[2];
    size_t inlen, outlen;

    in_vec[0].base = &req;
    in_vec[0].len = sizeof(req);
    if (input != NULL) {
        in_vec[1].base = input->base;
        in_vec[1].len = input->len;
        inlen = 2;
    } else {
        inlen = 1;
    }

    if (output != NULL) {
        outlen = 1;
    } else {
        outlen = 0;
    }

    return (enum tfm_platform_err_t) tfm_ns_interface_dispatch(
                                (veneer_fn)tfm_platform_sp_ioctl_veneer,
                                (uint32_t)in_vec, (uint32_t)inlen,
                                (uint32_t)output, (uint32_t)outlen);
}

enum tfm_platform_err_t
tfm_platform_nv_counter_increment(uint32_t counter_id)
{
    struct psa_invec in_vec[1];

    in_vec[0].base = &counter_id;
    in_vec[0].len = sizeof(counter_id);

    return (enum tfm_platform_err_t) tfm_ns_interface_dispatch(
            (veneer_fn)tfm_platform_sp_nv_counter_increment_veneer,
            (uint32_t)in_vec, 1,
            (uint32_t)NULL, 0);
}

enum tfm_platform_err_t
tfm_platform_nv_counter_read(uint32_t counter_id,
                             uint32_t size, uint8_t *val)
{
    struct psa_invec in_vec[1];
    struct psa_outvec out_vec[1];

    in_vec[0].base = &counter_id;
    in_vec[0].len = sizeof(counter_id);

    out_vec[0].base = val;
    out_vec[0].len = size;

    return (enum tfm_platform_err_t) tfm_ns_interface_dispatch(
            (veneer_fn)tfm_platform_sp_nv_counter_read_veneer,
            (uint32_t)in_vec, 1,
            (uint32_t)out_vec, 1);
}
