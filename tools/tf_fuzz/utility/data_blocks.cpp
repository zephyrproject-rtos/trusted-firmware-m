/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* These classes "cut down the clutter" by grouping together related data and
   associated methods (most importantly their constructors) used in template_
   line, psa_call, psa_asset (etc.). */

#include <string>
#include <vector>
#include <cstdint>

#include "class_forwards.hpp"

#include "boilerplate.hpp"
#include "randomization.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "string_ops.hpp"
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "crypto_asset.hpp"
#include "find_or_create_asset.hpp"
#include "psa_call.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"



/**********************************************************************************
   Methods of class expect_info follow:
**********************************************************************************/

expect_info::expect_info (void)  // (default constructor)
{
    pf_nothing = false;  // by default, TF-Fuzz provides expected results
    pf_pass = pf_fail = pf_specified = false;
    pf_result_string.assign ("");  data.assign ("");
    data_var_specified = false;
    data_var.assign ("");  // name of expected-data variable
    data_specified = false;
    data_matches_asset = false;
    data.assign ("");
    pf_info_incomplete = true;
    n_exp_vars = -1;  // so the first reference is 0 (no suffix), then _1, _2, ...
    expected_results_saved = false;
}
expect_info::~expect_info (void)  // (destructor)
{}

void expect_info::set_pf_pass (void)
{
    pf_pass = true;
    pf_fail = pf_nothing = pf_specified = false;
    pf_result_string = "";
}

void expect_info::set_pf_fail (void)
{
    pf_fail = true;
    pf_pass = pf_nothing = pf_specified = false;
    pf_result_string = "";
}

void expect_info::set_pf_nothing (void)
{
    pf_nothing = true;
    pf_fail = pf_pass = pf_specified = false;
    pf_result_string = "";
}

void expect_info::set_pf_error (string error)
{
    pf_specified = true;
    pf_result_string.assign (error);  // just default "guess," to be filled in
    pf_pass = pf_fail = pf_nothing = false;
}

/* The expected pass/fail results are not available from the parser until the call has
   already been created.  The flag, pf_info_incomplete, that indicates whether or not
   the "expects" information has been filled in.  If not, fill it in from the template,
   once that info has been parsed. */
void expect_info::copy_expect_to_call (psa_call *the_call)
{
    the_call->exp_data.pf_nothing = pf_nothing;
    the_call->exp_data.pf_pass = pf_pass;
    the_call->exp_data.pf_fail = pf_fail;
    the_call->exp_data.pf_specified = pf_specified;
    the_call->exp_data.pf_result_string = pf_result_string;
    the_call->exp_data.expected_results_saved = true;
    the_call->exp_data.pf_info_incomplete = false;
}

/**********************************************************************************
   End of methods of class expect_info.
**********************************************************************************/


/**********************************************************************************
   Class set_data_info methods regarding setting and getting asset-data values:
**********************************************************************************/

string set_data_info::rand_creation_flags (void)
{
    return ((rand() % 2) == 1)?
        "PSA_STORAGE_FLAG_WRITE_ONCE" : "PSA_STORAGE_FLAG_NONE";

    /* TODO:  There are also PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION and
              PSA_STORAGE_FLAG_NO_CONFIDENTIALITY, but they don't seem to appear
              in any test suites, so it's iffy as to whether they really exist.
              We'll not routinely initialize to them, for now at least, but if
              we want to enable them, then uncomment the following:
    string result = "";
    const int most_flags = 2,
    int n_flags = (rand() % most_flags);

    for (int i = 0;  i < ;  i < n_flags;  ++i) {
        switch (rand() % 4) {
            case 0:
                result += "PSA_STORAGE_FLAG_NONE";
                break;
            case 1:
                result += "PSA_STORAGE_FLAG_WRITE_ONCE";
                break;
            case 2:
                result += "PSA_STORAGE_FLAG_NO_REPLAY_PROTECTION";
                break;
            case 3:
                result += "PSA_STORAGE_FLAG_NO_CONFIDENTIALITY";
                break;
        }
        if (i < n_flags-1)
            result += " | ";
    }
    if (result == "") result = "PSA_STORAGE_FLAG_NONE";
*/
}

