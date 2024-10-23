/*
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: Copyright The TrustedFirmware-M Contributors
 *
 */

#ifndef __DRIVER_FLASH_RPI_H__
#define __DRIVER_FLASH_RPI_H__

#include "Driver_Flash.h"
#include "hardware/flash.h"
#include <string.h>

#ifndef ARG_UNUSED
#define ARG_UNUSED(arg)  ((void)arg)
#endif

/* Driver version */
#define ARM_FLASH_DRV_VERSION   ARM_DRIVER_VERSION_MAJOR_MINOR(1, 0)

static const ARM_DRIVER_VERSION DriverVersion = {
    ARM_FLASH_API_VERSION,  /* Defined in the CMSIS Flash Driver header file */
    ARM_FLASH_DRV_VERSION
};

/**
 * Data width values for ARM_FLASH_CAPABILITIES::data_width
 * \ref ARM_FLASH_CAPABILITIES
 */
 enum {
    DATA_WIDTH_8BIT   = 0u,
    DATA_WIDTH_16BIT,
    DATA_WIDTH_32BIT,
    DATA_WIDTH_ENUM_SIZE
};

/* Flash Status */
static ARM_FLASH_STATUS FlashStatus = {0, 0, 0};

/**
 * \brief Flash driver capability macro definitions \ref ARM_FLASH_CAPABILITIES
 */
/* Flash Ready event generation capability values */
#define EVENT_READY_NOT_AVAILABLE   (0u)
#define EVENT_READY_AVAILABLE       (1u)

/* Chip erase capability values */
#define CHIP_ERASE_NOT_SUPPORTED    (0u)
#define CHIP_ERASE_SUPPORTED        (1u)

static inline ARM_DRIVER_VERSION ARM_Flash_GetVersion(void)
{
    return DriverVersion;
}

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES DriverCapabilities = {
    EVENT_READY_NOT_AVAILABLE,
    DATA_WIDTH_8BIT,
    CHIP_ERASE_NOT_SUPPORTED
};

/*
 * ARM FLASH device structure
 */
typedef struct rp2350_flash_dev_t{
    ARM_FLASH_INFO* data;               /* FLASH data */
    uint32_t base;                      /* Flash base address, used for flash
                                           reads */
    uint32_t size;                      /* Flash size */
    void (*save_mpu_state)(struct rp2350_flash_dev_t* dev);
                                        /*!< Function to save MPU settings */
    void (*restore_mpu_state)(void);
                                        /*!< Function to restore MPU settings */
} rp2350_flash_dev_t;

/* Driver Capabilities */
static const ARM_FLASH_CAPABILITIES PicoDriverCapabilities = {
    0, /* event_ready */
    0, /* data_width = 0:8-bit, 1:16-bit, 2:32-bit */
    0, /* erase_chip */
    0, /* reserved */
};

static inline ARM_FLASH_CAPABILITIES Pico_Driver_GetCapabilities(void)
{
    return PicoDriverCapabilities;
}

static inline int32_t ARM_Flash_Uninitialize(void)
{
    /* Nothing to be done */
    return ARM_DRIVER_OK;
}

static inline int32_t ARM_Flash_PowerControl(ARM_POWER_STATE state)
{
    switch (state) {
    case ARM_POWER_FULL:
        /* Nothing to be done */
        return ARM_DRIVER_OK;
        break;

    case ARM_POWER_OFF:
    case ARM_POWER_LOW:
    default:
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }
}

static inline ARM_FLASH_STATUS ARM_Flash_GetStatus(void)
{
    return FlashStatus;
}

/*
 * \brief Macro for Pico Flash Driver
 *
 * \param[out] FLASH_DRIVER_NAME  Resulting Driver name
 */
#define RPI_RP2350_FLASH(FLASH_DEV, FLASH_DRIVER_NAME)                          \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_Initialize(                                \
                                            ARM_Flash_SignalEvent_t cb_event) \
{                                                                             \
    ARG_UNUSED(cb_event);                                                     \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_ReadData(uint32_t addr,                    \
                                            void *data,                       \
                                            uint32_t cnt)                     \
{                                                                             \
    if ((addr+cnt) >= FLASH_DEV.size) {                                       \
        return ARM_DRIVER_ERROR_PARAMETER;                                    \
    }                                                                         \
                                                                              \
    memcpy(data, (void *)(addr + FLASH_DEV.base), cnt);                       \
                                                                              \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_ProgramData(uint32_t addr,                 \
                                               const void *data,              \
                                               uint32_t cnt)                  \
{                                                                             \
    if ((addr+cnt) >= FLASH_DEV.size) {                                       \
        return ARM_DRIVER_ERROR_PARAMETER;                                    \
    }                                                                         \
                                                                              \
    if ((cnt < FLASH_DEV.data->program_unit) ||                               \
        (cnt % FLASH_DEV.data->program_unit) ||                               \
        (addr % FLASH_DEV.data->page_size)) {                                 \
        return ARM_DRIVER_ERROR_PARAMETER;                                    \
    }                                                                         \
                                                                              \
    FLASH_DEV.save_mpu_state(&FLASH_DEV);                                     \
                                                                              \
    flash_range_program(addr, data, cnt);                                     \
                                                                              \
    FLASH_DEV.restore_mpu_state();                                            \
                                                                              \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_EraseSector(uint32_t addr)                 \
{                                                                             \
    if (addr >= FLASH_DEV.size) {                                             \
        return ARM_DRIVER_ERROR_PARAMETER;                                    \
    }                                                                         \
                                                                              \
    if (addr % FLASH_DEV.data->sector_size) {                                 \
        return ARM_DRIVER_ERROR_PARAMETER;                                    \
    }                                                                         \
                                                                              \
    FLASH_DEV.save_mpu_state(&FLASH_DEV);                                     \
                                                                              \
    flash_range_erase(addr, FLASH_DEV.data->sector_size);                     \
                                                                              \
    FLASH_DEV.restore_mpu_state();                                            \
                                                                              \
    return ARM_DRIVER_OK;                                                     \
}                                                                             \
                                                                              \
static int32_t FLASH_DRIVER_NAME##_EraseChip(void)                            \
{                                                                             \
    return ARM_DRIVER_ERROR_UNSUPPORTED;                                      \
}                                                                             \
                                                                              \
static ARM_FLASH_INFO * FLASH_DRIVER_NAME##_GetInfo(void)                     \
{                                                                             \
    return FLASH_DEV.data;                                                    \
}                                                                             \
                                                                              \
ARM_DRIVER_FLASH FLASH_DRIVER_NAME = {                                        \
    ARM_Flash_GetVersion,                                                     \
    Pico_Driver_GetCapabilities,                                              \
    FLASH_DRIVER_NAME##_Initialize,                                           \
    ARM_Flash_Uninitialize,                                                   \
    ARM_Flash_PowerControl,                                                   \
    FLASH_DRIVER_NAME##_ReadData,                                             \
    FLASH_DRIVER_NAME##_ProgramData,                                          \
    FLASH_DRIVER_NAME##_EraseSector,                                          \
    FLASH_DRIVER_NAME##_EraseChip,                                            \
    ARM_Flash_GetStatus,                                                      \
    FLASH_DRIVER_NAME##_GetInfo                                               \
};

#endif /* __DRIVER_FLASH_RPI_H__ */
