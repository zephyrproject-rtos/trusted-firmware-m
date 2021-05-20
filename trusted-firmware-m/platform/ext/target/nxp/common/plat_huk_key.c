/*
 * Copyright (c) 2017-2020 Arm Limited. All rights reserved.
 * Copyright 2020 NXP. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#include "platform/include/tfm_plat_crypto_keys.h"
#include "tfm_plat_defs.h"
#include <stddef.h>

#include "tfm_mbedcrypto_include.h" /* Use Mbed-Crypto PSA */

#include "log/tfm_log.h"

#ifndef TFM_HUK_KEY_DERIVATION
    #define TFM_HUK_KEY_DERIVATION 1    /* Enable Key derivation from HUK */
#endif


/* Functions in this file should be implemented by platform vendor. For
 * the security of the storage system, it is critical to use a hardware unique
 * key. For the security of the attestation, it is critical to use a unique key
 * pair and keep the private key is secret.
 */
#ifndef TFM_HUK_KEY_ADDR
static const uint8_t sample_tfm_key[] =
             {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, \
              0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};
             
#define TFM_HUK_KEY_ADDR sample_tfm_key
#endif
#ifndef TFM_HUK_KEY_LEN              
#define TFM_HUK_KEY_LEN   (16)         
#endif

#if !TFM_HUK_KEY_DERIVATION               
/**
 * \brief Copy the key to the destination buffer
 *
 * \param[out]  p_dst  Pointer to buffer where to store the key
 * \param[in]   p_src  Pointer to the key
 * \param[in]   size   Length of the key
 */
static inline void copy_key(uint8_t *p_dst, const uint8_t *p_src, size_t size)
{
    uint32_t i;

    for (i = size; i > 0; i--) {
        *p_dst = *p_src;
        p_src++;
        p_dst++;
    }
}
#endif

enum tfm_plat_err_t tfm_plat_get_huk_derived_key(const uint8_t *label,
                                                 size_t label_size,
                                                 const uint8_t *context,
                                                 size_t context_size,
                                                 uint8_t *key,
                                                 size_t key_size)
{
    (void)label;
    (void)label_size;
    (void)context;
    (void)context_size;

#if TFM_HUK_KEY_DERIVATION  /* Create the storage key from the key derivation operation */ 
    {
        psa_status_t                    status;
        psa_key_handle_t                base_key;
        psa_key_derivation_operation_t  op = PSA_KEY_DERIVATION_OPERATION_INIT;
        psa_key_attributes_t            attributes = PSA_KEY_ATTRIBUTES_INIT;
        psa_algorithm_t                 alg = PSA_ALG_HKDF(PSA_ALG_SHA_256); /* HMAC-based Extract-and-Expand Key Derivation Function (HKDF) using SHA-256 [RFC5869] */
        
        /* Import HUK key for use in key derivation. */
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&attributes, alg);
        psa_set_key_type(&attributes, PSA_KEY_TYPE_DERIVE);
        status = psa_import_key(&attributes, (const uint8_t *)TFM_HUK_KEY_ADDR, TFM_HUK_KEY_LEN, &base_key);
        if (status != PSA_SUCCESS) {
            LOG_MSG("Failed to import a key\n");
            return TFM_PLAT_ERR_SYSTEM_ERR;
        }
        
        /* Set up a key derivation operation */
        status = psa_key_derivation_setup(&op, alg);
        if (status != PSA_SUCCESS) {
            LOG_MSG("Failed to Set up a key derivation operation\n");
            goto err_release_key;
        }
        
        status = psa_key_derivation_set_capacity(&op, key_size);
        if (status != PSA_SUCCESS) {
            LOG_MSG("Failed to set capacity\n");
            goto err_release_op;
        }
        
        /* Initial keying material */
        status = psa_key_derivation_input_key(&op,
                                              PSA_KEY_DERIVATION_INPUT_SECRET,
                                              base_key);
        if (status != PSA_SUCCESS) {
            LOG_MSG("Failed to input key\n");
            goto err_release_op;
        }
        
        /* Supply the key label as an input to the key derivation.
        Application- and context-specific information */
        status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_INFO,
                                                label,
                                                label_size);
        if (status != PSA_SUCCESS) {
            LOG_MSG("Failed to psa_key_derivation_input_bytes()\n");
            goto err_release_op;
        }

        /* Create the storage key from the key derivation operation */
        status = psa_key_derivation_output_bytes(&op, key, key_size);
        if (status != PSA_SUCCESS) {
            LOG_MSG("Failed to psa_key_derivation_output_bytes()\n");
            goto err_release_op;
        }

        /* Free resources associated with the key derivation operation */
        (void)psa_close_key(base_key);
        (void)psa_key_derivation_abort(&op);
    
        return TFM_PLAT_ERR_SUCCESS;

 err_release_op:
        (void)psa_key_derivation_abort(&op);
 err_release_key:
        (void)psa_close_key(base_key);
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
        
#else /* Original code does not do any darivation */
    if (key_size > TFM_HUK_KEY_LEN) {
        return TFM_PLAT_ERR_SYSTEM_ERR;
    }
    
    /* FIXME: Do key derivation */
    copy_key(key, TFM_HUK_KEY_ADDR, key_size);
    
    return TFM_PLAT_ERR_SUCCESS;
#endif    
    
}
