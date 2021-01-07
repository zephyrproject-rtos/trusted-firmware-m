/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "tfm_fwu_req_mngr.h"
#include "tfm_fwu.h"
#include "psa/update.h"
#include "service_api.h"
#include "tfm_api.h"

#ifdef TFM_PSA_API
#include "psa/service.h"
#include "psa_manifest/tfm_firmware_update.h"
#endif

typedef struct tfm_fwu_ctx_s {
    psa_image_id_t image_id;
    uint8_t image_state;
    bool in_use;
} tfm_fwu_ctx_t;

/**
 * \brief The context of FWU service.
 */
static tfm_fwu_ctx_t fwu_ctx[TFM_FWU_MAX_IMAGES];

/**
 * \brief Check if the image is in FWU process, return the index if it is.
 */
static bool get_image_index(psa_image_id_t image_id, uint8_t *index)
{
    if (!index) {
        return false;
    }

    for (uint8_t i = 0; i < TFM_FWU_MAX_IMAGES; i++) {
        if (fwu_ctx[i].in_use && (fwu_ctx[i].image_id == image_id)) {
            *index = i;
            return true;
        }
    }
    return false;
}

/**
 * \brief The the next free index in fwu_ctx.
 */
static bool get_next_free_index(uint8_t *index)
{
    for (uint8_t i = 0; i < TFM_FWU_MAX_IMAGES; i++) {
        if (!fwu_ctx[i].in_use) {
            *index = i;
            return true;
        }
    }
    return false;
}

