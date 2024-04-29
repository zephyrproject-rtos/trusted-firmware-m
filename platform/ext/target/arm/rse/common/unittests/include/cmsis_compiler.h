/*
 * Copyright (c) 2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CMSIS_COMPILER_H
#define __CMSIS_COMPILER_H

/* This file satisfies the inclusion of CMSIS headers
 * Use UNIT_TEST_COMPILE_DEFS to redefine macros on a per-test basis
 */

#ifndef __ASM
#define __ASM
#endif
#ifndef __INLINE
#define __INLINE inline
#endif
#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif
#ifndef __STATIC_FORCEINLINE
#define __STATIC_FORCEINLINE static inline
#endif
#ifndef __NO_RETURN
#define __NO_RETURN
#endif
#ifndef __USED
#define __USED
#endif
#ifndef __WEAK
#define __WEAK
#endif
#ifndef __PACKED
#define __PACKED
#endif
#ifndef __PACKED_STRUCT
#define __PACKED_STRUCT struct
#endif
#ifndef __PACKED_UNION
#define __PACKED_UNION union
#endif
#ifndef __ALIGNED
#define __ALIGNED(x)
#endif
#ifndef __RESTRICT
#define __RESTRICT restrict
#endif
#ifndef __COMPILER_BARRIER
#define __COMPILER_BARRIER()
#endif
#ifndef __NO_INIT
#define __NO_INIT
#endif
#ifndef __ALIAS
#define __ALIAS(x)
#endif

#endif /* __CMSIS_COMPILER_H */
