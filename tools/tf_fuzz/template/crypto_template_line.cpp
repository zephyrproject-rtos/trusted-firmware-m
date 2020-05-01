/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "class_forwards.hpp"

#include "boilerplate.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"
#include "crypto_call.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"
#include "crypto_template_line.hpp"



/**********************************************************************************
   Methods of class set_policy_template_line follow:
**********************************************************************************/

set_policy_template_line::set_policy_template_line (tf_fuzz_info *resources)
    : policy_template_line (resources)
{
    // No further setup to be performed.
    return;  // just to have something to pin a breakpoint onto
}

bool set_policy_template_line::copy_template_to_call (void)
{
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no && call->exp_data.pf_info_incomplete) {
            call->asset_info.set_just_name (asset_info.get_name());
            call->asset_info.id_n = asset_info.id_n;
            call->random_asset = random_asset;
            call->exp_data.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // failed to find the call
}

// Default destructor:
set_policy_template_line::~set_policy_template_line (void)
{
    // TODO:  Add an error for this being invoked.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class set_policy_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class read_policy_template_line follow:
**********************************************************************************/

read_policy_template_line::read_policy_template_line (tf_fuzz_info *resources)
    : policy_template_line (resources)
{
}

bool read_policy_template_line::copy_template_to_call (void)
{
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no && call->exp_data.pf_info_incomplete) {
            call->asset_info.set_just_name (asset_info.get_name());
            call->asset_info.id_n = asset_info.id_n;
            call->random_asset = random_asset;
            call->exp_data.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // failed to find the call
}

// Default destructor:
read_policy_template_line::~read_policy_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class read_policy_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class set_key_template_line follow:
**********************************************************************************/

set_key_template_line::set_key_template_line (tf_fuzz_info *resources)
    : key_template_line (resources)
{
    // Nothing further to initialize.
}

bool set_key_template_line::copy_template_to_call (void)
{
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            // Copy asset info to call object for creation code:
            call->asset_info.set_just_name (asset_info.get_name());
            call->asset_info.id_n = asset_info.id_n;
            call->asset_info.asset_ser_no = asset_info.asset_ser_no;
            call->asset_info.how_asset_found = asset_info.how_asset_found;
            call->random_asset = random_asset;
            call->set_data.string_specified = set_data.string_specified;
            call->set_data.set (set_data.get());
            call->set_data.file_specified = set_data.file_specified;
            call->set_data.file_path = set_data.file_path;
            call->exp_data.pf_info_incomplete = true;
            call->flags_string = flags_string;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

// Default destructor:
set_key_template_line::~set_key_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class set_key_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class remove_key_template_line follow:
**********************************************************************************/

remove_key_template_line::remove_key_template_line (tf_fuzz_info *resources)
    : key_template_line (resources)
{
    is_remove = true;  // template_line's constructor defaults this to false
}

bool remove_key_template_line::copy_template_to_call (void)
{
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            call->asset_info.set_just_name (asset_info.get_name());
            call->asset_info.id_n = asset_info.id_n;
            call->asset_info.asset_ser_no = asset_info.asset_ser_no;
            call->asset_info.how_asset_found = asset_info.how_asset_found;
            call->random_asset = random_asset;
            call->exp_data.pf_nothing = expect.pf_nothing;
            call->exp_data.pf_pass = expect.pf_pass;
            call->exp_data.pf_specified = expect.pf_specified;
            call->exp_data.pf_result_string = expect.pf_result_string;
            call->exp_data.pf_info_incomplete = true;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

// Default destructor:
remove_key_template_line::~remove_key_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class remove_key_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class read_key_template_line follow:
**********************************************************************************/

read_key_template_line::read_key_template_line (tf_fuzz_info *resources)
    : key_template_line (resources)
{
    char gibberish_buffer[4096];  string databuff;
    int data_length;
    set_data.string_specified = (rand()%2) == 1?  true : false;

    // Go ahead and create a literal-data string even if not ultimately used:
    data_length = test_state->gibbergen.pick_sentence_len();
    test_state->gibbergen.sentence (gibberish_buffer, gibberish_buffer + data_length);
    databuff = gibberish_buffer;  set_data.set (databuff);

    set_data.file_specified = (!set_data.string_specified && (rand()%2) == 1)?  true : false;
    set_data.file_path = "";  // can't really devise a random path
}

bool read_key_template_line::copy_template_to_call (void)
{
    for (auto call : test_state->calls) {
        if (call->call_ser_no == call_ser_no) {
            // Copy expected results to the call object, to check:
            call->asset_info.set_just_name (asset_info.get_name());
            call->asset_info.id_n = asset_info.id_n;
            call->asset_info.how_asset_found = asset_info.how_asset_found;
            call->asset_info.asset_ser_no = asset_info.asset_ser_no;
            call->random_asset = random_asset;
            call->set_data.string_specified = set_data.string_specified;
            call->set_data.set (set_data.get());
            call->set_data.file_specified = set_data.file_specified;
            call->set_data.file_path = set_data.file_path;
            call->set_data.string_specified = set_data.string_specified;
            call->exp_data.pf_info_incomplete = true;
            call->flags_string = flags_string;
            return true;
        }
    }
    return false;  // somehow didn't find it the call.
}

// Default destructor:
read_key_template_line::~read_key_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)

/**********************************************************************************
   End of methods of class read_key_template_line.
**********************************************************************************/
