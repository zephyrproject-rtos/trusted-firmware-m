/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "kmu_drv.h"

#include "unity.h"

/*
 * RSE build KMU configuration values:
 *  KMUNKS          = 0x5 -> 32 key slots
 *  KMUTANG         = 0x5 -> Top nibble of 1st target crypto devices
 *                           addresses group is 0x0005
 *  KMUNHWKSLTS     = 0x7 -> 7 hardware key slots
 */
#define KMU_BC_RESET_VALUE 0x3D0005u /* Build configuration register */
#define KMU_IS_RESET_VALUE 0x0u      /* Interrupt status register */
#define KMU_IE_RESET_VALUE 0x0u      /* Interrupt enable register */
#define KMU_IC_RESET_VALUE 0x0u      /* Interrupt clear register */
#define KMU_PRBGSI_RESET_VALUE                                                 \
    0x0u /* Pseudorandom bit generator seed input register */
#define KMU_KSC_RESET_VALUE 0x0u          /* Key slot config register */
#define KMU_KSC_0_RESET_VALUE 0xD60100u   /* Key slot 0 config register */
#define KMU_KSC_1_RESET_VALUE 0xD60100u   /* Key slot 1 config register */
#define KMU_KSC_2_RESET_VALUE 0xD60100u   /* Key slot 2 config register */
#define KMU_KSC_3_RESET_VALUE 0xD60100u   /* Key slot 3 config register */
#define KMU_KSC_4_RESET_VALUE 0xD60100u   /* Key slot 4 config register */
#define KMU_KSC_5_RESET_VALUE 0xD60100u   /* Key slot 5 config register */
#define KMU_KSC_6_RESET_VALUE 0xD60100u   /* Key slot 6 config register */
#define KMU_DKPA_RESET_VALUE 0x0u         /* Dest key port addr register */
#define KMU_DKPA_0_RESET_VALUE 0x50154400 /* Dest key 0 port addr register */
#define KMU_DKPA_1_RESET_VALUE 0x50154400 /* Dest key 1 port addr register */
#define KMU_DKPA_2_RESET_VALUE 0x50154400 /* Dest key 2 port addr register */
#define KMU_DKPA_3_RESET_VALUE 0x50154400 /* Dest key 3 port addr register */
#define KMU_DKPA_4_RESET_VALUE 0x50154400 /* Dest key 4 port addr register */
#define KMU_DKPA_5_RESET_VALUE 0x50154400 /* Dest key 5 port addr register */
#define KMU_DKPA_6_RESET_VALUE 0x50154400 /* Dest key 6 port addr register */
#define KMU_KSK_RESET_VALUE 0x0u          /* Key slot key registers */
#define KMU_RD_8_RESET_VALUE 0x0u         /* Random delay 8 clocks register */
#define KMU_RD_16_RESET_VALUE 0x0u        /* Random delay 16 clocks register */
#define KMU_RD_32_RESET_VALUE 0x0u        /* Random delay 32 clocks register */
#define KMU_PIDR0_RESET_VALUE 0xF3u       /* Peripheral ID 0 register */
#define KMU_PIDR1_RESET_VALUE 0xB0u       /* Peripheral ID 1 register */
#define KMU_PIDR2_RESET_VALUE 0x1Bu       /* Peripheral ID 2 register */
#define KMU_PIDR3_RESET_VALUE 0x0u        /* Peripheral ID 3 register */
#define KMU_PIDR4_RESET_VALUE 0x4u        /* Peripheral ID 4 register */
#define KMU_CIDR0_RESET_VALUE 0xDu        /* Component ID 0 register */
#define KMU_CIDR1_RESET_VALUE 0xF0u       /* Component ID 1 register */
#define KMU_CIDR2_RESET_VALUE 0x5u        /* Component ID 2 register */
#define KMU_CIDR3_RESET_VALUE 0xB1u       /* Component ID 3 register */

