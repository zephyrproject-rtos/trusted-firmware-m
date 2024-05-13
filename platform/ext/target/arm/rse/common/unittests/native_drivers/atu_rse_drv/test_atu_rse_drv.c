/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "atu_rse_drv.h"

#include "unity.h"

/* Dummy logical and physical addresses */
#define ATU_DUMMY_SLOT_LOG_ADDR 0x70000000
#define ATU_DUMMY_SLOT_PHY_ADDR 0x80000000
#define ATU_DUMMY_SLOT_SIZE 0x4000

/*
 * RSE ATU build configuration values (atubc):
 *  ATUNTR  = 0x5 -> 32 translation regions
 *  ATUPS   = 0xD -> 8192 byte page size
 *  ATUPAW  = 0x5 -> 52 bit physical address width
 */
#define ATU_BC_RESET_VALUE 0x5D5u   /* Build configuration register */
#define ATU_C_RESET_VALUE 0x0u      /* Configuration register */
#define ATU_IS_RESET_VALUE 0x0u     /* Interrupt status register */
#define ATU_IE_RESET_VALUE 0x1u     /* Interrupt enable register */
#define ATU_IC_RESET_VALUE 0x0u     /* Interrupt clear register */
#define ATU_MA_RESET_VALUE 0x0u     /* Mismatched address register */
#define ATU_PIDR0_RESET_VALUE 0xC0u /* Peripheral ID 0 register */
#define ATU_PIDR1_RESET_VALUE 0xB3u /* Peripheral ID 1 register */
#define ATU_PIDR2_RESET_VALUE 0xBu  /* Peripheral ID 2 register */
#define ATU_PIDR3_RESET_VALUE 0x0u  /* Peripheral ID 3 register */
#define ATU_PIDR4_RESET_VALUE 0x4u  /* Peripheral ID 4 register */
#define ATU_CIDR0_RESET_VALUE 0xDu  /* Component ID 0 register */
#define ATU_CIDR1_RESET_VALUE 0xF0u /* Component ID 1 register */
#define ATU_CIDR2_RESET_VALUE 0x5u  /* Component ID 2 register */
#define ATU_CIDR3_RESET_VALUE 0xB1u /* Component ID 3 register */

/* ATU register map structure */
static struct _atu_reg_map_t {
    uint32_t atubc;
    /*!< Offset: 0x000 (R/O) ATU Build Configuration Register */
    uint32_t atuc;
    /*!< Offset: 0x004 (R/W) ATU Configuration Register */
    uint32_t atuis;
    /*!< Offset: 0x008 (R/O) ATU Interrupt Status Register */
    uint32_t atuie;
    /*!< Offset: 0x00C (R/W) ATU Interrupt Enable Register */
    uint32_t atuic;
    /*!< Offset: 0x010 (R/W) ATU Interrupt Clear Register */
    uint32_t atuma;
    /*!< Offset: 0x014 (R/O) ATU Mismatched Address Register */
    uint32_t reserved_0[2];
    /*!< Offset: 0x018-0x01C Reserved */
    uint32_t atursla[32];
    /*!< Offset: 0x020 (R/W) ATU Region Start Logical Address n
     *                       Register */
    uint32_t aturela[32];
    /*!< Offset: 0x0A0 (R/W) ATU Region End Logical Address n
     *                       Register */
    uint32_t aturav_l[32];
    /*!< Offset: 0x120 (R/W) ATU Region Add Value Low n Register */
    uint32_t aturav_m[32];
    /*!< Offset: 0x1A0 (R/W) ATU Region Add Value High n Register */
    uint32_t aturoba[32];
    /*!< Offset: 0x220 (R/W) ATU Region Output Bus Attributes n
     *                       Register */
    uint32_t aturgp[32];
    /*!< Offset: 0x2A0 (R/W) ATU Region General Purpose n
     *                       Register */
    uint32_t reserved_1[811]; /*!< Offset: 0x320-0xFCC Reserved */
    uint32_t pidr4;
    /*!< Offset: 0xFD0 (R/O) Peripheral ID 4 */
    uint32_t reserved_2[2];
    /*!< Offset: 0xFD4-0xFDC Reserved */
    uint32_t pidr0;
    /*!< Offset: 0xFE0 (R/O) Peripheral ID 0 */
    uint32_t pidr1;
    /*!< Offset: 0xFE4 (R/O) Peripheral ID 1 */
    uint32_t pidr2;
    /*!< Offset: 0xFE8 (R/O) Peripheral ID 2 */
    uint32_t pidr3;
    /*!< Offset: 0xFEC (R/O) Peripheral ID 3 */
    uint32_t cidr0;
    /*!< Offset: 0xFF0 (R/O) Component ID 0 */
    uint32_t cidr1;
    /*!< Offset: 0xFF4 (R/O) Component ID 1 */
    uint32_t cidr2;
    /*!< Offset: 0xFF8 (R/O) Component ID 2 */
    uint32_t cidr3;
    /*!< Offset: 0xFFC (R/O) Component ID 3 */
} regmap;

