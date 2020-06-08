/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef STRING_OPS_HPP
#define STRING_OPS_HPP

#include <cstddef>
#include <string>

using namespace std;

// Replace first occurrence of find_str within orig of replace_str:
size_t find_replace_1st (const string &find_str, const string &replace_str,
                         string &orig);

// Replace all occurrences of find_str in "this" string, with replace_str:
size_t find_replace_all (const string &find_str, const string &replace_str,
                         string &orig);

/* In both of the above string-replacement functions, the return value is start
   offset to the (last) occurrence of "find_str" within "orig." */

/* formalize() turns an abbreviated, "human" name for a PSA setting into its
   presumed official name.  Examples:
   "export" with prefix "PSA_KEY_USAGE_" becomes "PSA_KEY_USAGE_EXPORT".
   "generic_error" with prefix "PSA_ERROR_" becomes "PSA_ERROR_GENERIC_ERROR".
   "PSA_ERROR_INVALID_ARGUMENT" stays unchanged as "PSA_ERROR_INVALID_ARGUMENT".
*/
string formalize (string input, string prefix);

/* string_or_hex() takes a string (e.g., describing a key's "material" -- data),
   and:
   *  If it "appears to be" alphanumeric, then returns it unaltered.
   *  Otherwise, if it "appears to be binary data, it returns a new string of
      it as hex digits.
   *  clump_size is how many bytes to clump together between spaces */
string string_or_hex (string input, int clump_size);

/* binary_from_hex() takes a string of hex characters, and returns a string
   containing those raw byte values (i.e., not human-readable, and not really
   a string in the usual sense of the word).  In the future, it would be good
   to address binary data better. */
string binary_from_hex (string input);

#endif  // #ifndef STRING_OPS_HPP