#ifndef TFM_PSA_API
psa_status_t tfm_fwu_write_req(psa_invec *in_vec, size_t in_len,
                               psa_outvec *out_vec, size_t out_len)
{
    psa_image_id_t image_id;
    size_t block_offset;
    uint8_t *p_data;
    size_t data_length;
    psa_status_t status = PSA_SUCCESS;
    uint8_t image_index;

    (void)out_vec;

    /* Check input parameters. */
    if (in_vec[0].len != sizeof(image_id) ||
        in_vec[1].len != sizeof(block_offset)) {
        /* The size of one of the arguments is incorrect */
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    p_data = (uint8_t * const)in_vec[2].base;

    if (p_data == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    block_offset = *((size_t *)in_vec[1].base);
    data_length = in_vec[2].len;
    image_id = *((psa_image_id_t *)in_vec[0].base);

    if (get_image_index(image_id, &image_index)) {
        /* The image is in FWU process. */
        if ((fwu_ctx[image_index].image_state != PSA_IMAGE_CANDIDATE) &&
            (fwu_ctx[image_index].image_state != PSA_IMAGE_REJECTED)) {
            return PSA_ERROR_CURRENTLY_INSTALLING;
        }
    } else {
        /* The image is not in FWU process. */
        if (get_next_free_index(&image_index)) {
            /* Get a free index. Start the FWU process of this image. */
            status = tfm_internal_fwu_initialize(image_id);
            if (status != PSA_SUCCESS) {
                return status;
            }
            fwu_ctx[image_index].in_use = true;
            fwu_ctx[image_index].image_id = image_id;
            fwu_ctx[image_index].image_state = PSA_IMAGE_CANDIDATE;
        } else {
            /* No more resource can be used. */
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }
    }

    return tfm_internal_fwu_write(image_id,
                                  block_offset,
                                  p_data,
                                  data_length);
}

psa_status_t tfm_fwu_install_req(psa_invec *in_vec, size_t in_len,
                                 psa_outvec *out_vec, size_t out_len)
{
    psa_image_id_t image_id;
    psa_image_id_t *dependency_id;
    psa_image_version_t *dependency_version;
    psa_status_t status;
    uint8_t image_index;

    /* Check input/output parameters. */
    if (in_vec[0].len != sizeof(image_id) ||
        out_vec[0].len != sizeof(*dependency_id) ||
        out_vec[1].len != sizeof(*dependency_version)) {
        /* The size of one of the arguments is incorrect. */
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    image_id = *((psa_image_id_t *)in_vec[0].base);
    dependency_id = out_vec[0].base;
    dependency_version = out_vec[1].base;

    if ((!get_image_index(image_id, &image_index)) ||
       (fwu_ctx[image_index].image_state != PSA_IMAGE_CANDIDATE)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    } else {
        status = tfm_internal_fwu_install(image_id,
                                          dependency_id,
                                          dependency_version);
        if (status == PSA_SUCCESS) {
            fwu_ctx[image_index].image_state = PSA_IMAGE_INSTALLED;

            /* The image has been successfully installed, free the index. */
            fwu_ctx[image_index].in_use = false;
        } else if (status == PSA_SUCCESS_REBOOT) {
            fwu_ctx[image_index].image_state = PSA_IMAGE_REBOOT_NEEDED;
        } else {
            fwu_ctx[image_index].image_state = PSA_IMAGE_REJECTED;
        }

        return status;
    }
}

psa_status_t tfm_fwu_query_req(psa_invec *in_vec, size_t in_len,
                               psa_outvec *out_vec, size_t out_len)
{
    psa_image_id_t image_id;
    psa_image_info_t *info_p;
    psa_status_t result = 0;
    uint8_t image_index;

    if ((in_vec[0].len != sizeof(image_id)) ||
       (out_vec[0].len != sizeof(*info_p))) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    image_id = *((psa_image_id_t *)in_vec[0].base);
    info_p = (psa_image_info_t *)out_vec[0].base;
    result = tfm_internal_fwu_query(image_id, info_p);
    if (result == PSA_SUCCESS) {
        /* Fill the image state if the query image is not the running image. */
        if (info_p->state == PSA_IMAGE_UNDEFINED) {
            if (get_image_index(image_id, &image_index)) {
                /* The queried image is the currently updating image. */
                info_p->state = fwu_ctx[image_index].image_state;
            }
        }
    }

    return result;
}

psa_status_t tfm_fwu_request_reboot_req(psa_invec *in_vec, size_t in_len,
                               psa_outvec *out_vec, size_t out_len)
{
    (void)in_vec;
    (void)out_vec;
    (void)in_len;
    (void)out_len;

    tfm_internal_fwu_request_reboot();

    /* If it goes here, then the reboot fails. */
    return PSA_ERROR_SERVICE_FAILURE;
}

psa_status_t tfm_fwu_accept_req(psa_invec *in_vec, size_t in_len,
                               psa_outvec *out_vec, size_t out_len)
{
    (void)in_vec;
    (void)out_vec;
    (void)in_len;
    (void)out_len;

    /* This operation set the running image to INSTALLED state, the images
     * in the staging area does not impact this operation.
     */
    return tfm_internal_fwu_accept();
}

/* Abort the currently running FWU. */
psa_status_t tfm_fwu_abort_req(psa_invec *in_vec, size_t in_len,
                               psa_outvec *out_vec, size_t out_len)
{
    psa_image_id_t image_id;
    psa_status_t status;
    uint8_t image_index;

    (void)out_vec;

    if (in_vec[0].len != sizeof(image_id)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    image_id = *((psa_image_id_t *)in_vec[0].base);
    if (get_image_index(image_id, &image_index)) {
        /* The image is in FWU process. */
        if ((fwu_ctx[image_index].image_state == PSA_IMAGE_CANDIDATE) ||
           (fwu_ctx[image_index].image_state == PSA_IMAGE_REBOOT_NEEDED) ||
           (fwu_ctx[image_index].image_state == PSA_IMAGE_REJECTED)) {
            status = tfm_internal_fwu_abort(image_id);
            if (status != PSA_SUCCESS) {
                return status;
            }

            fwu_ctx[image_index].image_state = PSA_IMAGE_UNDEFINED;
            fwu_ctx[image_index].in_use = false;
            fwu_ctx[image_index].image_id = TFM_FWU_INVALID_IMAGE_ID;
            return PSA_SUCCESS;
        } else {
            /* If the image is in INSTALLED state or UNDEFINED, it should not in
             * a FWU process.
             */
            return PSA_ERROR_PROGRAMMER_ERROR;
        }
    } else {
        /* No image with the provided image_id is not in FWU process. */
        return PSA_ERROR_INVALID_ARGUMENT;
    }
}
#endif

psa_status_t tfm_fwu_init(void)
{
    return fwu_bootloader_init();
}
