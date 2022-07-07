/*
 * Copyright (c) 2019-2022 Arm Limited
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

#ifndef __DRIVER_PPC_COMMON_H__
#define __DRIVER_PPC_COMMON_H__

#define ARM_DRIVER_PPC(PPC_NAME, PPC_DEV) \
static int32_t PPC_NAME##_Initialize(void) \
{ \
    enum ppc_corstone310_error_t ret; \
 \
    ret = ppc_corstone310_init(&PPC_DEV); \
 \
    return error_trans(ret); \
} \
 \
static int32_t PPC_NAME##_Uninitialize(void) \
{ \
    /* Nothing to do */ \
    return ARM_DRIVER_OK; \
} \
 \
static int32_t \
PPC_NAME##_ConfigPeriph(uint8_t periph, \
                        ARM_PPC_SecAttr sec_attr, \
                        ARM_PPC_PrivAttr priv_attr) \
{ \
    enum ppc_corstone310_error_t ret_priv, ret_sec; \
 \
    ret_priv = ppc_corstone310_config_privilege(&PPC_DEV, periph, \
                        (enum ppc_corstone310_sec_attr_t)sec_attr, \
                        (enum ppc_corstone310_priv_attr_t)priv_attr); \
 \
    ret_sec = ppc_corstone310_config_security(&PPC_DEV, periph, \
                                (enum ppc_corstone310_sec_attr_t)sec_attr); \
 \
   return error_trans(ret_priv | ret_sec); \
} \
 \
static uint32_t PPC_NAME##_IsPeriphPrivOnly(uint8_t periph) \
{ \
    return (uint32_t)ppc_corstone310_is_periph_priv_only(&PPC_DEV, periph); \
} \
 \
static uint32_t PPC_NAME##_IsPeriphSecure(uint8_t periph) \
{ \
    return (uint32_t)ppc_corstone310_is_periph_secure(&PPC_DEV, periph); \
} \
 \
static int32_t PPC_NAME##_EnableInterrupt(void) \
{ \
    enum ppc_corstone310_error_t ret; \
 \
    ret = ppc_corstone310_irq_enable(&PPC_DEV); \
 \
   return error_trans(ret); \
} \
 \
static void PPC_NAME##_DisableInterrupt(void) \
{ \
    ppc_corstone310_irq_disable(&PPC_DEV); \
} \
 \
static void PPC_NAME##_ClearInterrupt(void) \
{ \
    ppc_corstone310_clear_irq(&PPC_DEV); \
} \
 \
static uint32_t PPC_NAME##_InterruptState(void) \
{ \
    return (uint32_t)ppc_corstone310_irq_state(&PPC_DEV); \
} \
 \
/* PPC Corstone310 PPC_NAME Driver CMSIS access structure */ \
ARM_DRIVER_PPC Driver_##PPC_NAME = { \
    .GetVersion        = ARM_PPC_GetVersion, \
    .Initialize        = PPC_NAME##_Initialize, \
    .Uninitialize      = PPC_NAME##_Uninitialize, \
    .ConfigPeriph      = PPC_NAME##_ConfigPeriph, \
    .IsPeriphPrivOnly  = PPC_NAME##_IsPeriphPrivOnly, \
    .IsPeriphSecure    = PPC_NAME##_IsPeriphSecure, \
    .EnableInterrupt   = PPC_NAME##_EnableInterrupt, \
    .DisableInterrupt  = PPC_NAME##_DisableInterrupt, \
    .ClearInterrupt    = PPC_NAME##_ClearInterrupt, \
    .InterruptState    = PPC_NAME##_InterruptState \
};
#endif  /* __DRIVER_PPC_COMMON_H */