static const struct atu_dev_cfg_t ATU_DEV_CFG_S = {.base = (uintptr_t)&regmap};

static struct atu_dev_t ATU_DEV_S = {.cfg = &ATU_DEV_CFG_S};

/* Set ATU registers to their default reset values */
void set_default_register_values(void)
{
    regmap.atubc = ATU_BC_RESET_VALUE;
    regmap.atuc = ATU_C_RESET_VALUE;
    regmap.atuis = ATU_IS_RESET_VALUE;
    regmap.atuie = ATU_IE_RESET_VALUE;
    regmap.atuic = ATU_IC_RESET_VALUE;
    regmap.atuma = ATU_MA_RESET_VALUE;
    memset(regmap.reserved_0, 0x0u, sizeof(regmap.reserved_0));
    memset(regmap.atursla, 0x0u, sizeof(regmap.atursla));
    memset(regmap.aturela, 0x0u, sizeof(regmap.aturela));
    memset(regmap.aturav_l, 0x0u, sizeof(regmap.aturav_l));
    memset(regmap.aturav_m, 0x0u, sizeof(regmap.aturav_m));
    memset(regmap.aturoba, 0x0u, sizeof(regmap.aturoba));
    memset(regmap.aturgp, 0x0u, sizeof(regmap.aturgp));
    memset(regmap.reserved_1, 0x0u, sizeof(regmap.reserved_1));
    regmap.pidr4 = ATU_PIDR4_RESET_VALUE;
    memset(regmap.reserved_2, 0x0u, sizeof(regmap.reserved_2));
    regmap.pidr0 = ATU_PIDR0_RESET_VALUE;
    regmap.pidr1 = ATU_PIDR1_RESET_VALUE;
    regmap.pidr2 = ATU_PIDR2_RESET_VALUE;
    regmap.pidr3 = ATU_PIDR3_RESET_VALUE;
    regmap.cidr0 = ATU_CIDR0_RESET_VALUE;
    regmap.cidr1 = ATU_CIDR1_RESET_VALUE;
    regmap.cidr2 = ATU_CIDR2_RESET_VALUE;
    regmap.cidr3 = ATU_CIDR3_RESET_VALUE;
}

