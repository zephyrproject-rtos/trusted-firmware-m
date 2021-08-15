/*
 * Copyright (c) 2017-2021 Arm Limited. All rights reserved.
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

/*
 * This file is derivative of CMSIS V5.01:
 * Device\_Template_Vendor\Vendor\Device\Include\Device.h
 */

#ifndef __PLATFORM_BASE_ADDRESS_H__
#define __PLATFORM_BASE_ADDRESS_H__

/* Secure Enclave Peripheral base addresses */
#define DIPHDA_ROM_BASE                      (0x00000000U) /* SE ROM Region                     */
#define DIPHDA_CRYPTO_ACCELERATOR_BASE       (0x2F000000U) /* Crypto Accelerator                */
#define DIPHDA_SRAM_BASE                     (0x30000000U) /* SE RAM Region                     */
#define DIPHDA_PERIPHERAL_BASE               (0x50000000U) /* SE Peripheral Region              */
#define DIPHDA_CMSDK_TIMER_0_BASE            (0x50000000U) /* CMSDK Timer 0                     */
#define DIPHDA_CMSDK_TIMER_1_BASE            (0x50001000U) /* CMSDK Timer 1                     */
#define DIPHDA_SEH_0_SENDER_BASE             (0x50003000U) /* SEH0 Sender                       */
#define DIPHDA_HSE_0_RECEIVER_BASE           (0x50004000U) /* HSE1 Receiver                     */
#define DIPHDA_SEH_1_SENDER_BASE             (0x50005000U) /* SEH1 Sender                       */
#define DIPHDA_HSE_1_RECEIVER_BASE           (0x50006000U) /* HSE1 Receiver                     */
/* Not all of the SEESx/ESxSE peripherals will be applicable, depending on the
 * number of external systems present         */
#define DIPHDA_SEES0_0_SENDER_BASE           (0x50010000U) /* SEES0 0 Sender                    */
#define DIPHDA_ES0SE_0_RECEIVER_BASE         (0x50011000U) /* ES0SE 0 Receiver                  */
#define DIPHDA_SEES0_1_SENDER_BASE           (0x50012000U) /* SEES0 1 Sender                    */
#define DIPHDA_ES0SE_1_RECEIVER_BASE         (0x50013000U) /* ES0SE 1 Receiver                  */
#define DIPHDA_SEES1_0_SENDER_BASE           (0x50014000U) /* SEES1 0 Sender                    */
#define DIPHDA_ES1SE_0_RECEIVER_BASE         (0x50015000U) /* ES1SE 0 Receiver                  */
#define DIPHDA_SEES1_1_SENDER_BASE           (0x50016000U) /* SEES1 1 Sender                    */
#define DIPHDA_ES1SE_1_RECEIVER_BASE         (0x50017000U) /* ES1SE 1 Receiver                  */
#define DIPHDA_SEES2_0_SENDER_BASE           (0x50018000U) /* SEES2 0 Sender                    */
#define DIPHDA_ES2SE_0_RECEIVER_BASE         (0x50019000U) /* ES2SE 0 Receiver                  */
#define DIPHDA_SEES2_1_SENDER_BASE           (0x5001A000U) /* SEES2 1 Sender                    */
#define DIPHDA_ES2SE_1_RECEIVER_BASE         (0x5001B000U) /* ES2SE 1 Receiver                  */
#define DIPHDA_SEES3_0_SENDER_BASE           (0x5001C000U) /* SEES3 0 Sender                    */
#define DIPHDA_ES3SE_0_RECEIVER_BASE         (0x5001D000U) /* ES3SE 0 Receiver                  */
#define DIPHDA_SEES3_1_SENDER_BASE           (0x5001E000U) /* SEES3 1 Sender                    */
#define DIPHDA_ES3SE_1_RECEIVER_BASE         (0x5001F000U) /* ES3SE 1 Receiver                  */
#define DIPHDA_SCR_BASE                      (0x50080000U) /* System Control Register           */
#define DIPHDA_WATCHDOG_TIMER_BASE           (0x50081000U) /* Watchdog Timer                    */
#define DIPHDA_SECENCTOP_PPU_BASE            (0x5008D000U) /* SECENCTOP PPU                     */
#define DIPHDA_BASE_SCR_BASE                 (0x5008E000U) /* SE Base System Control Register   */
#define DIPHDA_SOC_WATCHDOG_BASE             (0x5008F000U) /* SoC Watchdog                      */
#define DIPHDA_UART_BASE                     (0x50090000U) /* UART                              */
#define DIPHDA_FIREWALL_BASE                 (0x50200000U) /* SE Firewall                       */
#define DIPHDA_HOST_ACCESS_REGION_BASE       (0x60000000U) /* Host Access Region                */
#define DIPHDA_PPB_BASE                      (0xE0000000U) /* Private Peripheral Bus (PPB)      */
#define DIPHDA_CS_ROM_BASE                   (0xF0000000U) /* SE CS ROM                         */
#define DIPHDA_CTI_BASE                      (0xF0001000U) /* SE CTI                            */

/* Host base addresses from the SE perspective - partial list, only the ones
 * required by the SE are defined here */
#define DIPHDA_HOST_BIR_BASE                 (0x60000000U) /* Boot Instruction Register         */
#define DIPHDA_HOST_SHARED_RAM_BASE          (0x62000000U) /* Shared RAM                        */
#define DIPHDA_HOST_XNVM_BASE                (0x68000000U) /* XNVM                              */
#define DIPHDA_HOST_BASE_SYSTEM_CONTROL_BASE (0x7A010000U) /* Host SCB                          */
#define DIPHDA_HOST_FIREWALL_BASE            (0x7A800000U) /* Host Firewall                     */
#define DIPHDA_HOST_FPGA_SCC_REGISTERS       (0x80000000U) /* FPGA SCC Registers                */
#define DIPHDA_AXI_QSPI_CTRL_REG_BASE        (0x80050000U) /* AXI QSPI Controller               */
#define DIPHDA_SE_SECURE_FLASH_BASE_FVP      (0x80010000U) /* SE Flash                          */

#endif  /* __PLATFORM_BASE_ADDRESS_H__ */
