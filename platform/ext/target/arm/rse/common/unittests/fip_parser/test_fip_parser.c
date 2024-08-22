/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include "Driver_Flash.h"
#include "fip_parser.h"
#include "tfm_plat_defs.h"

#include "unity.h"

#define TEST_UUID                                                              \
    {                                                                          \
        .time_low = {0x01, 0x4f, 0x53, 0x30}, .time_mid = {0x24, 0x57},        \
        .time_hi_and_version = {0xac, 0x4a},                                   \
        .clock_seq_hi_and_reserved = 0x4a, .clock_seq_low = 0xbc, .node = {    \
            0x75,                                                              \
            0xc2,                                                              \
            0xab,                                                              \
            0xf3,                                                              \
            0xe9,                                                              \
            0xd4                                                               \
        }                                                                      \
    }

static struct _unittest_flash_context_t {
    char *filepath;
    int32_t fd;
    bool is_initialized;
} UNITTEST_FLASH_CONTEXT;

static ARM_FLASH_CAPABILITIES UNITTEST_Flash_GetCapabilities(void)
{
    static const ARM_FLASH_CAPABILITIES Caps = {
        .data_width = sizeof(uint8_t) - 1,
        .erase_chip = 0u,
        .event_ready = 0u,
        .reserved = 0u,
    };

    return Caps;
}

static int32_t UNITTEST_Flash_Initialize(ARM_Flash_SignalEvent_t cb_event)
{
    (void)(cb_event);

    UNITTEST_FLASH_CONTEXT.fd = open(UNITTEST_FLASH_CONTEXT.filepath, O_RDONLY);
    if (UNITTEST_FLASH_CONTEXT.fd == -1) {
        return ARM_DRIVER_ERROR;
    }

    UNITTEST_FLASH_CONTEXT.is_initialized = true;

    return ARM_DRIVER_OK;
}

static int32_t UNITTEST_Flash_Uninitialize(void)
{
    close(UNITTEST_FLASH_CONTEXT.fd);
    UNITTEST_FLASH_CONTEXT.fd = -1;

    UNITTEST_FLASH_CONTEXT.is_initialized = false;

    return ARM_DRIVER_OK;
}

static int32_t UNITTEST_Flash_ReadData(uint32_t addr, void *data, uint32_t cnt)
{
    lseek(UNITTEST_FLASH_CONTEXT.fd, addr, SEEK_SET);
    return read(UNITTEST_FLASH_CONTEXT.fd, data, cnt);
}

ARM_DRIVER_FLASH UNITTEST_FLASH_DEV = {
    .GetCapabilities = UNITTEST_Flash_GetCapabilities,
    .Initialize = UNITTEST_Flash_Initialize,
    .Uninitialize = UNITTEST_Flash_Uninitialize,
    .ReadData = UNITTEST_Flash_ReadData};

TEST_CASE("/files/empty.fip", UINT32_MAX,
          TFM_PLAT_ERR_FIP_TOC_HEADER_INVALID_READ)
TEST_CASE("/files/invalid_toc_header.fip", UINT32_MAX,
          TFM_PLAT_ERR_FIP_TOC_HEADER_INVALID_NAME)
TEST_CASE("/files/valid_toc_header.fip", UINT32_MAX,
          TFM_PLAT_ERR_FIP_TOC_ENTRY_INVALID_READ)
TEST_CASE("/files/valid_toc_header.fip", 0x0,
          TFM_PLAT_ERR_FIP_TOC_ENTRY_OVERFLOW)
TEST_CASE("/files/invalid_toc_entry_uuid.fip", UINT32_MAX,
          TFM_PLAT_ERR_GPT_TOC_ENTRY_NOT_FOUND)
TEST_CASE("/files/invalid_toc_entry_size.fip", UINT32_MAX,
          TFM_PLAT_ERR_FIP_TOC_ENTRY_INVALID_SIZE)
TEST_CASE("/files/valid_toc_entry.fip", UINT32_MAX, TFM_PLAT_ERR_SUCCESS)
void test_fip_get_entry_by_uuid(const char *path, uint32_t atu_slot_size,
                                enum tfm_plat_err_t expected_err)
{
    enum tfm_plat_err_t plat_err;
    char filepath[PATH_MAX] = __FILE__;
    uint32_t fip_offset;
    uint64_t offset;
    size_t size;
    uuid_t uuid = TEST_UUID;

    /* Prepare */
    fip_offset = 0;

    strcpy(strrchr(filepath, '/'), path);
    UNITTEST_FLASH_CONTEXT.filepath = filepath;
    UNITTEST_FLASH_CONTEXT.is_initialized = false;
    UNITTEST_FLASH_CONTEXT.fd = -1;

    /* Act */
    UNITTEST_FLASH_DEV.Initialize(NULL);
    plat_err = fip_get_entry_by_uuid(&UNITTEST_FLASH_DEV, fip_offset,
                                     atu_slot_size, uuid, &offset, &size);
    UNITTEST_FLASH_DEV.Uninitialize();

    /* Assert */
    TEST_ASSERT_EQUAL(plat_err, expected_err);
}