/* Arm Key Management Unit Speciﬁcation (107715_0000_02_en) */
struct _kmu_reg_map_t {
    volatile uint32_t kmubc;
    /*!< Offset: 0x000 (R/ ) KMU Build Configuration Register */
    volatile uint32_t kmuis;
    /*!< Offset: 0x004 (R/ ) KMU Interrupt Status Register */
    volatile uint32_t kmuie;
    /*!< Offset: 0x008 (R/W) KMU Interrupt Enable Register */
    volatile uint32_t kmuic;
    /*!< Offset: 0x00C (R/W) KMU Interrupt Clear Register */
    volatile uint32_t kmuprbgsi;
    /*!< Offset: 0x010 (R/W) PRBG Seed Input Register */
    volatile uint32_t reserved_0[7];
    /*!< Offset: 0x14-0x30 Reserved */
    volatile uint32_t kmuksc[32];
    /*!< Offset: 0x030 (R/W) KMU Key Slot Configuration Register */
    volatile uint32_t kmudkpa[32];
    /*!< Offset: 0x0B0 (R/W) KMU Destination Key Port Address Register */
    volatile uint32_t kmuksk[32][8];
    /*!< Offset: 0x130 (R/W) KMU Key Slot Register */
    volatile uint32_t kmurd_8;
    /*!< Offset: 0x530 (R/ ) KMU 8-cycle-limit random delay Register */
    volatile uint32_t kmurd_16;
    /*!< Offset: 0x534 (R/ ) KMU 16-cycle-limit random delay Register */
    volatile uint32_t kmurd_32;
    /*!< Offset: 0x538 (R/ ) KMU 32-cycle-limit random delay Register */
    volatile uint32_t reserved_1[668];
    /*!< Offset: 0x53C-0xFCC Reserved */
    volatile uint32_t pidr4;
    /*!< Offset: 0xFD0 (R/ ) Peripheral ID 4 */
    volatile uint32_t reserved_2[3];
    /*!< Offset: 0xFD4-0xFDC Reserved */
    volatile uint32_t pidr0;
    /*!< Offset: 0xFE0 (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;
    /*!< Offset: 0xFE4 (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;
    /*!< Offset: 0xFE8 (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;
    /*!< Offset: 0xFEC (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;
    /*!< Offset: 0xFF0 (R/ ) Component ID 0 */
    volatile uint32_t cidr1;
    /*!< Offset: 0xFF4 (R/ ) Component ID 1 */
    volatile uint32_t cidr2;
    /*!< Offset: 0xFF8 (R/ ) Component ID 2 */
    volatile uint32_t cidr3;
    /*!< Offset: 0xFFC (R/ ) Component ID 3 */
} regmap;

struct kmu_thread_args_t {
    uint32_t slot;
    uint32_t kmuksc_mask;
    uint32_t kmuis_mask;
};

static struct kmu_dev_cfg_t KMU_DEV_CFG_S = {.base = (uintptr_t)&regmap};
struct kmu_dev_t KMU_DEV_S = {.cfg = &(KMU_DEV_CFG_S)};

/* Set KMU Registers to their default reset values */
void set_default_regmap_values(void)
{
    regmap.kmubc = KMU_BC_RESET_VALUE;
    regmap.kmuis = KMU_IS_RESET_VALUE;
    regmap.kmuie = KMU_IE_RESET_VALUE;
    regmap.kmuic = KMU_IC_RESET_VALUE;
    regmap.kmuprbgsi = KMU_PRBGSI_RESET_VALUE;
    memset((void *)regmap.reserved_0, 0x0u, sizeof(regmap.reserved_0));
    memset((void *)regmap.kmuksc, KMU_KSC_RESET_VALUE, sizeof(regmap.kmuksc));
    regmap.kmuksc[0] = KMU_KSC_0_RESET_VALUE;
    regmap.kmuksc[1] = KMU_KSC_1_RESET_VALUE;
    regmap.kmuksc[2] = KMU_KSC_2_RESET_VALUE;
    regmap.kmuksc[3] = KMU_KSC_3_RESET_VALUE;
    regmap.kmuksc[4] = KMU_KSC_4_RESET_VALUE;
    regmap.kmuksc[5] = KMU_KSC_5_RESET_VALUE;
    regmap.kmuksc[6] = KMU_KSC_6_RESET_VALUE;
    memset((void *)regmap.kmudkpa, KMU_DKPA_RESET_VALUE,
           sizeof(regmap.kmudkpa));
    regmap.kmudkpa[0] = KMU_DKPA_0_RESET_VALUE;
    regmap.kmudkpa[1] = KMU_DKPA_1_RESET_VALUE;
    regmap.kmudkpa[2] = KMU_DKPA_2_RESET_VALUE;
    regmap.kmudkpa[3] = KMU_DKPA_3_RESET_VALUE;
    regmap.kmudkpa[4] = KMU_DKPA_4_RESET_VALUE;
    regmap.kmudkpa[5] = KMU_DKPA_5_RESET_VALUE;
    regmap.kmudkpa[6] = KMU_DKPA_6_RESET_VALUE;
    memset((void *)regmap.kmuksk, KMU_KSK_RESET_VALUE, sizeof(regmap.kmuksk));
    regmap.kmurd_8 = KMU_RD_8_RESET_VALUE;
    regmap.kmurd_16 = KMU_RD_16_RESET_VALUE;
    regmap.kmurd_32 = KMU_RD_32_RESET_VALUE;
    memset((void *)regmap.reserved_1, 0x0u, sizeof(regmap.reserved_1));
    regmap.pidr4 = KMU_PIDR4_RESET_VALUE;
    memset((void *)regmap.reserved_2, 0x0u, sizeof(regmap.reserved_2));
    regmap.pidr0 = KMU_PIDR0_RESET_VALUE;
    regmap.pidr1 = KMU_PIDR1_RESET_VALUE;
    regmap.pidr2 = KMU_PIDR2_RESET_VALUE;
    regmap.pidr3 = KMU_PIDR3_RESET_VALUE;
    regmap.cidr0 = KMU_CIDR0_RESET_VALUE;
    regmap.cidr1 = KMU_CIDR1_RESET_VALUE;
    regmap.cidr2 = KMU_CIDR2_RESET_VALUE;
    regmap.cidr3 = KMU_CIDR3_RESET_VALUE;
}