set_data_info::set_data_info (void)  // (default constructor)
{
    literal_data_not_file = true;  // currently, not using files as data sources
    string_specified = false;
    data.assign ("");
    random_data = false;
    file_specified = false;
    file_path.assign ("");
    n_set_vars = -1;  // so the first reference is 0 (no suffix), then _1, _2, ...
    data_offset = 0;
    flags_string = rand_creation_flags();
}
set_data_info::~set_data_info (void)  // (destructor)
{}

/* set() establishes:
   *  An asset's data value from a template line (e.g., set sst snort data "data
      value"), and
   *  *That* such a value was directly specified, as opposed to no data value having
      been specified, or a random data value being requested.
   Arguably, this method "has side effects," in that it not only sets a value, but
   also "takes notes" about where that value came from.
*/
void set_data_info::set (string set_val)
{
    literal_data_not_file = true;  // currently, not using files as data sources
    string_specified = true;
    data.assign (set_val);
}

/* set_calculated() establishes:
   *  An asset's data value as *not* taken from a template line, and
   *  *That* such a value was not directly specified in any template line, such as
      if a random data value being requested.
   Arguably, this method "has side effects," in that it not only sets a value, but
   also "takes notes" about where that value came from.
*/
void set_data_info::set_calculated (string set_val)
{
    literal_data_not_file = true;  // currently, not using files as data sources
    string_specified = false;
    data.assign (set_val);
}

/* randomize() establishes:
   *  An asset's data value as *not* taken from a template line, and
   *  *That* such a value was randomized.
   Arguably, this method "has side effects," in that it not only sets a value, but
   also "takes notes" about where that value came from.
*/
void set_data_info::randomize (void)
{
    gibberish gib;
    char gib_buff[4096];  // spew gibberish into here
    int rand_data_length = 0;

    string_specified = false;
    random_data = true;
    literal_data_not_file = true;
    rand_data_length = 40 + (rand() % 256);
        /* Note:  Multiple assets do get different random data */
    gib.sentence (gib_buff, gib_buff + rand_data_length - 1);
    data = gib_buff;
}

/* Getter for protected member, data.  Protected so that it can only be set by
   set() or set_calculated(), above, to establish not only its value but
   how it came about. */
string set_data_info::get (void)
{
    return data;
}

/* Currently, files as data sources aren't used, so this whole method is not "of
   use," but that might change at some point. */
bool set_data_info::set_file (string file_name)
{
    literal_data_not_file = true;
    string_specified = false;
    data.assign ("");
    file_specified = true;
    // Remove the ' ' quotes around the file name:
    file_name.erase (0, 1);
    file_name.erase (file_name.length()-1, 1);
    file_path = file_name;
    return true;
}

/**********************************************************************************
   End of methods of class set_data_info.
**********************************************************************************/


/**********************************************************************************
   Class asset_name_id_info methods regarding setting and getting asset-data values:
**********************************************************************************/

asset_name_id_info::asset_name_id_info (void)  // (default constructor)
{
    id_n_not_name = false;  // (arbitrary)
    id_n = 100LL + ((uint64_t) rand() % 10000);  // default to random ID#
    asset_name.assign ("");
    id_n_specified = name_specified = false;  // no ID info yet
    asset_type = psa_asset_type::unknown;
    how_asset_found = asset_search::not_found;
    the_asset = nullptr;
    asset_ser_no = -1;
}
asset_name_id_info::~asset_name_id_info (void)
{
    asset_name_vector.clear();
    asset_id_n_vector.clear();
}

