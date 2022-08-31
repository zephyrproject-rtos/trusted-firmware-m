/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DEVICE_CFG_PLATFORM_H__
#define __DEVICE_CFG_PLATFORM_H__

/* DMA350 */
#define DMA350_DMA0_S
#define DMA350_DMA0_DEV             DMA350_DMA0_DEV_S

#define DMA350_DMA0_CH0_S

#ifdef PLATFORM_SVC_HANDLERS
/* Required for DMA350 checker layer even if some channels are configured NS by
 * default
 */
#define DMA350_DMA0_CH1_S
#endif  /* PLATFORM_SVC_HANDLERS */

#endif  /* __DEVICE_CFG_PLATFORM_H__ */