/* Verify that Read-Only registers have not been modified */
void verify_read_only_registers(void)
{
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_BC_RESET_VALUE, regmap.kmubc,
                                     "Read-Only register KMUBC was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_IS_RESET_VALUE, regmap.kmuis,
                                     "Read-Only register KMUIS was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_RD_8_RESET_VALUE, regmap.kmurd_8,
                                     "Read-Only register KMURD_8 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(
        KMU_RD_16_RESET_VALUE, regmap.kmurd_16,
        "Read-Only register KMURD_16 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(
        KMU_RD_32_RESET_VALUE, regmap.kmurd_32,
        "Read-Only register KMURD_32 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_PIDR0_RESET_VALUE, regmap.pidr0,
                                     "Read-Only register PIDR0 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_PIDR1_RESET_VALUE, regmap.pidr1,
                                     "Read-Only register PIDR1 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_PIDR2_RESET_VALUE, regmap.pidr2,
                                     "Read-Only register PIDR2 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_PIDR3_RESET_VALUE, regmap.pidr3,
                                     "Read-Only register PIDR3 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_PIDR4_RESET_VALUE, regmap.pidr4,
                                     "Read-Only register PIDR4 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_CIDR0_RESET_VALUE, regmap.cidr0,
                                     "Read-Only register CIDR0 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_CIDR1_RESET_VALUE, regmap.cidr1,
                                     "Read-Only register CIDR1 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_CIDR2_RESET_VALUE, regmap.cidr2,
                                     "Read-Only register CIDR2 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(KMU_CIDR3_RESET_VALUE, regmap.cidr3,
                                     "Read-Only register CIDR3 was modified");
}

void setUp(void) { set_default_regmap_values(); }

void tearDown(void) { verify_read_only_registers(); }

void test_kmu_init_prbg_seed_invalid_alignment(void)
{
    enum kmu_error_t kmu_err;
    uint8_t prbg_seed[KMU_PRBG_SEED_LEN + sizeof(uint8_t)];

    /* Act */
    kmu_err = kmu_init(&KMU_DEV_S, prbg_seed + sizeof(uint8_t));

    /* Assert */
    TEST_ASSERT_EQUAL(KMU_ERROR_INIT_INVALID_ALIGNMENT, kmu_err);
}

void test_kmu_init_ok(void)
{
    enum kmu_error_t kmu_err;
    uint8_t prbg_seed[KMU_PRBG_SEED_LEN];
    uint32_t expected_kmuprbgsi;
    uint32_t expected_kmuksc;
    uint32_t expected_kmuie;

    /* Prepare */
    expected_kmuprbgsi = 0xAAAAAAAAu;
    expected_kmuksc = regmap.kmuksc[0] | KMU_KMUKSC_L_KMUPRBGSI_MASK;
    expected_kmuie = 0xFFFFFFFFu;

    memset(prbg_seed, 0xAAu, KMU_PRBG_SEED_LEN);

    /* Act */
    kmu_err = kmu_init(&KMU_DEV_S, prbg_seed);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuprbgsi, expected_kmuprbgsi);
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuksc[0], expected_kmuksc);
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuie, expected_kmuie);
    TEST_ASSERT_EQUAL(KMU_ERROR_NONE, kmu_err);
}

