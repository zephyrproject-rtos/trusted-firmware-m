/*
 * attest_token_test_values.h
 *
 * Copyright (c) 2019, Laurence Lundblade.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef __ATTEST_TOKEN_TEST_VALUES_H__
#define __ATTEST_TOKEN_TEST_VALUES_H__

/**
 * \file attest_token_test_values.h
 *
 * \brief Expected values for test suite.
 *
 * This is a bunch of made up values for hard-coded test cases for
 * attestation tokens.
 *
 * There are four possible test configurations for testing each claim:
 *
 *  1. No checking at all. \c TOKEN_TEST_REQUIRE_XXX is false and
 *  TOKEN_TEST_VALUE_XXX is not given.
 *
 *  2. Check for presence only. \c TOKEN_TEST_REQUIRE_XXX is true and
 *  TOKEN_TEST_VALUE_XXX is not given.
 *
 *  3. Check value if it is present, but it is not required to be
 *  present. \c TOKEN_TEST_REQUIRE_XXX is false and \c
 *  TOKEN_TEST_VALUE_XXX is given.
 *
 *  4. Must be present and of specific value. \c
 *  TOKEN_TEST_REQUIRE_XXX is true and \c TOKEN_TEST_VALUE_XXX is
 *  given.
 *
 * TOKEN_TEST_VALUE_XXX is not given as follows:
 *  - #define text strings as \c NULL
 *  - #define binary strings as \c NULL_Q_USEFUL_BUF_C
 *  - #define the integer value as \c INT32_MAX
 *
 * It is assumed that the expected value for any test will never be
 * any of these.
 *
 * Individual test can also be made to return values that are not
 * fixed at compile time by defining them to be a function and
 * implementing the funciton.  Here are examples for the three types:
 *
 *      struct q_useful_buf_c get_expected_nonce(void);
 *      #define TOKEN_TEST_VALUE_NONCE get_expected_nonce()
 *
 *      const char *get_expected_hw_version(void);
 *      #define TOKEN_TEST_VALUE_HW_VERSION get_expected_hw_version()
 *
 *      uint32_t get_expected_client_id(void);
 *      #define TOKEN_TEST_VALUE_CLIENT_ID get_expected_client_id()
 *
 * The initialization value for byte strings uses a compound literal
 * to create the \c ptr and \c len for a \c struct \c q_useful_buf_c.
 * They are a bit ugly, but they work and setting up this way allows
 * the literal value to be replaced by a function call for dynamic
 * expected values.
 *
 * The first part of the compound literal is the value of the
 * bytes. The second is an integer that is the length, the number of
 * bytes. They length must be the number of bytes in the first.
 */


/* A 32 byte mostly random value. Binary. */
#define TOKEN_TEST_VALUE_NONCE \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_NONCE true


/* A 32 byte mostly random value. Binary. */
#define TOKEN_TEST_VALUE_UEID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x21, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_UEID true

/* A 32 byte mostly random value. Binary. */
#define TOKEN_TEST_VALUE_BOOT_SEED \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x31, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_BOOT_SEED true


/* A text string mostly in EAN 13 format */
#define TOKEN_TEST_VALUE_HW_VERSION "4 003994 155486"
#define TOKEN_TEST_REQUIRE_HW_VERSION true

/* A 32 byte mostly random value. Binary. */
#define TOKEN_TEST_VALUE_IMPLEMENTATION_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x41, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_IMPLEMENTATION_ID true

/* A small unsigned integer */
#define TOKEN_TEST_VALUE_SECURITY_LIFECYCLE  4
#define TOKEN_TEST_REQUIRE_SECURITY_LIFECYCLE true

/* An integer (can be positive or negative */
#define TOKEN_TEST_VALUE_CLIENT_ID  589
#define TOKEN_TEST_REQUIRE_CLIENT_ID true


/* Text string naming the profile definition */
#define TOKEN_TEST_VALUE_PROFILE_DEFINITION  "psa_tfm-profile-1.md"
#define TOKEN_TEST_REQUIRE_PROFILE_DEFINITION true


/* Text string with verification URL or similar */
#define TOKEN_TEST_VALUE_ORIGINATION  \
                        "https://verification.attestationtoken.com"
#define TOKEN_TEST_REQUIRE_ORIGINATION true



/**
 * \c TOKEN_TEST_REQUIRED_NUM_SWC can be either 0, 1, 2 or \c
 * INT32_MAX
 *
 * 0 -- No SW components are required, but if there is 1, its values
 * must compare to SWC1 correctly and if there are 2, the first must
 * compare to SWC1 and the second to SWC2.
 *
 * 1 -- At least one SW component is required and it must compare
 * correctly to SWC1. If a second one is present its values will also
 * be checked.
 *
 * 2 -- Two SW components are required and their values must compare
 * correctly.
 *
 * INT32_MAX -- No checking of the SW components of any sort is
 * performed.
 *
 * Note that attest_token_decode() checks for the presence of the the
 * EAT_CBOR_ARM_LABEL_NO_SW_COMPONENTS CBOR data item for the case of
 * no SW components and gives an error if it is absent.
 */
#define TOKEN_TEST_REQUIRED_NUM_SWC 0

/* Text string */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_TYPE "type library"
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_TYPE true

/* A 32 byte mostly random value. Binary. */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_VAL \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x51, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_VAL true

/* Small unsigned integer */
#define TOKEN_TEST_VALUE_SWC1_EPOCH 2
#define TOKEN_TEST_REQUIRE_SWC1_EPOCH true

/* Text string */
#define TOKEN_TEST_VALUE_SWC1_VERSION "v1.2.4.8"
#define TOKEN_TEST_REQUIRE_SWC1_VERSION true


/* A 32 byte mostly random value. Binary. */
#define TOKEN_TEST_VALUE_SWC1_SIGNER_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x61, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_SWC1_SIGNER_ID true

/* Text string */
#define TOKEN_TEST_VALUE_SWC1_MEASUREMENT_DESC "execute-in-place-rom"
#define TOKEN_TEST_REQUIRE_SWC1_MEASUREMENT_DESC true



/* Text string */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_TYPE "type service"
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_TYPE true


/* A 32 byte mostly random value. Binary. */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_VAL \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x71, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_VAL true

/* Small unsigned integer */
#define TOKEN_TEST_VALUE_SWC2_EPOCH 4
#define TOKEN_TEST_REQUIRE_SWC2_EPOCH true

/* Text string */
#define TOKEN_TEST_VALUE_SWC2_VERSION "v3.9.27"
#define TOKEN_TEST_REQUIRE_SWC2_VERSION true

/* A 32 byte mostly random value. Binary. */
#define TOKEN_TEST_VALUE_SWC2_SIGNER_ID \
    (struct q_useful_buf_c) {\
        (uint8_t[]){ \
            0x81, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, \
            0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08  \
        },\
        32\
    }
#define TOKEN_TEST_REQUIRE_SWC2_SIGNER_ID true

/* Text string */
#define TOKEN_TEST_VALUE_SWC2_MEASUREMENT_DESC "dynamically-loaded"
#define TOKEN_TEST_REQUIRE_SWC2_MEASUREMENT_DESC true



#endif /* __ATTEST_TOKEN_TEST_VALUES_H__ */
