/*
 * Copyright (c) 2017 ARM Limited
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

#ifndef REGION_LIMITS_H
#define REGION_LIMITS_H

#define TOTAL_ROM_SIZE (0x00400000) /* 4 MB */
#define TOTAL_RAM_SIZE (0x00200000) /* 2 MB */

/* Use SRAM1 memory to store Code data */
#define S_ROM_ALIAS_BASE  (0x10000000)
#define NS_ROM_ALIAS_BASE (0x00000000)

/* Use SRAM2 memory to store RW data as Internal RAM has issues in the FPGA */
#define S_RAM_ALIAS_BASE  (0x38000000)
#define NS_RAM_ALIAS_BASE (0x28000000)

#endif /*REGION_LIMITS_H*/
