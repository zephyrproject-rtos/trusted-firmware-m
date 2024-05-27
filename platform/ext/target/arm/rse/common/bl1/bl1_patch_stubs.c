/*
 * Copyright (c) 2023-2024, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Since this gets run at the start of every relocatable function, use it to
 * make sure that the GOT register hasn't been clobbered
 */
void __cyg_profile_func_enter(void *func, void *callsite)
{
}

void __cyg_profile_func_exit(void *func, void *callsite)
{
}
