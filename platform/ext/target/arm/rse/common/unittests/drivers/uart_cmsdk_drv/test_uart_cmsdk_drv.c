/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdlib.h>
#include <string.h>

#include "uart_cmsdk_reg_map.h"
#include "uart_cmsdk_drv.h"

#include "unity.h"

#define DEFAULT_PERIPHERAL_CLOCK 75000000UL
#define DEFAULT_UART_BAUDRATE 115200UL

static struct uart_cmsdk_reg_map_t regmap;
static struct uart_cmsdk_dev_cfg_t UART0_CMSDK_DEV_CFG_NS = {
    .base = (uintptr_t) &regmap,
    .default_baudrate = DEFAULT_UART_BAUDRATE
};
static struct uart_cmsdk_dev_data_t UART0_CMSDK_DEV_DATA_NS = {
    .state = 0,
    .system_clk = 0,
    .baudrate = 0
};
struct uart_cmsdk_dev_t UART0_CMSDK_DEV_NS = {
    &(UART0_CMSDK_DEV_CFG_NS),
    &(UART0_CMSDK_DEV_DATA_NS)
};

void set_default_values(void)
{
    UART0_CMSDK_DEV_DATA_NS.state = 0;
    UART0_CMSDK_DEV_DATA_NS.system_clk = 0;
    UART0_CMSDK_DEV_DATA_NS.baudrate = 0;

    regmap.data = 0x0u;
    regmap.state = 0x0u;
    regmap.ctrl = 0x0u;
    regmap.intr_reg.intrstatus = 0x0u;
    regmap.intr_reg.intrclear = 0x0u;
    regmap.bauddiv = 0x0u;
}

void setUp(void)
{
    set_default_values();
}