void test_kmu_get_key_export_config_nok(void)
{
    enum kmu_error_t kmu_err;
    struct kmu_key_export_config_t config;
    uint32_t slot;

    /* Prepare */
    slot = 0x40;

    /* Act */
    kmu_err = kmu_get_key_export_config(&KMU_DEV_S, slot, &config);

    /* Assert */
    TEST_ASSERT_EQUAL(KMU_ERROR_INVALID_SLOT, kmu_err);
}

void test_kmu_get_key_export_config_ok(void)
{
    enum kmu_error_t kmu_err;
    struct kmu_key_export_config_t expected_config;
    struct kmu_key_export_config_t config;
    uint32_t slot;

    /* Prepare */
    slot = 0;
    expected_config.export_address = 0x50154400u;
    expected_config.destination_port_write_delay = 0x0u;
    expected_config.destination_port_address_increment = 0x1u;
    expected_config.destination_port_data_width_code =
        KMU_DESTINATION_PORT_WIDTH_32_BITS;
    expected_config.destination_port_data_writes_code =
        KMU_DESTINATION_PORT_WIDTH_8_WRITES;
    expected_config.new_mask_for_next_key_writes = true;
    expected_config.write_mask_disable = false;

    /* Act */
    kmu_err = kmu_get_key_export_config(&KMU_DEV_S, slot, &config);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(config.export_address,
                             expected_config.export_address);
    TEST_ASSERT_EQUAL_UINT8(config.destination_port_write_delay,
                            expected_config.destination_port_write_delay);
    TEST_ASSERT_EQUAL_UINT8(config.destination_port_address_increment,
                            expected_config.destination_port_address_increment);
    TEST_ASSERT_EQUAL(expected_config.destination_port_data_width_code,
                      config.destination_port_data_width_code);
    TEST_ASSERT_EQUAL(expected_config.destination_port_data_writes_code,
                      config.destination_port_data_writes_code);
    TEST_ASSERT_EQUAL(expected_config.new_mask_for_next_key_writes,
                      config.new_mask_for_next_key_writes);
    TEST_ASSERT_EQUAL(expected_config.write_mask_disable,
                      config.write_mask_disable);
    TEST_ASSERT_EQUAL(KMU_ERROR_NONE, kmu_err);
}

TEST_CASE(0x40u, 0x0u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0xAu, KMU_KMUKSC_LKS_MASK, KMU_ERROR_SLOT_LOCKED)
void test_kmu_set_key_export_config_nok(uint32_t slot, uint32_t kmuksc_mask,
                                        enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;
    struct kmu_key_export_config_t config;

    /* Prepare */
    regmap.kmuksc[slot] |= kmuksc_mask;

    /* Act */
    kmu_err = kmu_set_key_export_config(&KMU_DEV_S, slot, &config);

    /* Assert */
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

void test_kmu_set_key_export_config_ok(void)
{
    enum kmu_error_t kmu_err;
    struct kmu_key_export_config_t config;
    uint32_t expected_kmuksc;
    uint32_t expected_kmudkpa;
    uint32_t slot;

    /* Prepare */
    slot = 0xAu;

    config.export_address = 0x50154400u;
    config.destination_port_write_delay = 0x0u;
    config.destination_port_address_increment = 0x1u;
    config.destination_port_data_width_code =
        KMU_DESTINATION_PORT_WIDTH_32_BITS;
    config.destination_port_data_writes_code =
        KMU_DESTINATION_PORT_WIDTH_8_WRITES;
    config.new_mask_for_next_key_writes = true;
    config.write_mask_disable = false;

    expected_kmudkpa = 0x50154400;
    expected_kmuksc = 0x160100;

    /* Act */
    kmu_err = kmu_set_key_export_config(&KMU_DEV_S, slot, &config);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuksc[slot], expected_kmuksc);
    TEST_ASSERT_EQUAL_UINT32(regmap.kmudkpa[slot], expected_kmudkpa);
    TEST_ASSERT_EQUAL(KMU_ERROR_NONE, kmu_err);
}

TEST_CASE(0x40u, 0x0u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0xAu, KMU_KMUKSC_LKSKR_MASK, KMU_ERROR_NONE)
void test_kmu_set_key_locked(uint32_t slot, uint32_t expected_kmuksc,
                             enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;

    /* Act */
    kmu_err = kmu_set_key_locked(&KMU_DEV_S, slot);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuksc[slot], expected_kmuksc);
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