/* Verify that Read-Only registers have not been modified */
void verify_read_only_registers(void)
{
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_BC_RESET_VALUE, regmap.atubc,
                                     "Read-Only register ATUBC was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_IS_RESET_VALUE, regmap.atuis,
                                     "Read-Only register ATUIS was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_MA_RESET_VALUE, regmap.atuma,
                                     "Read-Only register ATUMA was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_PIDR4_RESET_VALUE, regmap.pidr4,
                                     "Read-Only register PIDR4 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_PIDR0_RESET_VALUE, regmap.pidr0,
                                     "Read-Only register PIDR0 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_PIDR1_RESET_VALUE, regmap.pidr1,
                                     "Read-Only register PIDR1 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_PIDR2_RESET_VALUE, regmap.pidr2,
                                     "Read-Only register PIDR2 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_PIDR3_RESET_VALUE, regmap.pidr3,
                                     "Read-Only register PIDR3 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_CIDR0_RESET_VALUE, regmap.cidr0,
                                     "Read-Only register CIDR0 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_CIDR1_RESET_VALUE, regmap.cidr1,
                                     "Read-Only register CIDR1 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_CIDR2_RESET_VALUE, regmap.cidr2,
                                     "Read-Only register CIDR2 was modified");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(ATU_CIDR3_RESET_VALUE, regmap.cidr3,
                                     "Read-Only register CIDR3 was modified");
}

void setUp(void) { set_default_register_values(); }

void tearDown(void) { verify_read_only_registers(); }

void test_get_page_size_ok(void)
{
    uint16_t page_size;
    uint16_t expected_page_size;

    /* Prepare */
    expected_page_size = 0x1u << ((ATU_BC_RESET_VALUE >> 4) & 0xFu);

    /* Act */
    page_size = get_page_size(&ATU_DEV_S);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT16(expected_page_size, page_size);
}

void test_get_supported_region_count_ok(void)
{
    uint8_t supported_region;
    uint8_t expected_supported_region;

    /* Prepare */
    expected_supported_region = 0x20;

    /* Act */
    supported_region = get_supported_region_count(&ATU_DEV_S);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT8(expected_supported_region, supported_region);
}

void test_enable_atu_region_invalid_region(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_atuc;
    uint8_t region;

    /* Prepare */
    region = 0xFF;
    expected_atuc = ATU_C_RESET_VALUE;

    /* Act */
    atu_err = enable_atu_region(&ATU_DEV_S, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atuc, expected_atuc);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_REGION);
}

void test_enable_atu_region_ok(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_atuc;
    uint8_t region;

    /* Prepare */
    region = 0x1;
    expected_atuc = ATU_C_RESET_VALUE | (1u << region);

    /* Act */
    atu_err = enable_atu_region(&ATU_DEV_S, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atuc, expected_atuc);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_disable_atu_region_invalid_region(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_atuc;
    uint8_t region;

    /* Prepare */
    region = 0xFF;
    expected_atuc = ATU_C_RESET_VALUE;

    /* Act */
    atu_err = disable_atu_region(&ATU_DEV_S, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atuc, expected_atuc);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_REGION);
}

void test_disable_atu_region_ok(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_atuc;
    uint8_t region;

    /* Prepare */
    region = 0x1;
    regmap.atuc = ATU_C_RESET_VALUE | (1u << region);
    expected_atuc = ATU_C_RESET_VALUE;

    /* Act */
    atu_err = disable_atu_region(&ATU_DEV_S, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atuc, expected_atuc);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_me_interrupt_is_waiting_int_available(void)
{
    bool int_available;

    /* Act */
    int_available = me_interrupt_is_waiting(&ATU_DEV_S);

    /* Assert */
    TEST_ASSERT_EQUAL(int_available, false);
}

void test_me_interrupt_is_watiting_no_int(void)
{
    bool int_available;

    /* Prepare */
    regmap.atuis = 0x0u;

    /* Act */
    int_available = me_interrupt_is_waiting(&ATU_DEV_S);

    /* Assert */
    TEST_ASSERT_EQUAL(int_available, false);
}

void test_enable_me_interrupt_ok(void)
{
    /* Act */
    enable_me_interrupt(&ATU_DEV_S);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atuie, 0x1u);
}

void test_clear_me_interrupt_ok(void)
{
    /* Act */
    clear_me_interrupt(&ATU_DEV_S);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atuic, 0x1u);
}

