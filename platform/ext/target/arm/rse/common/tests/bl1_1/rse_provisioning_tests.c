/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "rse_provisioning_tests.h"

#include <string.h>

#include "test_framework_helpers.h"
#include "tfm_plat_defs.h"
#include "region_defs.h"
#include "rse_provisioning_bundle.h"
#include "rse_kmu_keys.h"
#include "cc3xx_aes.h"
#include "device_definition.h"
#include "tfm_plat_provisioning.h"

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof((arr)[0]))

#define TEST_ASSERT(cond, msg) \
    if (!(cond)) { \
        TEST_FAIL(msg); \
        return; \
    }

#define TEST_SETUP(x) TEST_ASSERT((x) == 0, "Test setup failed")
#define TEST_TEARDOWN(x) TEST_ASSERT((x) == 0, "Test teardown failed")

/* These functions are part of the provisioning code. They are usually static
 * functions, but enabling the provisioning tests makes them non-static. Because
 * they're usually static, they are not included in any header, so in order to
 * have the definitions we need to manually define them here.
 */
enum tfm_plat_err_t provision_assembly_and_test(const volatile struct rse_provisioning_bundle *bundle,
                                                uint8_t *code_output,
                                                size_t code_output_size,
                                                uint8_t *values_output,
                                                size_t values_output_size,
                                                uint8_t *data_output,
                                                size_t data_output_size);

enum tfm_plat_err_t provision_psa_rot(const volatile struct rse_provisioning_bundle *bundle,
                                      uint8_t *code_output,
                                      size_t code_output_size,
                                      uint8_t *values_output,
                                      size_t values_output_size,
                                      uint8_t *data_output,
                                      size_t data_output_size);

enum tfm_plat_err_t validate_and_unpack_encrypted_bundle(
                                              const volatile struct rse_provisioning_bundle *bundle,
                                              enum rse_kmu_slot_id_t key,
                                              uint8_t *code_output,
                                              size_t code_output_size,
                                              uint8_t *values_output,
                                              size_t values_output_size,
                                              uint8_t *data_output,
                                              size_t data_output_size);

enum tfm_plat_err_t setup_provisioning_key(bool is_cm);

enum tfm_plat_err_t set_tp_mode(void);

static struct rse_provisioning_bundle test_bundle;

static struct kmu_key_export_config_t aes_key0_export_config = {
    CC3XX_BASE_S + 0x400, /* CC3XX AES_KEY_0 register */
    0, /* No delay */
    0x01, /* Increment by 4 bytes with each write */
    KMU_DESTINATION_PORT_WIDTH_32_BITS, /* Write 32 bits with each write */
    KMU_DESTINATION_PORT_WIDTH_8_WRITES, /* Perform 8 writes (total 256 bits) */
    true,  /* refresh the masking */
    false, /* Don't disable the masking */
};

static struct kmu_key_export_config_t aes_key1_export_config = {
    CC3XX_BASE_S + 0x420, /* CC3XX AES_KEY_1 register */
    0, /* No delay */
    0x01, /* Increment by 4 bytes with each write */
    KMU_DESTINATION_PORT_WIDTH_32_BITS, /* Write 32 bits with each write */
    KMU_DESTINATION_PORT_WIDTH_8_WRITES, /* Perform 8 writes (total 256 bits) */
    true,  /* refresh the masking */
    false, /* Don't disable the masking */
};

static struct kmu_key_export_config_t aes_128_key0_export_config = {
    CC3XX_BASE_S + 0x400, /* CC3XX AES_KEY_0 register */
    0, /* No delay */
    0x01, /* Increment by 4 bytes with each write */
    KMU_DESTINATION_PORT_WIDTH_32_BITS, /* Write 32 bits with each write */
    KMU_DESTINATION_PORT_WIDTH_4_WRITES, /* Perform 8 writes (total 256 bits) */
    true,  /* refresh the masking */
    false, /* Don't disable the masking */
};

static enum tfm_plat_err_t setup_random_key(enum rse_kmu_slot_id_t key)
{
    enum tfm_plat_err_t plat_err;
    enum kmu_error_t kmu_err;