TEST_CASE(0x40u, 0x0u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0xAu, KMU_KMUKSC_LKSKR_MASK, KMU_ERROR_SLOT_LOCKED)
TEST_CASE(0xAu, 0x0u, KMU_ERROR_NONE)
void test_kmu_get_key_locked(uint32_t slot, uint32_t kmuksc_mask,
                             enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;

    /* Prepare */
    regmap.kmuksc[slot] |= kmuksc_mask;

    /* Act */
    kmu_err = kmu_get_key_locked(&KMU_DEV_S, slot);

    /* Assert */
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

TEST_CASE(0x40u, 0x0u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0xAu, KMU_KMUKSC_LKS_MASK, KMU_ERROR_NONE)
void test_kmu_set_key_export_config_locked(uint32_t slot,
                                           uint32_t expected_kmuksc,
                                           enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;

    /* Act */
    kmu_err = kmu_set_key_export_config_locked(&KMU_DEV_S, slot);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuksc[slot], expected_kmuksc);
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

TEST_CASE(0x40u, 0x0u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0xAu, KMU_KMUKSC_LKS_MASK, KMU_ERROR_SLOT_LOCKED)
TEST_CASE(0xAu, 0x0u, KMU_ERROR_NONE)
void test_kmu_get_key_export_config_locked(uint32_t slot, uint32_t kmuksc_mask,
                                           enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;

    /* Prepare */
    regmap.kmuksc[slot] |= kmuksc_mask;

    /* Act */
    kmu_err = kmu_get_key_export_config_locked(&KMU_DEV_S, slot);

    /* Assert */
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

void test_kmu_set_slot_invalid_invalid_slot(void)
{
    enum kmu_error_t kmu_err;
    uint32_t slot;

    /* Prepare */
    slot = 0x40;

    /* Act */
    kmu_err = kmu_set_slot_invalid(&KMU_DEV_S, slot);

    /* Assert */
    TEST_ASSERT_EQUAL(KMU_ERROR_INVALID_SLOT, kmu_err);
}

static void *kmu_key_invalidate_hw_op(void *thread_args)
{
    struct kmu_thread_args_t *args = (struct kmu_thread_args_t *)thread_args;

    while (!(regmap.kmuksc[args->slot] & KMU_KMUKSC_IKS_MASK)) {
    }
    regmap.kmuis |= args->kmuis_mask;
    regmap.kmuksc[args->slot] &= ~KMU_KMUKSC_IKS_MASK;

    return NULL;
}

TEST_CASE(0x0000000Au, 0x00000000u, KMU_KMISR_AIKSWE_MASK, 0x00000000u,
          0xFFFFFFFFu, KMU_ERROR_SLOT_INTERNAL_ERROR)
TEST_CASE(0x0000000Au, 0x00000000u, 0x00000000u, 0x00000000u, 0xFFFFFFFFu,
          KMU_ERROR_NONE)
void test_kmu_set_slot_invalid(uint32_t slot, uint32_t kmuksc_mask,
                               uint32_t kmuis_mask, uint32_t expected_kmuksc,
                               uint32_t expected_kmuic,
                               enum kmu_error_t expected_err)
{
    int32_t pthread_err;
    pthread_t pthread_id;
    struct kmu_thread_args_t pthread_args;
    enum kmu_error_t kmu_err;

    /* Prepare */
    pthread_args.slot = slot;
    pthread_args.kmuksc_mask = kmuksc_mask;
    pthread_args.kmuis_mask = kmuis_mask;

    /* Act */
    pthread_err = pthread_create(&pthread_id, NULL, kmu_key_invalidate_hw_op,
                                 &pthread_args);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, pthread_err, "Failed to create kmu_key_invalidate_hw_op thread");

    kmu_err = kmu_set_slot_invalid(&KMU_DEV_S, slot);

    pthread_err = pthread_join(pthread_id, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, pthread_err, "Failed to join kmu_key_invalidate_hw_op thread");

    /* Cleanup */
    regmap.kmuis = KMU_IS_RESET_VALUE;

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuksc[slot], expected_kmuksc);
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuic, expected_kmuic);
    TEST_ASSERT_EQUAL(kmu_err, expected_err);
}

TEST_CASE(0x40u, 0x0u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0xAu, KMU_KMUKSC_KSIP_MASK, KMU_ERROR_SLOT_INVALIDATED)
TEST_CASE(0xAu, 0x0u, KMU_ERROR_NONE)
void test_kmu_get_slot_invalid(uint32_t slot, uint32_t kmuksc_mask,
                               enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;

    /* Prepare */
    regmap.kmuksc[slot] |= kmuksc_mask;

    /* Act */
    kmu_err = kmu_get_slot_invalid(&KMU_DEV_S, slot);

    /* Assert */
    TEST_ASSERT_EQUAL(kmu_err, expected_err);
}