TEST_CASE(0UL, UART_CMSDK_ERR_INIT_INVALID_ARG)
TEST_CASE(75000000UL, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_init(uint32_t system_clk,
                          enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Act */
    err = uart_cmsdk_init(&UART0_CMSDK_DEV_NS, system_clk);

    /* Assert */
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0, 0UL, UART_CMSDK_ERR_INVALID_BAUD)
TEST_CASE(0, 5000000UL, UART_CMSDK_ERR_NOT_INIT)
TEST_CASE(1, 5000000UL, UART_CMSDK_ERR_INVALID_BAUD)
TEST_CASE(1, 115200UL, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_set_baudrate(uint32_t state,
                                  uint32_t baudrate,
                                  enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;
    UART0_CMSDK_DEV_DATA_NS.system_clk = DEFAULT_PERIPHERAL_CLOCK;

    /* Act */
    err = uart_cmsdk_set_baudrate(&UART0_CMSDK_DEV_NS, baudrate);

    /* Assert */
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

void test_uart_cmsdk_get_baudrate(void)
{
    enum uart_cmsdk_error_t baudrate;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.baudrate = DEFAULT_PERIPHERAL_CLOCK;

    /* Act */
    baudrate = uart_cmsdk_get_baudrate(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL(baudrate,
            DEFAULT_PERIPHERAL_CLOCK);
}

TEST_CASE(0, 0UL, UART_CMSDK_ERR_SET_CLOCK_INVALID_ARG)
TEST_CASE(0, 75000000UL, UART_CMSDK_ERR_NOT_INIT)
TEST_CASE(1, 75000000UL, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_set_clock(uint32_t state,
                               uint32_t system_clk,
                               enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;
    UART0_CMSDK_DEV_DATA_NS.baudrate = DEFAULT_UART_BAUDRATE;

    /* Act */
    err = uart_cmsdk_set_clock(&UART0_CMSDK_DEV_NS, system_clk);

    /* Assert */
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x0u, 0x00u, UART_CMSDK_ERR_READ_NOT_READY)
TEST_CASE(0x2u, 0xFFu, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_read(uint32_t uart_state,
                          uint8_t expected_rx_data,
                          enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;
    uint8_t rx_data;

    /* Prepare */
    rx_data = 0;
    regmap.state = uart_state;
    regmap.data = expected_rx_data;

    /* Act */
    err = uart_cmsdk_read(&UART0_CMSDK_DEV_NS, &rx_data);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT8(rx_data,
            expected_rx_data);
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x1u, 0x00u, UART_CMSDK_ERR_WRITE_NOT_READY)
TEST_CASE(0x0u, 0xFFu, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_write(uint32_t uart_state,
                           uint8_t tx_data,
                           enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    regmap.state = uart_state;

    /* Act */
    err = uart_cmsdk_write(&UART0_CMSDK_DEV_NS, tx_data);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT8(regmap.data,
            tx_data);
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x0u, 0x0u, UART_CMSDK_ERR_NOT_INIT)
TEST_CASE(0x1u, 0x4u, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_irq_tx_enable(uint32_t state,
                                   uint32_t expected_uart_ctrl,
                                   enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;

    /* Act */
    err = uart_cmsdk_irq_tx_enable(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.ctrl,
            expected_uart_ctrl);
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x0u, 0x4u, 0x4u)
TEST_CASE(0x1u, 0x4u, 0x0u)
void test_uart_cmsdk_irq_tx_disable(uint32_t state,
                                    uint32_t uart_ctrl,
                                    uint32_t expected_uart_ctrl)
{
    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;
    regmap.ctrl = uart_ctrl;

    /* Act */
    uart_cmsdk_irq_tx_disable(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.ctrl,
            expected_uart_ctrl);
}

TEST_CASE(0x0u, 0x0u, UART_CMSDK_ERR_NONE)
TEST_CASE(0x1u, 0x0u, 0x1u)
TEST_CASE(0x1u, 0x1u, 0x0u)
void test_uart_cmsdk_tx_ready(uint32_t state,
                              uint32_t uart_state,
                              enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;
    regmap.state = uart_state;

    /* Act */
    err = uart_cmsdk_tx_ready(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x0u, 0x0u, UART_CMSDK_ERR_NOT_INIT)
TEST_CASE(0x1u, 0x8u, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_irq_rx_enable(uint32_t state,
                                   uint32_t expected_uart_ctrl,
                                   enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;

    /* Act */
    err = uart_cmsdk_irq_rx_enable(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.ctrl,
            expected_uart_ctrl);
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x0u, 0x8u, 0x8u)
TEST_CASE(0x1u, 0x8u, 0x0u)
void test_uart_cmsdk_irq_rx_disable(uint32_t state,
                                    uint32_t uart_ctrl,
                                    uint32_t expected_uart_ctrl)
{
    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;
    regmap.ctrl = uart_ctrl;

    /* Act */
    uart_cmsdk_irq_rx_disable(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.ctrl,
            expected_uart_ctrl);
}

TEST_CASE(0x0u, 0x0u, UART_CMSDK_ERR_NONE)
TEST_CASE(0x1u, 0x2u, 0x2u)
TEST_CASE(0x1u, 0x0u, 0x0u)
void test_uart_cmsdk_rx_ready(uint32_t state,
                              uint32_t uart_state,
                              enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;
    regmap.state = uart_state;

    /* Act */
    err = uart_cmsdk_rx_ready(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x0u, UART_CMSDK_IRQ_RX, 0x0u)
TEST_CASE(0x1u, UART_CMSDK_IRQ_RX, 0x2u)
TEST_CASE(0x1u, UART_CMSDK_IRQ_TX, 0x1u)
TEST_CASE(0x1u, UART_CMSDK_IRQ_COMBINED, 0x3u)
void test_uart_cmsdk_clear_interrupt(uint32_t state,
                                     enum uart_cmsdk_irq_t irq,
                                     uint32_t expected_intrclear)
{
    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;

    /* Act */
    uart_cmsdk_clear_interrupt(&UART0_CMSDK_DEV_NS, irq);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.intr_reg.intrclear,
            expected_intrclear);
}

TEST_CASE(0x0u, 0x0u, UART_CMSDK_ERR_NOT_INIT)
TEST_CASE(0x1u, 0x1u, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_tx_enable(uint32_t state,
                               uint32_t expected_uart_ctrl,
                               enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;

    /* Act */
    err = uart_cmsdk_tx_enable(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.ctrl,
            expected_uart_ctrl);
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x0u, 0x1u, 0x1u)
TEST_CASE(0x1u, 0x1u, 0x0u)
void test_uart_cmsdk_tx_disable(uint32_t state,
                                uint32_t uart_ctrl,
                                uint32_t expected_uart_ctrl)
{

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;
    regmap.ctrl = uart_ctrl;

    /* Act */
    uart_cmsdk_tx_disable(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.ctrl,
            expected_uart_ctrl);
}

TEST_CASE(0x0u, 0x0u, UART_CMSDK_ERR_NOT_INIT)
TEST_CASE(0x1u, 0x2u, UART_CMSDK_ERR_NONE)
void test_uart_cmsdk_rx_enable(uint32_t state,
                               uint32_t expected_uart_ctrl,
                               enum uart_cmsdk_error_t expected_err)
{
    enum uart_cmsdk_error_t err;

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;

    /* Act */
    err = uart_cmsdk_rx_enable(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.ctrl,
            expected_uart_ctrl);
    TEST_ASSERT_EQUAL(err,
            expected_err);
}

TEST_CASE(0x0u, 0x2u, 0x2u)
TEST_CASE(0x1u, 0x2u, 0x0u)
void test_uart_cmsdk_rx_disable(uint32_t state,
                                uint32_t uart_ctrl,
                                uint32_t expected_uart_ctrl)
{

    /* Prepare */
    UART0_CMSDK_DEV_DATA_NS.state = state;
    regmap.ctrl = uart_ctrl;

    /* Act */
    uart_cmsdk_rx_disable(&UART0_CMSDK_DEV_NS);

    /* Assert */
    TEST_ASSERT_EQUAL_UINT32(regmap.ctrl,
            expected_uart_ctrl);
}