    plat_err = setup_key_from_rng(key, &aes_key0_export_config, &aes_key1_export_config, true);
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return plat_err;
    }

    kmu_err = kmu_set_key_locked(&KMU_DEV_S, key);
    if (kmu_err != KMU_ERROR_NONE) {
        return (enum tfm_plat_err_t)kmu_err;
    }

    kmu_err = kmu_set_key_locked(&KMU_DEV_S, key + 1);
    if (kmu_err != KMU_ERROR_NONE) {
        return (enum tfm_plat_err_t)kmu_err;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t setup_correct_key(enum rse_kmu_slot_id_t key)
{
    switch (key) {
    case RSE_KMU_SLOT_CM_PROVISIONING_KEY:
        return setup_provisioning_key(true);
    case RSE_KMU_SLOT_DM_PROVISIONING_KEY:
        return setup_provisioning_key(false);
    default:
        return 1;
    }
}

static enum tfm_plat_err_t sign_test_image(enum rse_kmu_slot_id_t key,
                                           struct rse_provisioning_bundle *bundle)
{
    enum cc3xx_error cc_err;

    cc_err = cc3xx_lowlevel_aes_init(CC3XX_AES_DIRECTION_ENCRYPT, CC3XX_AES_MODE_CCM,
                                     (cc3xx_aes_key_id_t)key, NULL, CC3XX_AES_KEYSIZE_256,
                                     (uint32_t *)bundle->iv,
                                     sizeof(bundle->iv));
    if (cc_err != CC3XX_ERR_SUCCESS) {
        return (enum tfm_plat_err_t)cc_err;
    }

    cc3xx_lowlevel_aes_set_tag_len(sizeof(bundle->tag));
    cc3xx_lowlevel_aes_set_data_len(offsetof(struct rse_provisioning_bundle, iv) -
                                    offsetof(struct rse_provisioning_bundle, code),
                                    sizeof(bundle->magic));

    cc3xx_lowlevel_aes_update_authed_data((uint8_t *)&bundle->magic,
                                          sizeof(bundle->magic));

    cc3xx_lowlevel_aes_set_output_buffer((uint8_t *)bundle->code,
                                         PROVISIONING_BUNDLE_CODE_SIZE
                                         + PROVISIONING_BUNDLE_DATA_SIZE
                                         + PROVISIONING_BUNDLE_VALUES_SIZE);

    cc_err = cc3xx_lowlevel_aes_update((uint8_t *)bundle->code,
                                       PROVISIONING_BUNDLE_CODE_SIZE);
    if (cc_err != CC3XX_ERR_SUCCESS) {
        return (enum tfm_plat_err_t)cc_err;
    }

    cc_err = cc3xx_lowlevel_aes_update((uint8_t *)&bundle->values_as_bytes,
                                       PROVISIONING_BUNDLE_VALUES_SIZE);
    if (cc_err != CC3XX_ERR_SUCCESS) {
        return (enum tfm_plat_err_t)cc_err;
    }

    cc_err = cc3xx_lowlevel_aes_update((uint8_t *)&bundle->data,
                                       PROVISIONING_BUNDLE_DATA_SIZE);
    if (cc_err != CC3XX_ERR_SUCCESS) {
        return (enum tfm_plat_err_t)cc_err;
    }

    cc_err = cc3xx_lowlevel_aes_finish((uint32_t *)bundle->tag, NULL);
    if (cc_err != CC3XX_ERR_SUCCESS) {
        return (enum tfm_plat_err_t)cc_err;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

static enum tfm_plat_err_t test_teardown(enum rse_kmu_slot_id_t key,
                                         struct rse_provisioning_bundle *bundle)
{
    enum kmu_error_t kmu_err;

    if (bundle != NULL) {
        memset(bundle, 0, sizeof(struct rse_provisioning_bundle));
    }

    kmu_err = kmu_set_slot_invalid(&KMU_DEV_S, key);
    if (kmu_err != KMU_ERROR_NONE) {
        return (enum tfm_plat_err_t)kmu_err;
    }

    kmu_err = kmu_set_slot_invalid(&KMU_DEV_S, key + 1);
    if (kmu_err != KMU_ERROR_NONE) {
        return (enum tfm_plat_err_t)kmu_err;
    }

    return TFM_PLAT_ERR_SUCCESS;
}

void rse_bl1_provisioning_test_0001(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;

    TEST_SETUP(setup_random_key(RSE_KMU_SLOT_CM_PROVISIONING_KEY));
    TEST_SETUP(sign_test_image(RSE_KMU_SLOT_CM_PROVISIONING_KEY, &test_bundle));

    plat_err = validate_and_unpack_encrypted_bundle(&test_bundle,
                                                    RSE_KMU_SLOT_CM_PROVISIONING_KEY,
                                                    test_bundle.code,
                                                    sizeof(test_bundle.code),
                                                    test_bundle.values_as_bytes,
                                                    sizeof(test_bundle.values_as_bytes),
                                                    test_bundle.data,
                                                    sizeof(test_bundle.data));

    TEST_ASSERT(plat_err == TFM_PLAT_ERR_SUCCESS, "Signature validation failed");

    TEST_TEARDOWN(test_teardown(RSE_KMU_SLOT_CM_PROVISIONING_KEY, &test_bundle));

    ret->val = TEST_PASSED;
    return;
}

void rse_bl1_provisioning_test_0002(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;

    uint32_t *corruption_ptrs[] = {
        (uint32_t *)&test_bundle.magic,
        (uint32_t *)&test_bundle.code,
        (uint32_t *)&test_bundle.values_as_bytes,
        (uint32_t *)&test_bundle.data,
        (uint32_t *)&test_bundle.iv,
        (uint32_t *)&test_bundle.tag,
    };

    for (int idx = 0; idx < ARRAY_SIZE(corruption_ptrs); idx++) {
        TEST_SETUP(setup_random_key(RSE_KMU_SLOT_CM_PROVISIONING_KEY));
        TEST_SETUP(sign_test_image(RSE_KMU_SLOT_CM_PROVISIONING_KEY, &test_bundle));

        *corruption_ptrs[idx] ^= 0xDEADBEEF;

        TEST_LOG(" > testing corruption at offset %d (%d of %d): ",
                 (uintptr_t)corruption_ptrs[idx] - (uintptr_t)&test_bundle,
                 idx + 1, ARRAY_SIZE(corruption_ptrs));

        plat_err = validate_and_unpack_encrypted_bundle(&test_bundle,
                                                    RSE_KMU_SLOT_CM_PROVISIONING_KEY,
                                                    test_bundle.code,
                                                    sizeof(test_bundle.code),
                                                    test_bundle.values_as_bytes,
                                                    sizeof(test_bundle.values_as_bytes),
                                                    test_bundle.data,
                                                    sizeof(test_bundle.data));

        TEST_LOG("\r");

        TEST_ASSERT(plat_err == CC3XX_ERR_INVALID_TAG,
                    "\nSignature validation succeeded when it should have failed");

        TEST_TEARDOWN(test_teardown(RSE_KMU_SLOT_CM_PROVISIONING_KEY, &test_bundle));
    }

    ret->val = TEST_PASSED;
    return;
}

void rse_bl1_provisioning_test_0003(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;
    enum kmu_error_t kmu_err;
    uint32_t *slot_ptr;
    size_t slot_size;

    TEST_SETUP(setup_key_from_rng(26, &aes_128_key0_export_config, &aes_key1_export_config, true));
    TEST_SETUP(kmu_get_key_buffer_ptr(&KMU_DEV_S, 26, &slot_ptr, &slot_size));
    for (int idx = slot_size / sizeof(uint32_t); idx < 8; idx++) {
        slot_ptr[idx] = 0xDEADBEEF;
    }

    TEST_SETUP(setup_key_from_rng(28, &aes_key0_export_config, &aes_key1_export_config, true));
    TEST_SETUP(kmu_set_key_locked(&KMU_DEV_S, 28));

    enum rse_kmu_slot_id_t invalid_keys[] = {
        RSE_KMU_SLOT_DM_PROVISIONING_KEY, /* Not set up */
        _RSE_KMU_AEAD_RESERVED_SLOT_CM_PROVISIONING_KEY, /* No valid AEAD key in next slot */
        (enum rse_kmu_slot_id_t)KMU_HW_SLOT_GUK, /* Can't use HW keys for AEAD */
        26, /* Size mismatch */
        28, /* Not locked */
        30, /* Not set up at all */
    };

    TEST_SETUP(setup_random_key(RSE_KMU_SLOT_CM_PROVISIONING_KEY));
    TEST_SETUP(sign_test_image(RSE_KMU_SLOT_CM_PROVISIONING_KEY, &test_bundle));

    for (int idx = 0; idx < ARRAY_SIZE(invalid_keys); idx++) {
        TEST_LOG(" > testing invalid key %d (%d of %d): ", invalid_keys[idx],
                                                           idx + 1, ARRAY_SIZE(invalid_keys));

        plat_err = validate_and_unpack_encrypted_bundle(&test_bundle,
                                                    invalid_keys[idx],
                                                    test_bundle.code,
                                                    sizeof(test_bundle.code),
                                                    test_bundle.values_as_bytes,
                                                    sizeof(test_bundle.values_as_bytes),
                                                    test_bundle.data,
                                                    sizeof(test_bundle.data));

        TEST_LOG("\r");

        TEST_ASSERT(plat_err == CC3XX_ERR_KEY_IMPORT_FAILED,
                    "\nKey loading succeeded when it should have failed");

        TEST_TEARDOWN(test_teardown(invalid_keys[idx], NULL));
    }

    TEST_TEARDOWN(test_teardown(RSE_KMU_SLOT_CM_PROVISIONING_KEY, &test_bundle));

    ret->val = TEST_PASSED;
    return;
}

void rse_bl1_provisioning_test_0004(struct test_result_t *ret)
{
    bool provisioning_is_required;
    enum lcm_error_t lcm_err;
    enum lcm_lcs_t lcs;

    TEST_SETUP(lcm_get_lcs(&LCM_DEV_S, &lcs));

    switch(lcs) {
        case LCM_LCS_CM:
            provisioning_is_required = true;
            break;
        case LCM_LCS_DM:
            provisioning_is_required = true;
            break;
        case LCM_LCS_SE:
            provisioning_is_required = false;
            break;
        case LCM_LCS_RMA:
            provisioning_is_required = false;
            break;
        case LCM_LCS_INVALID:
            provisioning_is_required = false;
            break;
        default:
            TEST_FAIL("Invalid LCS");
            return;
    }

    TEST_ASSERT(tfm_plat_provisioning_is_required() == provisioning_is_required,
                "Provisioning requirements check failed");

    ret->val = TEST_PASSED;
    return;
}



void rse_bl1_provisioning_test_0101(struct test_result_t *ret)
{
    TEST_ASSERT(set_tp_mode() != TFM_PLAT_ERR_SUCCESS,
                "TP mode set should fail when not in CM");

    ret->val = TEST_PASSED;
    return;
}


void rse_bl1_provisioning_test_0201(struct test_result_t *ret)
{
    TEST_ASSERT(setup_provisioning_key(true) == TFM_PLAT_ERR_SUCCESS,
                "CM Key derivation should work in secure provisioning mode");

    TEST_ASSERT(setup_provisioning_key(false) == TFM_PLAT_ERR_SUCCESS,
                "DM Key derivation should work in secure provisioning mode");

    TEST_TEARDOWN(test_teardown(RSE_KMU_SLOT_CM_PROVISIONING_KEY, NULL));
    TEST_TEARDOWN(test_teardown(RSE_KMU_SLOT_DM_PROVISIONING_KEY, NULL));

    ret->val = TEST_PASSED;
    return;
}

void rse_bl1_provisioning_test_0202(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;

    TEST_ASSERT(setup_provisioning_key(true) != TFM_PLAT_ERR_SUCCESS,
                "CM Key derivation should not be possible outside of secure provisioning mode");

    TEST_ASSERT(setup_provisioning_key(false) != TFM_PLAT_ERR_SUCCESS,
                "DM Key derivation should not be possible outside of secure provisioning mode");

    ret->val = TEST_PASSED;
    return;
}

void rse_bl1_provisioning_test_0301(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;

    plat_err = provision_assembly_and_test(&test_bundle,
                                           test_bundle.code,
                                           sizeof(test_bundle.code),
                                           test_bundle.values_as_bytes,
                                           sizeof(test_bundle.values_as_bytes),
                                           test_bundle.data,
                                           sizeof(test_bundle.data));
    if (plat_err != TFM_PLAT_ERR_SYSTEM_ERR) {
        TEST_FAIL("CM provisioning should not be possible outside of secure provisioning mode");
        return;
    }

    plat_err = provision_psa_rot(&test_bundle,
                                 test_bundle.code,
                                 sizeof(test_bundle.code),
                                 test_bundle.values_as_bytes,
                                 sizeof(test_bundle.values_as_bytes),
                                 test_bundle.data,
                                 sizeof(test_bundle.data));
    if (plat_err != TFM_PLAT_ERR_SYSTEM_ERR) {
        TEST_FAIL("DM provisioning should not be possible outside of secure provisioning mode");
        return;
    }

    ret->val = TEST_PASSED;
    return;
}

void rse_bl1_provisioning_test_0302(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;

    plat_err = provision_assembly_and_test(&test_bundle,
                                           test_bundle.code,
                                           sizeof(test_bundle.code),
                                           test_bundle.values_as_bytes,
                                           sizeof(test_bundle.values_as_bytes),
                                           test_bundle.data,
                                           sizeof(test_bundle.data));
    if (plat_err != CC3XX_ERR_INVALID_TAG) {
        TEST_FAIL("CM provisioning should not be possible with an invalid image");
        return;
    }

    plat_err = provision_psa_rot(&test_bundle,
                                 test_bundle.code,
                                 sizeof(test_bundle.code),
                                 test_bundle.values_as_bytes,
                                 sizeof(test_bundle.values_as_bytes),
                                 test_bundle.data,
                                 sizeof(test_bundle.data));
    if (plat_err != CC3XX_ERR_INVALID_TAG) {
        TEST_FAIL("DM provisioning should not be possible with an invalid image");
        return;
    }

    test_teardown(RSE_KMU_SLOT_CM_PROVISIONING_KEY, NULL);
    test_teardown(RSE_KMU_SLOT_DM_PROVISIONING_KEY, NULL);

    ret->val = TEST_PASSED;
    return;
}

static int __attribute__((naked)) return_true(void) {
    __ASM volatile(
#if !defined(__ICCARM__)
        ".syntax unified                               \n"
#endif
        "movs    r0, #0                                \n"
        "bx lr                                         \n"
    );
}

static int __attribute__((naked)) return_false(void) {
    __ASM volatile(
#if !defined(__ICCARM__)
        ".syntax unified                               \n"
#endif
        "movs    r0, #1                                \n"
        "bx lr                                         \n"
    );
}

void rse_bl1_provisioning_test_0303(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;

    plat_err = provision_assembly_and_test(&test_bundle,
                                           test_bundle.code,
                                           sizeof(test_bundle.code),
                                           test_bundle.values_as_bytes,
                                           sizeof(test_bundle.values_as_bytes),
                                           test_bundle.data,
                                           sizeof(test_bundle.data));
    TEST_ASSERT(plat_err == CC3XX_ERR_INVALID_TAG,
                "CM provisioning should not be possible with an invalid image");

    test_teardown(RSE_KMU_SLOT_CM_PROVISIONING_KEY, &test_bundle);

    plat_err = provision_psa_rot(&test_bundle,
                                 test_bundle.code,
                                 sizeof(test_bundle.code),
                                 test_bundle.values_as_bytes,
                                 sizeof(test_bundle.values_as_bytes),
                                 test_bundle.data,
                                 sizeof(test_bundle.data));
    TEST_ASSERT(plat_err == CC3XX_ERR_INVALID_TAG,
                "DM provisioning should not be possible with an invalid image");

    test_teardown(RSE_KMU_SLOT_DM_PROVISIONING_KEY, &test_bundle);

    ret->val = TEST_PASSED;
    return;
}

void rse_bl1_provisioning_test_0304(struct test_result_t *ret)
{
    enum tfm_plat_err_t plat_err;

    uintptr_t test_payload_pointers[] = {
        (uintptr_t)return_true,
        (uintptr_t)return_false
    };

    int test_payload_return_values[] = {
        0,
        1
    };

    enum rse_kmu_slot_id_t test_keys[] = {
        RSE_KMU_SLOT_CM_PROVISIONING_KEY,
        RSE_KMU_SLOT_DM_PROVISIONING_KEY,
    };

    enum tfm_plat_err_t (*test_funcs[])(const volatile struct rse_provisioning_bundle *,
                                        uint8_t *, size_t, uint8_t *, size_t, uint8_t *, size_t) = {
        provision_assembly_and_test,
        provision_psa_rot
    };

    for (int idx = 0; idx < ARRAY_SIZE(test_payload_pointers); idx++) {
        for (int key_idx = 0; key_idx < ARRAY_SIZE(test_keys); key_idx++) {
            TEST_SETUP(setup_correct_key(test_keys[key_idx]));

            memcpy(test_bundle.code, (void *)(test_payload_pointers[idx] & ~0x1), sizeof(uint32_t));

            TEST_SETUP(sign_test_image(test_keys[key_idx], &test_bundle));
            TEST_SETUP(test_teardown(test_keys[key_idx], NULL));

            plat_err = test_funcs[key_idx](&test_bundle,
                                           test_bundle.code,
                                           sizeof(test_bundle.code),
                                           test_bundle.values_as_bytes,
                                           sizeof(test_bundle.values_as_bytes),
                                           test_bundle.data,
                                           sizeof(test_bundle.data));
            TEST_ASSERT(plat_err == test_payload_return_values[idx],
                        "Provisioning should return with the value the bundle returns");

            TEST_TEARDOWN(test_teardown(test_keys[key_idx], &test_bundle));
        }
    }

    ret->val = TEST_PASSED;
    return;
}