void test_kmu_set_key_invalid_alignment(void)
{
    enum kmu_error_t kmu_err;
    uint8_t key[32];
    uint32_t offset;
    uint32_t slot;

    /* Prepare */
    slot = 0xA;
    offset = sizeof(uint8_t);

    /* Act */
    kmu_err = kmu_set_key(&KMU_DEV_S, slot, key + offset, sizeof(key));

    /* Assert */
    TEST_ASSERT_EQUAL(KMU_ERROR_SET_KEY_INVALID_ALIGNMENT, kmu_err);
}

void test_kmu_set_key_invalid_length(void)
{
    enum kmu_error_t kmu_err;
    uint8_t key[32 + sizeof(uint8_t)];
    uint32_t slot;

    /* Prepare */
    slot = 0xA;

    /* Act */
    kmu_err = kmu_set_key(&KMU_DEV_S, slot, key, sizeof(key));

    /* Assert */
    TEST_ASSERT_EQUAL(KMU_ERROR_SET_KEY_INVALID_LENGTH, kmu_err);
}

TEST_CASE(0xFFu, 0x00000000u, 0x00000000u, 0x00u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0x0Au, KMU_KMUKSC_LKSKR_MASK, 0x00000000u, 0x00u,
          KMU_ERROR_SLOT_LOCKED)
TEST_CASE(0x0Au, 0x00000000u, KMU_KMISR_MWKSW_MASK, 0xAAu,
          KMU_ERROR_SET_KEY_SLOT_ALREADY_WRITTEN)