void test_get_mismatch_address(void)
{
    uint32_t mismatch_address;

    /* Act */
    mismatch_address = get_mismatch_address(&ATU_DEV_S);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atuma, mismatch_address);
}

void test_set_start_logical_address_invalid_region(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_atursla;
    uint8_t region;

    /* Prepare */
    expected_atursla = 0x0;
    region = 0xFF;

    /* Act */
    atu_err =
        set_start_logical_address(&ATU_DEV_S, ATU_DUMMY_SLOT_LOG_ADDR, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atursla[region], expected_atursla);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_REGION);
}

void test_set_start_logical_address_ok(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_atursla;
    uint16_t page_size;
    uint8_t region;

    /* Prepare */
    page_size = ((ATU_BC_RESET_VALUE >> 4) & 0xFu);
    expected_atursla = (ATU_DUMMY_SLOT_LOG_ADDR >> page_size);
    region = 0x1;

    /* Act */
    atu_err =
        set_start_logical_address(&ATU_DEV_S, ATU_DUMMY_SLOT_LOG_ADDR, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.atursla[region], expected_atursla);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_end_logical_address_invalid_region(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_atursla;
    uint32_t region_size;
    uint8_t region;

    /* Prepare */
    region_size = 0x200;
    expected_atursla = 0x0;
    region = 0xFF;

    /* Act */
    atu_err = set_end_logical_address(
        &ATU_DEV_S, ATU_DUMMY_SLOT_LOG_ADDR + region_size, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturela[region], expected_atursla);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_REGION);
}

void test_set_end_logical_address_invalid_end_address(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_aturela;
    uint32_t region_size;
    uint16_t page_size;
    uint8_t region;

    /* Prepare */
    region_size = ATU_DUMMY_SLOT_SIZE;
    page_size = ((ATU_BC_RESET_VALUE >> 4) & 0xFu);
    expected_aturela = 0x0;
    region = 0x1;

    regmap.atursla[region] = (ATU_DUMMY_SLOT_LOG_ADDR >> page_size);

    /* Act */
    atu_err = set_end_logical_address(
        &ATU_DEV_S, ATU_DUMMY_SLOT_LOG_ADDR - region_size, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturela[region], expected_aturela);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_LOGICAL_ADDRESS);
}

void test_set_end_logical_address_ok(void)
{
    enum atu_error_t atu_err;
    uint32_t expected_aturela;
    uint32_t region_size;
    uint16_t page_size;
    uint8_t region;

    /* Prepare */
    region_size = ATU_DUMMY_SLOT_SIZE;
    page_size = ((ATU_BC_RESET_VALUE >> 4) & 0xFu);
    expected_aturela = (ATU_DUMMY_SLOT_LOG_ADDR + region_size) >> page_size;
    region = 0x1;

    regmap.atursla[region] = (ATU_DUMMY_SLOT_LOG_ADDR >> page_size);

    /* Act */
    atu_err = set_end_logical_address(
        &ATU_DEV_S, ATU_DUMMY_SLOT_LOG_ADDR + region_size, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturela[region], expected_aturela);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_add_value_invalid_region(void)
{
    enum atu_error_t atu_err;
    uint64_t offset_address;
    uint32_t expected_aturav_l;
    uint32_t expected_aturav_m;
    uint16_t page_size;
    uint8_t region;

    /* Prepare */
    page_size = ((ATU_BC_RESET_VALUE >> 4) & 0xFu);
    offset_address =
        (ATU_DUMMY_SLOT_PHY_ADDR - ATU_DUMMY_SLOT_LOG_ADDR) >> page_size;
    expected_aturav_l = 0x0;
    expected_aturav_m = 0x0;
    region = 0xFF;

    /* Act */
    atu_err = set_add_value(&ATU_DEV_S, offset_address, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturav_l[region], expected_aturav_l);
    TEST_ASSERT_EQUAL_UINT32(regmap.aturav_m[region], expected_aturav_m);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_REGION);
}

