/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* Objects typed to subclasses of the these classes are constructed and filled in by
   the parser as it parses test-template lines.  There is not necessarily a one-to-one
   correspondence between the template lines and either the PSA commands generated nor
   PSA assets they manipulate.  PSA assets (which persist through the test) and PSA
   commands are therefore tracked in separate objects, but referenced here. */

#include <vector>
#include <algorithm>  // for STL find()

#include "class_forwards.hpp"

#include "data_blocks.hpp"
#include "boilerplate.hpp"
#include "randomization.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"
#include "crypto_call.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"



/**********************************************************************************
   Methods of class template_line follow:
**********************************************************************************/

// Constructor, passing in the tf_fuzz object for reference to all objects:
template_line::template_line (tf_fuzz_info *resources) : test_state(resources)
{
    set_data.file_path.assign ("");
    assign_data_var_specified = false;
    set_data.flags_string.assign ("");
    random_asset = psa_asset_usage::all;
        // if not deleting a random asset of a certain type, then search all as usual
    target_barrier = "";
    set_data.file_specified = false;
    print_data = hash_data = false;
    call_ser_no = -1;
    asset_2_name.assign ("");  asset_3_name.assign ("");
    is_remove = false;  // will correct this in the remove case
}

/**********************************************************************************
   Class template_line methods regarding setting and getting asset-names follow:
**********************************************************************************/

// Default destructor:
template_line::~template_line (void)
{
    // Destruct the vectors of asset names/IDs:
    asset_info.asset_name_vector.erase (asset_info.asset_name_vector.begin(),
                                        asset_info.asset_name_vector.end());
    asset_info.asset_id_n_vector.erase (asset_info.asset_id_n_vector.begin(),
                                        asset_info.asset_id_n_vector.end());
}

// (Default constructor not used)

/**********************************************************************************
   End of methods of class template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class sst_template_line follow:
**********************************************************************************/

// (Currently, no need to specialize copy_template_to_call() for individual SST ops.)
bool sst_template_line::copy_template_to_call (psa_call *call)
{
    // Copy asset info to call object for creation code:
    call->asset_info = asset_info;
    call->set_data = set_data;
    call->set_data.string_specified =   set_data.string_specified
                                     || set_data.random_data;
        // not really right, but more convenient to combine these two cases
    call->assign_data_var_specified = assign_data_var_specified;
    call->assign_data_var = assign_data_var;
    call->random_asset = random_asset;
    if (target_barrier == "") {  // barriers are probably not used for SST, but...
        call->target_barrier = asset_info.get_name();
    } else {
        call->target_barrier = target_barrier;
    }
    call->set_data.flags_string.assign (set_data.flags_string);
    call->exp_data = expect;
    call->exp_data.pf_info_incomplete = true;
    call->id_string = asset_name;  // data = expected
    call->print_data = print_data;
    call->hash_data = hash_data;
    return true;
}

sst_template_line::sst_template_line (tf_fuzz_info *resources)
                                          : template_line (resources)
{
    asset_info.asset_type = psa_asset_type::sst;
}

// Default destructor:
sst_template_line::~sst_template_line (void)
{
    // No real clean-up needed.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)



/**********************************************************************************
   End of methods of class sst_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class key_template_line follow:
**********************************************************************************/

key_template_line::key_template_line (tf_fuzz_info *resources)
                                          : template_line (resources)
{
    asset_info.asset_type = psa_asset_type::key;
}

bool key_template_line::copy_template_to_call (psa_call *call)
{
    // Copy asset info to call object for creation code:
    call->asset_info = asset_info;
    call->set_data = set_data;
    call->set_data = set_data;
    call->policy = policy_info;
    call->asset_2_name = asset_2_name;
    call->asset_3_name = asset_3_name;
    call->set_data.string_specified =   set_data.string_specified
                                     || set_data.random_data;
        // not really right, but more convenient to combine these two cases
    call->assign_data_var_specified = assign_data_var_specified;
    call->assign_data_var = assign_data_var;
    call->random_asset = random_asset;
    if (target_barrier == "") {  // barriers are probably not used for SST, but...
        call->target_barrier = asset_info.get_name();
    } else {
        call->target_barrier = target_barrier;
    }
    call->set_data.flags_string.assign (set_data.flags_string);
    call->exp_data = expect;
    call->exp_data.pf_info_incomplete = true;
    call->id_string = asset_name;  // data = expected
    call->print_data = print_data;
    call->hash_data = hash_data;
    return true;
}

// Create ID-based name:
string key_template_line::make_id_based_name (uint64_t id_n, string &name)
{
    string result = "Key_ID_";
    result.append(to_string(id_n));
    return result;
}

// Default destructor:
key_template_line::~key_template_line (void)
{
    // No real clean-up needed.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)



/**********************************************************************************
   End of methods of class key_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class policy_template_line follow:
**********************************************************************************/

policy_template_line::policy_template_line (tf_fuzz_info *resources)
            : template_line (resources)
{
    asset_info.asset_type = psa_asset_type::policy;
}

bool policy_template_line::copy_template_to_call (psa_call *call)
{
    // Copy asset info to call object for creation code:
    call->asset_info = asset_info;
    call->set_data = set_data;
    call->set_data = set_data;
    call->policy = policy_info;
    call->asset_2_name = asset_2_name;
    call->asset_3_name = asset_3_name;
    call->set_data.string_specified =   set_data.string_specified
                                     || set_data.random_data;
        // not really right, but more convenient to combine these two cases
    call->assign_data_var_specified = assign_data_var_specified;
    call->assign_data_var = assign_data_var;
    call->random_asset = random_asset;
    if (target_barrier == "") {  // barriers are probably not used for SST, but...
        call->target_barrier = asset_info.get_name();
    } else {
        call->target_barrier = target_barrier;
    }
    call->set_data.flags_string.assign (set_data.flags_string);
    call->exp_data = expect;
    call->exp_data.pf_info_incomplete = true;
    call->id_string = asset_name;  // data = expected
    call->print_data = print_data;
    call->hash_data = hash_data;
    return true;
}

// Create ID-based name:
string policy_template_line::make_id_based_name (uint64_t id_n, string &name)
{
    string result = "Policy_ID_";
    result.append(to_string(id_n));
    //
    return result;
}

// Default destructor:
policy_template_line::~policy_template_line (void)
{
    // No real clean-up needed.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)



/**********************************************************************************
   End of methods of class policy_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class security_template_line follow:
**********************************************************************************/

security_template_line::security_template_line (tf_fuzz_info *resources)
            : template_line (resources)
{
}

// Default destructor:
security_template_line::~security_template_line (void)
{
    // No real clean-up needed.
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class security_template_line.
**********************************************************************************/

