/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* boilerplate.*pp contains class(es) to read and store customizable code
   fragments used in realizing the pieces of a test.  It's critical that
   these be customizable in external library files, so that TF-Fuzz can
   exercise TF-A as well as TF-M.  Each TF-x has its own library of text
   pieces, which are read into these data structures upon program initial-
   ization. */

#ifndef BOILERPLATE_HPP
#define BOILERPLATE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


using namespace std;

// Constant mnemonics for entries in array of boilerplate texts:
const int
    // Stuff around the test itself:
        preamble_A = 0,  // setup stuff before PSA calls begin
            /* Strings to be substituted from template:
            $purpose:  The purpose of the test */
        hashing_code = 1,  // code to perform a simple hash of asset data
            /* Strings to be substituted from template:  (none) */
        preamble_B = 2,  // setup stuff before PSA calls begin
            /* Strings to be substituted from template:
            $purpose:  The purpose of the test */
        preamble_C = 3,  // setup stuff before PSA calls begin
            /* Strings to be substituted from template:
            $purpose:  The purpose of the test */
        declare_int = 4,  // declaration for an int
            /* Strings to be substituted from template:
            $var:  The integer thing to declare
            $init:  Its initialization */
        declare_string = 5,  // declaration for a string
            /* Strings to be substituted from template:
            $var:  The string variable to declare
            $init:  Its initialization */
        declare_big_string = 6,  // declaration for a string, sized large
            /* Strings to be substituted from template:
            $var:  The string variable to declare
            $init:  Its initialization */
        declare_size_t = 7,  // declaration for a size_t
            /* Strings to be substituted from template:
            $var:  The integer thing to declare
            $init:  Its initialization */
        declare_policy = 8,  // declaration for a policy (key attributes)
            /* Strings to be substituted from template:
            $var:  The variable to declare */
        declare_policy_algorithm = 9,  // declaration for a
            /* Strings to be substituted from template:
            $var:  The variable to declare */
        declare_policy_lifetime = 10,  // declaration for a
            /* Strings to be substituted from template:
            $var:  The variable to declare */
        declare_policy_type = 11,  // declaration for a
            /* Strings to be substituted from template:
            $var:  The variable to declare */
        declare_policy_usage = 12,  // declaration for a
            /* Strings to be substituted from template:
            $var:  The variable to declare */
        declare_key = 13,  // declaration for a
            /* Strings to be substituted from template:
            $var:  The variable to declare */
        declare_generic = 14,  // declaration for a some other type
            /* Strings to be substituted from template:
            $type:  The type to declare the variable to
            $var:  The string variable to declare
            $init:  Its initialization */
        test_log = 15,  // print a message to the test log
            /* Strings to be substituted from template:
            $message:  What to print. */
        teardown_sst = 16,  // call to delete SST resources after test completes
            /* Strings to be substituted from template:
              $uid:  (Exactly that) */
        teardown_sst_check = 17,  // boilerplate code to check success of previous;
            /* No strings to be substituted from template */
        teardown_key = 18,  // call to delete crypto keys after test completes
            /* Strings to be substituted from template:
              $handle:  The handle to the key */
        teardown_key_check = 19,  // boilerplate code to check success of previous;
            /* No strings to be substituted from template */
        closeout = 20,  // final code to close out the test.
            // No strings to substitute.
        sst_pass_string = 21,  // passing SST expected result
        sst_fail_removed = 22,  // SST expected result from having deleted the asset
    // SST calls:
        set_sst_call = 23,  // boilerplate for setting up an SST asset;
            /* Strings to be substituted from template:
              $op:  (for comment) either "Creating" or "Resetting"
              $description:  its "human name" if given, or "UID=..." if not
              $data_source:  either first ~10 chars of data or file path
              $uid:  (Exactly that)
              $length:  Data length
              $data:  Pointer to data (C string)
              $flags:  SST-asset creation flags */
        set_sst_check = 24,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status string */
        get_sst_call = 25,  // boilerplate for retrieving data from an SST asset;
            /* Strings to be substituted from template:
              $uid:  (Exactly that)
              $offset:  Start positon in the "file"  TO DO:  ADD OFFSET
              $length:  Data length
              $act_data:  Pointer to actual data (C string) */
        get_sst_check = 26,  // boilerplate code to check call result only;
            /* Strings to be substituted from template:
              $expect:  Expected return status string */
        get_sst_check_all = 27,  // boilerplate code to check call result and data;
            /* Strings to be substituted from template:
              $expect:  Expected return status
              $exp_data:  Expected read data
              $act_data:  Actual read data
              $length:  Data length */
        get_sst_hash = 28,  // boilerplate code to invoke hasher;
            /* Strings to be substituted from template:
              $act_data_var:  Actual read data, to be hashed
              $hash_var:  Hash-result variable */
        remove_sst = 29,  // boilerplate for removing an SST asset;
            /* Strings to be substituted from template:
              $uid:  (Exactly that) */
        remove_sst_check = 30,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
    // Crypto-key-policy calls:
        init_policy = 31,  // initialize a policy (key attributes);
            /* Strings to be substituted from template:
              $policy:  The name of the policy */
        reset_policy = 32,  // reset a policy (key attributes);
            /* Strings to be substituted from template:
              $policy:  The name of the policy */
        add_policy_usage = 33,  // add a usage flag to a policy;
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $flag:  usage-flag name */
        set_policy_lifetime = 34,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $life:  Lifetime -- volatile or persistent */
        set_policy_size = 35,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $size:  The key size, in bits */
        set_policy_type = 36,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $type:  The key type -- RSA pair, AES, etc. */
        set_policy_algorithm = 37,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $algorithm:  The algorithm -- AEAD, sign, hash mask, etc. */
        set_policy_usage = 38,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $usage:  The usage -- encrypt, decrypt, export, copy, etc. */
        get_policy_lifetime = 39,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $life:  Lifetime -- volatile or persistent */
        get_policy_lifetime_print = 40,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $life:  Lifetime -- volatile or persistent */
        get_policy_size = 41,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $size:  The key size, in bits */
        get_policy_type = 42,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $type:  The key type -- RSA pair, AES, etc. */
        get_policy_type_print = 43,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $type:  The key type -- RSA pair, AES, etc. */
        get_policy_algorithm = 44,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $algorithm:  The algorithm -- AEAD, sign, hash mask, etc. */
        get_policy_algorithm_print = 45,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $algorithm:  The algorithm -- AEAD, sign, hash mask, etc. */
        get_policy_usage = 46,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $usage:  The usage -- encrypt, decrypt, export, copy, etc. */
        print_policy_usage = 47,
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $usage:  The usage -- encrypt, decrypt, export, copy, etc. */
        get_policy = 48,  // read the policy from a key;
            /* Strings to be substituted from template:
              $key:  The key whose policy we want to read
              $policy:  The policy variable to read into */
        get_policy_check = 49,  // success check fpr previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
    // Crypto-key-related calls:
        generate_key = 50,  // generate key from scratch;
            /* Strings to be substituted from template:
              $key:  The key to generate
              $policy:  The name of the policy */
        generate_key_check = 51,  // success check for previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
        create_key = 52,  // Create key from policy and key data;
            /* Strings to be substituted from template:
              $policy:  The name of the policy
              $data:  The key data
              $length:  The key-data length -- sizeof() */
        create_key_check = 53,  // success check for previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
        copy_key = 54,  // Copy key data from one key to another; different policy;
            /* Strings to be substituted from template:
              $master:  The source key-handle variable
              $policy:  The name of the policy
              $copy:  The copy key-handle variable */
        copy_key_check = 55,  // success check for previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
        compare_data = 56,  // compare data blocks, such as key data
            /* Strings to be substituted from template:
              $exp_data:  Expected read data
              $act_data:  Actual read data
              $length:  Data length */
        read_key_data = 57,  // read key data;
            /* Strings to be substituted from template:
              $key:  The key to read
              $data:  Where to put the data
              $length:  Length of the data buffer
              $act_size:  The actual size of the key data retrieved */
        read_key_data_check = 58,  // success check for previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
        remove_key = 59,  // boilerplate for creating a key;
            /* Strings to be substituted from template:
              $handle:  The handle to the key */
        remove_key_check = 60,  // boilerplate code to check success of previous;
            /* Strings to be substituted from template:
              $expect:  Expected return status */
    // Just an anchor at the end, for sizing the string array (if needed):
        n_boilerplate_texts = 61;


class boilerplate
{
public:  // (More pain than it's worth to create getters and setters)
    // Data members:
    vector<string> bplate_string;
    string cpp_string;  // used to stuff bplate_string
    // Methods:
    void stuff_boilerplate_strings (void);
    boilerplate (void);
};


#endif  // #ifndef BOILERPLATE_HPP
