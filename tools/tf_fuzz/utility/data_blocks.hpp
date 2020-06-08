/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <string>

/* These classes "cut down the clutter" by grouping together related data and
   associated methods (most importantly their constructors) used in template_
   line, psa_call, psa_asset (etc.). */

#ifndef DATA_BLOCKS_HPP
#define DATA_BLOCKS_HPP

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.  However these in particular are mostly axiomatic:  Not
   dependent upon other classes. */


using namespace std;


/**********************************************************************************
  Class expect_info is all about expected data and expected pass/fail information.
  The members are therefore broken down with prefixes pf_ (for pass/fail) or
  data_.  Pass/fail, is broadly:
  *  "Pass" == the test passes
  *  "Specified" == some specified failure (e.g., no such asset)
  *  "Nothing" == no expectation
  Expected data refers to psa-asset data values, generally after reading them.
  Currently, they are limited to character strings, but that will probably be
  generalized in the future.
**********************************************************************************/

class expect_info
{
public:
    // Data members:
        // Expected-result info:
        bool pf_nothing;  // true to not generate results-check(s)
        bool pf_pass;  // if !expect.pf_nothing, then pass is expected
        bool pf_fail;  // if "expect fail" was specified
        bool pf_specified;
            /* if !pf_nothing && !pf_pass, then
               true == expected result was specified
               false == tf_fuzz must model expected result, and
               pf_result_string is the expected result */
        string pf_result_string;
        bool data_specified;  // (literal expected data specified)
        string data;  // what test template expects data from reading an asset to be
        int n_exp_vars;  // how many check-value variables have been created
        bool data_var_specified;  // check against a variable
        string data_var;  // name of variable containing expected data
        bool pf_info_incomplete;
            /* In parsing the template, the expect information comes later than the
               rest of the call info.  This flag tells us to fill in the pass/fail
               expect info when it comes available. */
        bool expected_results_saved;
            /* This indicates whether expected results have or have not already been
               copied to this call.  It's a "one-shot," so to speak, to copy only
               once when results are known good.  Since calls can be inserted into
               earlier points in the call sequence (not always appended), the call
               sequence has to be gone over for this process multiple times. */
    // Methods:
        expect_info (void);  // (default constructor)
        ~expect_info (void);  // (destructor)
        void set_pf_pass (void);
        void set_pf_fail (void);
        void set_pf_nothing (void);
        void set_pf_error (string error);
        void copy_expect_to_call (psa_call *the_call);

protected:
    // Data members:
        bool data_matches_asset;
            /* true if template specifies expected data, and that expected data
               agrees with that in the asset */
};


/**********************************************************************************
  Class set_data_info addresses PSA-asset data values as affected, directly or
  indirctly/implicitly, by the template-line content.  "Directly," that is, by
  virtue of the template line stating verbatim what to set data to, or indirectly
  by virtue of telling TF-Fuzz to create random data for it.
**********************************************************************************/

class set_data_info
{
public:
    // Data members:
        bool string_specified;
            // true if a string of data is specified in template file
        bool random_data;  // true to generate random data for the asset
        bool file_specified;  // true if a file of expected data was specified
        bool literal_data_not_file;
            // true to use data strings rather than files as data source
        int n_set_vars;  // how many implicit set variables have been created
        string file_path;  // path to file, if specified
        string flags_string;
            // creation flags, nominally for SST but have to be in a vector of base-class
        uint32_t data_offset;  // offset into asset data
    // Methods:
        set_data_info (void);  // (default constructor)
        ~set_data_info (void);  // (destructor)
        void set (string set_val);
        void set_calculated (string set_val);
        void randomize (void);
        string get (void);
        bool set_file (string file_name);

protected:
    // Data members:
        string data;  // String describing asset data.
    // Methods:
        string rand_creation_flags (void);
};


/**********************************************************************************
  Class asset_name_id_info groups together and acts upon all information related to the
  human names (as reflected in the code variable names, etc.) for PSA assets.
**********************************************************************************/

