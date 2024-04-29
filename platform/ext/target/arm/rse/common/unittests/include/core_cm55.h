/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CORE_CM55_H__
#define __CORE_CM55_H__

/* This file only satisfies the inclusion of CMSIS headers
 * Use UNIT_TEST_COMPILE_DEFS to redefine macros on a per-test basis
 */

#ifndef __DMB
#define __DMB()
#endif

#ifndef __DSB
#define __DSB()
#endif

#ifndef __ISB
#define __ISB()
#endif

#ifndef __NOP
#define __NOP()
#endif

#ifndef __WFI
#define __WFI()
#endif

#ifndef __COMPILER_BARRIER
#define __COMPILER_BARRIER()
#endif

#ifndef __set_MSPLIM
#define __set_MSPLIM(x)
#endif

#ifndef __set_MSP
#define __set_MSP(x)
#endif

#ifndef __IO
#define __IO
#endif

#ifndef __O
#define __O
#endif

#ifndef __I
#define __I
#endif

#ifndef __IOM
#define __IOM
#endif

#ifndef __OM
#define __OM
#endif

#ifndef __IM
#define __IM
#endif

#include "cmsis_compiler.h"

#endif /* __CORE_CM55_H__ */
