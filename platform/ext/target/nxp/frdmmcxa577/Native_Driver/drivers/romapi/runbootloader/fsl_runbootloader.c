/*
 * Copyright 2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_runbootloader.h"

/*! @brief Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.runBootloader"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BOOTLOADER_API_TREE_POINTER ((bootloader_tree_t *)0x1303d800u)

/*!
 * @brief Root of the bootloader API tree.
 *
 * An instance of this struct resides in read-only memory in the bootloader. It
 * provides a user application access to APIs exported by the bootloader.
 *
 * @note The order of existing fields must not be changed.
 */
typedef struct BootloaderTree
{
    void (*runBootloader)(void *arg); /*!< Function to start the bootloader executing.*/
    uint32_t flashDriver;             /*!< Flash driver API.*/
    uint32_t reserved1;               /*!< reserved */
    uint32_t nbootDriver;             /*!< NBOOT driver */
    uint32_t flexspiNorDriver;        /*!< FlexSPI NOR FLASH Driver API.*/
    uint32_t lpspiFlashDriver;        /*!< LPSPI Flash driver API.*/
    uint32_t version;                 /*!< Bootloader version number.*/
    const char *copyright;            /*!< Copyright string.*/
} bootloader_tree_t;

/*******************************************************************************
 * API
 ******************************************************************************/

void ROMAPI_RunBootloader(void *arg)
{
    assert(BOOTLOADER_API_TREE_POINTER);
    BOOTLOADER_API_TREE_POINTER->runBootloader(arg);
}