class asset_name_id_info
{
public:
    // Data members (not much value in "hiding" these behind getters)
        psa_asset *the_asset;
        psa_asset_type asset_type;  // SST vs. key vs. policy (etc.)
        bool id_n_not_name;  // true to create a PSA asset by ID
        bool name_specified;  // true iff template supplied human name
        bool id_n_specified;  // true iff template supplied ID #
        vector<string> asset_name_vector;
        vector<int> asset_id_n_vector;
        long asset_ser_no;  // unique ID for psa asset needed to find data string
        asset_search how_asset_found;
        uint64_t id_n;  // asset ID# (e.g., SST UID).
            /* Note:  This is just a holder to pass ID from template-line to call.  The
               IDs for a given template line are in asset_info.asset_id_n_vector. */
    // Methods:
        asset_name_id_info (void);  // (default constructor)
        ~asset_name_id_info (void);  // (destructor)
        void set_name (string set_val);
        void set_calc_name (string set_val);
        void set_just_name (string set_val);
        string get_name (void);
        void set_id_n (string set_val);
        void set_id_n (uint64_t set_val);
        string make_id_n_based_name (uint64_t id_n);
            // create UID-based asset name

protected:
    // Data members:
        string asset_name;  // parsed from template, assigned to psa_asset object
};


/**********************************************************************************
  Class key_policy_info collects together the aspects of a Crypto key attributes
  ("policies").  These include aspects that can affect TF-Fuzz's test-generation.
**********************************************************************************/

class key_policy_info
{
public:
    // Data members:
        // Digested info:
        bool get_policy_from_key;
            /* if true, then we must get policy info from a stated key;  the asset
               here is a key that uses the policy, and not the policy itself. */
        bool implicit_policy;
            /* if true, then the key was defined with policy specifications, but not
               a named policy, meaning that we have to create an implicit policy. */
        bool copy_key;  // true to indicate copying one key to another
        bool exportable;   // key data can be exported (viewed - fail exports if not).
        bool copyable;     // can be copied (fail key-copies if not).
        bool can_encrypt;  // OK for encryption (fail other uses).
        bool can_decrypt;  // OK for decryption (fail other uses).
        bool can_sign;     // OK for signing (fail other operations).
        bool can_verify;   // OK for verifing a message signature (fail other uses).
        bool derivable;    // OK for derive other keys (fail other uses).
        bool persistent;   // must be deleted at the end of test.
        string usage_string;
            /* This string is set to a PSA_KEY_USAGE_* value in the template
               immediately prior to making define_call<add_policy_usage_call>.
               The copy_template_to_call() therein sets the corresponding string
               in the call, and that is copied into the code in the fill_in_command()
               invocation. */
        string print_usage_true_string;
            /* For printing out policy usage, this states how to describe the usage
               if it can be used this way.  This is managed similarly with, and used
               in conjunction with usage_string above.  NOTE:  THIS ALSO SERVES AS AN
               INDICATOR WHETHER OR NOT TO PRINT ON A GET-USAGE CALL.  "" means not
               to print. */
        string print_usage_false_string;
            /* Also for printing out policy usage, this is how to describe usage if
               it cannot be used this way. */
        string key_type;   // AES, DES, RSA pair, DS public, etc.
        string key_algorithm;
        int n_bits;
           // for get_key_info call (possibly others) exected key size in bits
        string handle_str; // the text name of the key's "handle"
        string key_data;   // the key data as best we can know it.
        string asset_2_name;
            // if there's a 2nd asset, such as policy on key call, this is its name
        string asset_3_name;  // if there's a 3rd asset, then this is its name

    // Methods:
        key_policy_info (void);  // (default constructor)
        ~key_policy_info (void);  // (destructor)


protected:
    // Data members:
        bool data_matches_asset;
            /* true if template specifies expected data, and that expected data
               agrees with that in the asset */
};



#endif // DATA_BLOCKS_HPP