TEST_CASE(0x0Au, 0x00000000u, 0x00000000u, 0xAAu, KMU_ERROR_NONE)
void test_kmu_set_key(uint32_t slot, uint32_t kmuksc_mask, uint32_t kmuis_mask,
                      uint8_t pattern, enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;
    uint8_t expected_kmuksk[32];
    uint8_t key[32];

    /* Prepare */
    memset(key, pattern, sizeof(key));
    memset(expected_kmuksk, pattern, sizeof(expected_kmuksk));

    regmap.kmuksc[slot] |= kmuksc_mask;
    regmap.kmuis |= kmuis_mask;

    /* Act */
    kmu_err = kmu_set_key(&KMU_DEV_S, slot, key, sizeof(key));

    /* Cleanup */
    regmap.kmuis = KMU_IS_RESET_VALUE;

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32_ARRAY(regmap.kmuksk[slot],
                                   (uint32_t *)expected_kmuksk,
                                   sizeof(expected_kmuksk) / sizeof(uint32_t));
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

void test_kmu_get_key_invalid_alignment(void)
{
    enum kmu_error_t kmu_err;
    uint8_t key[32];
    uint32_t offset;
    uint32_t slot;

    /* Prepare */
    slot = 0xA;
    offset = sizeof(uint8_t);

    /* Act */
    kmu_err = kmu_get_key(&KMU_DEV_S, slot, key + offset, sizeof(key));

    /* Assert */
    TEST_ASSERT_EQUAL(KMU_ERROR_GET_KEY_INVALID_ALIGNMENT, kmu_err);
}

void test_kmu_get_key_invalid_length(void)
{
    enum kmu_error_t kmu_err;
    uint8_t key[32 + sizeof(uint8_t)];
    uint32_t slot;

    /* Prepare */
    slot = 0xA;

    /* Act */
    kmu_err = kmu_get_key(&KMU_DEV_S, slot, key, sizeof(key));

    /* Assert */
    TEST_ASSERT_EQUAL(KMU_ERROR_GET_KEY_INVALID_LENGTH, kmu_err);
}

TEST_CASE(0xFFu, 0x0u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0xAu, KMU_KMUKSC_LKSKR_MASK, KMU_ERROR_SLOT_LOCKED)
void test_kmu_get_key_nok(uint32_t slot, uint32_t kmuksc_mask,
                          enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;
    uint8_t key[32];

    /* Prepare */
    regmap.kmuksc[slot] |= kmuksc_mask;

    /* Act */
    kmu_err = kmu_get_key(&KMU_DEV_S, slot, key, sizeof(key));

    /* Assert */
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

TEST_CASE(0xAu, 0xAAAAAAAAu, KMU_ERROR_NONE)
void test_kmu_get_key_ok(uint32_t slot, uint32_t pattern,
                         enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;
    uint8_t key[32];

    /* Prepare */
    memset((uint32_t *)regmap.kmuksk[slot], pattern,
           sizeof(regmap.kmuksk[slot]));

    /* Act */
    kmu_err = kmu_get_key(&KMU_DEV_S, slot, key, sizeof(key));

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32_ARRAY(regmap.kmuksk[slot], (uint32_t *)key,
                                   sizeof(key) / sizeof(uint32_t));
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

TEST_CASE(0xFFu, 0x0u, KMU_ERROR_INVALID_SLOT)
TEST_CASE(0xAu, KMU_KMUKSC_LKSKR_MASK, KMU_ERROR_SLOT_LOCKED)
void test_kmu_get_key_buffer_ptr_nok(uint32_t slot, uint32_t kmuksc_mask,
                                     enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;
    volatile uint32_t *key_slot;
    size_t slot_size;

    /* Prepare */
    regmap.kmuksc[slot] |= kmuksc_mask;

    /* Act */
    kmu_err = kmu_get_key_buffer_ptr(&KMU_DEV_S, slot, &key_slot, &slot_size);

    /* Assert */
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

void test_kmu_get_key_buffer_ptr_ok(void)
{
    enum kmu_error_t kmu_err;
    volatile uint32_t *key_slot;
    size_t slot_size;
    uint32_t slot;

    /* Prepare */
    slot = 0xA;

    /* Act */
    kmu_err = kmu_get_key_buffer_ptr(&KMU_DEV_S, slot, &key_slot, &slot_size);

    /* Assert */
    TEST_ASSERT_EQUAL_PTR(key_slot, regmap.kmuksk[slot]);
    TEST_ASSERT_EQUAL_UINT32(slot_size, sizeof(regmap.kmuksk[slot]));
    TEST_ASSERT_EQUAL(KMU_ERROR_NONE, kmu_err);
}

void test_kmu_export_key_invalid_slot(void)
{
    enum kmu_error_t kmu_err;
    uint32_t slot;

    /* Prepare */
    slot = 0x40;

    /* Act */
    kmu_err = kmu_export_key(&KMU_DEV_S, slot);

    /* Assert */
    TEST_ASSERT_EQUAL(KMU_ERROR_INVALID_SLOT, kmu_err);
}

static void *kmu_key_ready_hw_op(void *thread_args)
{
    struct kmu_thread_args_t *args = (struct kmu_thread_args_t *)thread_args;

    while (!(regmap.kmuksc[args->slot] & KMU_KMUKSC_VKS_MASK)) {
    }
    regmap.kmuis |= args->kmuis_mask;
    regmap.kmuksc[args->slot] |= args->kmuksc_mask;
    regmap.kmuksc[args->slot] &= ~KMU_KMUKSC_VKS_MASK;

    return NULL;
}

static void *kmu_key_export_hw_op(void *thread_args)
{
    struct kmu_thread_args_t *args = (struct kmu_thread_args_t *)thread_args;

    while (!(regmap.kmuksc[args->slot] & KMU_KMUKSC_EK_MASK)) {
    }
    regmap.kmuis |= args->kmuis_mask;
    regmap.kmuksc[args->slot] &= ~KMU_KMUKSC_EK_MASK;

    return NULL;
}

TEST_CASE(0xAu, 0x00000000u, KMU_KMISR_KSNL_MASK, 0x0u, 0xFFFFFFFFu,
          KMU_ERROR_EXPORT_KEY_SLOT_NOT_LOCKED)
TEST_CASE(0xAu, 0x00000000u, KMU_KMISR_KSKRSM_MASK, 0x0u, 0xFFFFFFFFu,
          KMU_ERROR_EXPORT_KEY_INVALID_LENGTH)
TEST_CASE(0xAu, 0x00000000u, KMU_KMISR_KSDPANS_MASK, 0x0u, 0xFFFFFFFFu,
          KMU_ERROR_EXPORT_KEY_INVALID_ADDR)
TEST_CASE(0xAu, 0x00000000u, 0x00000000u, 0x0u, 0xFFFFFFFFu,
          KMU_ERROR_EXPORT_KEY_INTERNAL_ERROR)
void test_kmu_export_key_ready_op_nok(uint32_t slot, uint32_t kmuksc_mask,
                                      uint32_t kmuis_mask,
                                      uint32_t expected_kmuksc,
                                      uint32_t expected_kmuic,
                                      enum kmu_error_t expected_err)
{
    pthread_t pthread_id;
    int32_t pthread_err;
    struct kmu_thread_args_t pthread_args;
    enum kmu_error_t kmu_err;

    /* Prepare */
    pthread_args.slot = slot;
    pthread_args.kmuksc_mask = kmuksc_mask;
    pthread_args.kmuis_mask = kmuis_mask;

    /* Act */
    pthread_err =
        pthread_create(&pthread_id, NULL, kmu_key_ready_hw_op, &pthread_args);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, pthread_err, "Failed to create kmu_key_ready_hw_op thread");

    kmu_err = kmu_export_key(&KMU_DEV_S, slot);

    pthread_err = pthread_join(pthread_id, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pthread_err,
                                  "Failed to join kmu_key_ready_hw_op thread");

    /* Cleanup */
    regmap.kmuis = KMU_IS_RESET_VALUE;

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuksc[slot], expected_kmuksc);
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuic, expected_kmuic);
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

TEST_CASE(0xAu, KMU_KMUKSC_KSR_MASK, KMU_KMISR_KEC_MASK | KMU_KMISR_WTE_MASK,
          KMU_KMUKSC_KSR_MASK, 0xFFFFFFFFu, KMU_ERROR_EXPORT_KEY_INTERNAL_ERROR)
TEST_CASE(0xAu, KMU_KMUKSC_KSR_MASK, KMU_KMISR_KEC_MASK | KMU_KMISR_INPPE_MASK,
          KMU_KMUKSC_KSR_MASK, 0xFFFFFFFFu, KMU_ERROR_EXPORT_KEY_INTERNAL_ERROR)
TEST_CASE(0xAu, KMU_KMUKSC_KSR_MASK, KMU_KMISR_KEC_MASK | KMU_KMISR_AWBE_MASK,
          KMU_KMUKSC_KSR_MASK, 0xFFFFFFFFu, KMU_ERROR_EXPORT_KEY_INTERNAL_ERROR)
TEST_CASE(0xAu, KMU_KMUKSC_KSR_MASK, KMU_KMISR_KEC_MASK, KMU_KMUKSC_KSR_MASK,
          0xFFFFFFFFu, KMU_ERROR_NONE)
void test_kmu_export_key_ready_op_ok(uint32_t slot, uint32_t kmuksc_mask,
                                     uint32_t kmuis_mask,
                                     uint32_t expected_kmuksc,
                                     uint32_t expected_kmuic,
                                     enum kmu_error_t expected_err)
{
    int32_t pthread_err;
    pthread_t pthread_0_id, pthread_1_id;
    struct kmu_thread_args_t pthread_0_args, pthread_1_args;
    enum kmu_error_t kmu_err;

    /* Prepare */
    pthread_0_args.slot = slot;
    pthread_0_args.kmuksc_mask = kmuksc_mask;
    pthread_0_args.kmuis_mask = 0x0u;

    pthread_1_args.slot = slot;
    pthread_1_args.kmuksc_mask = 0x0u;
    pthread_1_args.kmuis_mask = kmuis_mask;

    /* Act */
    pthread_err = pthread_create(&pthread_0_id, NULL, kmu_key_ready_hw_op,
                                 &pthread_0_args);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, pthread_err, "Failed to create kmu_key_ready_hw_op thread");

    pthread_err = pthread_create(&pthread_1_id, NULL, kmu_key_export_hw_op,
                                 &pthread_1_args);
    TEST_ASSERT_EQUAL_INT_MESSAGE(
        0, pthread_err, "Failed to create kmu_key_export_hw_op thread");

    kmu_err = kmu_export_key(&KMU_DEV_S, slot);

    pthread_err = pthread_join(pthread_0_id, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pthread_err,
                                  "Failed to join kmu_key_ready_hw_op thread");

    pthread_err = pthread_join(pthread_1_id, NULL);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, pthread_err,
                                  "Failed to join kmu_key_export_hw_op thread");

    /* Cleanup */
    regmap.kmuis = KMU_IS_RESET_VALUE;

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuksc[slot], expected_kmuksc);
    TEST_ASSERT_EQUAL_UINT32(regmap.kmuic, expected_kmuic);
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}

TEST_CASE(KMU_DELAY_LIMIT_8_CYCLES, KMU_ERROR_NONE)
TEST_CASE(KMU_DELAY_LIMIT_16_CYCLES, KMU_ERROR_NONE)
TEST_CASE(KMU_DELAY_LIMIT_32_CYCLES, KMU_ERROR_NONE)
TEST_CASE(KMU_DELAY_LIMIT_32_CYCLES + 1, KMU_ERROR_INVALID_DELAY_LENGTH)
void test_kmu_random_delay(enum kmu_delay_limit_t limit,
                           enum kmu_error_t expected_err)
{
    enum kmu_error_t kmu_err;

    /* Act */
    kmu_err = kmu_random_delay(&KMU_DEV_S, limit);

    /* Assert */
    TEST_ASSERT_EQUAL(expected_err, kmu_err);
}