void test_set_add_value_invalid_region_ok(void)
{
    enum atu_error_t atu_err;
    uint64_t offset_address;
    uint32_t expected_aturav_l;
    uint32_t expected_aturav_m;
    uint16_t page_size;
    uint8_t region;

    /* Prepare */
    page_size = ((ATU_BC_RESET_VALUE >> 4) & 0xFu);
    offset_address =
        (ATU_DUMMY_SLOT_PHY_ADDR - ATU_DUMMY_SLOT_LOG_ADDR) >> page_size;
    expected_aturav_l = (uint32_t)offset_address;
    expected_aturav_m = (uint32_t)(offset_address >> 32);
    region = 0x1;

    /* Act */
    atu_err = set_add_value(&ATU_DEV_S, offset_address, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturav_l[region], expected_aturav_l);
    TEST_ASSERT_EQUAL_UINT32(regmap.aturav_m[region], expected_aturav_m);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_axnsc_invalid_region(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t region;

    /* Prepare */
    expected_aturoba = 0x0;
    atu_roba = ATU_ROBA_SET_0;
    region = 0xFF;

    /* Act */
    atu_err = set_axnsc(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_REGION);
}

void test_set_axnsc_ok(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t axnsc_offs;
    uint8_t region;

    /* Prepare */
    atu_roba = ATU_ROBA_SET_0;
    axnsc_offs = 14u;
    expected_aturoba = (atu_roba << axnsc_offs);
    region = 0x1;

    /* Act */
    atu_err = set_axnsc(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_axcache3_ok(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t axcache3_offs;
    uint8_t region;

    /* Prepare */
    atu_roba = ATU_ROBA_SET_0;
    axcache3_offs = 12u;
    expected_aturoba = (atu_roba << axcache3_offs);
    region = 0x1;

    /* Act */
    atu_err = set_axcache3(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_axcache2_ok(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t axcache2_offs;
    uint8_t region;

    /* Prepare */
    atu_roba = ATU_ROBA_SET_0;
    axcache2_offs = 10u;
    expected_aturoba = (atu_roba << axcache2_offs);
    region = 0x1;

    /* Act */
    atu_err = set_axcache2(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_axcache1_ok(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t axcache1_offs;
    uint8_t region;

    /* Prepare */
    atu_roba = ATU_ROBA_SET_0;
    axcache1_offs = 8u;
    expected_aturoba = (atu_roba << axcache1_offs);
    region = 0x1;

    /* Act */
    atu_err = set_axcache1(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_axcache0_ok(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t axcache0_offs;
    uint8_t region;

    /* Prepare */
    atu_roba = ATU_ROBA_SET_0;
    axcache0_offs = 6u;
    expected_aturoba = (atu_roba << axcache0_offs);
    region = 0x1;

    /* Act */
    atu_err = set_axcache0(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_axprot2_ok(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t axprot2_offs;
    uint8_t region;

    /* Prepare */
    atu_roba = ATU_ROBA_SET_0;
    axprot2_offs = 4u;
    expected_aturoba = (atu_roba << axprot2_offs);
    region = 0x1;

    /* Act */
    atu_err = set_axprot2(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_axprot1_ok(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t axprot1_offs;
    uint8_t region;

    /* Prepare */
    atu_roba = ATU_ROBA_SET_0;
    axprot1_offs = 2u;
    expected_aturoba = (atu_roba << axprot1_offs);
    region = 0x1;

    /* Act */
    atu_err = set_axprot1(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_axprot0_ok(void)
{
    enum atu_error_t atu_err;
    enum atu_roba_t atu_roba;
    uint32_t expected_aturoba;
    uint8_t axprot0_offs;
    uint8_t region;

    /* Prepare */
    atu_roba = ATU_ROBA_SET_0;
    axprot0_offs = 0u;
    expected_aturoba = (atu_roba << axprot0_offs);
    region = 0x1;

    /* Act */
    atu_err = set_axprot0(&ATU_DEV_S, atu_roba, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.aturoba[region], expected_aturoba);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_set_gp_value_invalid_region(void)
{
    enum atu_error_t atu_err;
    uint8_t expected_val;
    uint8_t region;
    uint8_t val;

    /* Prepare */
    region = 0xFF;
    val = 0xAA;
    expected_val = 0x0;

    /* Act */
    atu_err = set_gp_value(&ATU_DEV_S, val, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT8(regmap.aturgp[region], expected_val);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_REGION);
}

void test_set_gp_value_ok(void)
{
    enum atu_error_t atu_err;
    uint8_t expected_val;
    uint8_t region;
    uint8_t val;

    /* Prepare */
    region = 0x1;
    val = 0xAA;
    expected_val = 0xAA;

    /* Act */
    atu_err = set_gp_value(&ATU_DEV_S, val, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT8(regmap.aturgp[region], expected_val);
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_get_gp_value_ok(void)
{
    uint8_t region;
    uint8_t val;

    /* Prepare */
    region = 0x1;
    regmap.aturgp[region] = 0xAA;

    /* Act */
    val = get_gp_value(&ATU_DEV_S, region);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT8(regmap.aturgp[region], val);
}

void test_atu_initialize_region_invalid_atu_dev(void)
{
    enum atu_error_t atu_err;
    uint32_t size;
    uint8_t region;

    /* Prepare */
    size = ATU_DUMMY_SLOT_SIZE;
    region = 0x1;

    /* Act */
    atu_err = atu_initialize_region(NULL, region, ATU_DUMMY_SLOT_LOG_ADDR,
                                    ATU_DUMMY_SLOT_PHY_ADDR, size);

    /* Assert */
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INIT_REGION_INVALID_ARG);
}

void test_atu_initialize_region_invalid_size(void)
{
    enum atu_error_t atu_err;
    uint32_t size;
    uint8_t region;

    /* Prepare */
    size = 0xDEAD;
    region = 0x1;

    /* Act */
    atu_err = atu_initialize_region(&ATU_DEV_S, region, ATU_DUMMY_SLOT_LOG_ADDR,
                                    ATU_DUMMY_SLOT_PHY_ADDR, size);

    /* Assert */
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INIT_REGION_INVALID_ADDRESS);
}

void test_atu_initialize_region_ok(void)
{
    enum atu_error_t atu_err;
    uint8_t region;

    /* Prepare */
    region = 0x1;

    /* Act */
    atu_err =
        atu_initialize_region(&ATU_DEV_S, region, ATU_DUMMY_SLOT_LOG_ADDR,
                              ATU_DUMMY_SLOT_PHY_ADDR, ATU_DUMMY_SLOT_SIZE);

    /* Assert */
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}

void test_atu_uninitialize_region_invalid_atu_dev(void)
{
    enum atu_error_t atu_err;
    uint8_t region;

    /* Prepare */
    region = 0x1;

    /* Act */
    atu_err = atu_uninitialize_region(NULL, region);

    /* Assert */
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_UNINIT_REGION_INVALID_ARG);
}

void test_atu_uninitialize_region_disable_atu_region_fails(void)
{
    enum atu_error_t atu_err;
    uint8_t region;

    /* Prepare */
    region = 0xff;

    /* Act */
    atu_err = atu_uninitialize_region(&ATU_DEV_S, region);

    /* Assert */
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_INVALID_REGION);
}

void test_atu_uninitialize_region_ok(void)
{
    enum atu_error_t atu_err;
    uint8_t region;

    /* Prepare */
    region = 0x1;

    /* Act */
    atu_err = atu_uninitialize_region(&ATU_DEV_S, region);

    /* Assert */
    TEST_ASSERT_EQUAL(atu_err, ATU_ERR_NONE);
}