/* set_name() establishes:
   *  An asset's "human" name from a template line, and
   *  *That* that name was directly specified, as opposed to the asset being defined
      by ID only, or a random name being requested.
   Arguably, this method "has side effects," in that it not only sets a name, but
   also "takes notes" about where that name came from.
*/
void asset_name_id_info::set_name (string set_val)
{
    /* Use this to set the name as specified in the template file.  Call this only
       if the template file does indeed define a name. */
    name_specified = true;
    asset_name.assign (set_val);
}

/* set_calc_name() establishes:
   *  An asset's "human" name *not* from a template line, and
   *  *That* that name was *not* directly specified in any template line.
   Arguably, this method "has side effects," in that it not only sets a name, but
   also "takes notes" about where that name came from.
*/
void asset_name_id_info::set_calc_name (string set_val)
{
    name_specified = false;
    asset_name.assign (set_val);
}

// set_just_name() sets an asset's "human" name, without noting how that name came up.
void asset_name_id_info::set_just_name (string set_val)
{
    asset_name.assign (set_val);
}

/* Getter for protected member, asset_name.  Protected so that it can only be set by
   set_name() or set_calc_name(), above, to establish not only its value but
   how it came about. */
string asset_name_id_info::get_name (void)
{
    return asset_name;
}

// Asset IDs can be set directly from a uint64_t or converted from a string:
void asset_name_id_info::set_id_n (string set_val)
{
    id_n = stol (set_val, 0, 0);
}
void asset_name_id_info::set_id_n (uint64_t set_val)
{
    id_n = set_val;
}

// Create ID-based name:
string asset_name_id_info::make_id_n_based_name (uint64_t id_n)
{
    string result;

    switch (asset_type) {
        case psa_asset_type::sst:
            result = "SST_ID_";
            break;
        case psa_asset_type::key:
            result = "Key_ID_";
            break;
        case psa_asset_type::policy:
            result = "Policy_ID_";
            break;
        default:
            cerr << "\nError:  Tool-internal:  Please report error "
                 << "#1223 to the TF-Fuzz developers." << endl;
            exit(1223);
    }
    result.append(to_string(id_n));
    return result;
}

/**********************************************************************************
   End of methods of class asset_name_id_info.
**********************************************************************************/


/**********************************************************************************
   Class key_policy_info methods:
**********************************************************************************/

key_policy_info::key_policy_info (void)  // (default constructor)
{
    get_policy_from_key = false;  // specify policy asset by a key that uses it.
    copy_key = false;  // not copying one key to another
    /* The following settings are not necessarily being randomized in mutually-
       consistent ways, for two reasons:  First, the template should set all that
       matter, and second, testing TF response to nonsensical settings is also
       valuable. */
    exportable  = (rand()%2==1? true : false);
    copyable    = (rand()%2==1? true : false);
    can_encrypt = (rand()%2==1? true : false);
    can_decrypt = (rand()%2==1? true : false);
    can_sign    = (rand()%2==1? true : false);
    can_verify  = (rand()%2==1? true : false);
    derivable   = (rand()%2==1? true : false);
    persistent  = (rand()%2==1? true : false);
    // There's a really huge number of possible key types; won't randomize all:
    key_type = rand_key_type();
    usage_string.assign ("");
    print_usage_true_string.assign ("");
    print_usage_false_string.assign ("");
    key_algorithm = rand_key_algorithm();
    n_bits = 55 + (rand() % 1000);
    gibberish *gib = new gibberish;
    char buffer[256];
    char *end;
    int buf_len = 5ULL + (uint64_t) (rand() % 10);
    end = gib->word (false, buffer, buffer + buf_len);
    *end = '\0';
    buffer[buf_len] = '\0';
    handle_str = buffer;
    gib->sentence (buffer, buffer + (40ULL + (uint64_t) (rand() % 200)));
    key_data = buffer;
    delete gib;
}
key_policy_info::~key_policy_info (void)  // (destructor)
{
    return;  // (even if only to have something to pin a breakpoint on)
}


/**********************************************************************************
   End of methods of class key_policy_info.
**********************************************************************************/
