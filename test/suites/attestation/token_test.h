/*
 * token_test.h
 *
 * Copyright (c) 2018-2019, Laurence Lundblade. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __TOKEN_TEST_H__
#define __TOKEN_TEST_H__

/**
 * \file token_test.h
 *
 * \brief Entry points for very basic attestation token tests.
 */


/**
 * \brief Minimal token creation test using short-circuit signature.
 *
 * \return non-zero on failure.
 */
int minimal_test(void);


/**
 * \brief Test token size calculation.
 *
 * \return non-zero on failure.
 */
int minimal_get_size_test(void);

#endif /* __TOKEN_TEST_H__ */
