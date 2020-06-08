/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <cstdlib>

#include "class_forwards.hpp"

#include "boilerplate.hpp"
#include "variables.hpp"
#include "gibberish.hpp"
#include "compute.hpp"
#include "randomization.hpp"
#include "string_ops.hpp"
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"
#include "crypto_call.hpp"
#include "sst_asset.hpp"


/**********************************************************************************
   Methods of class policy_call follow:
**********************************************************************************/

/* Most of the policy classes, in their fill_in_prep_code() method, need to ensure
   that, at a minimum, the policy variable (psa_key_attributes_t) exists, so, just
   to cut down code duplication: */
void policy_call::policy_fill_in_prep_code (void)
{
    vector<variable_info>::iterator policy_variable;

    policy_variable = test_state->find_var (asset_info.get_name());
    if (policy_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (asset_info.get_name());
        policy_variable = test_state->find_var (asset_info.get_name());
        prep_code.assign (test_state->bplate->bplate_string[declare_policy]);
        find_replace_1st ("$var", asset_info.get_name(), prep_code);
    }
}


policy_call::policy_call (tf_fuzz_info *test_state,    // (constructor)
                          long &call_ser_no,
                          asset_search how_asset_found)
                             : crypto_call(test_state, call_ser_no, how_asset_found)
{
    // Note:  Key attributes are set in the key_policy_info constructor.
}
policy_call::~policy_call (void)
{
    // Nothing further to delete.
    return;  // just to have something to pin a breakpoint onto
}

vector<psa_asset*>::iterator policy_call::resolve_asset (bool create_asset_bool,
                                                         psa_asset_usage where) {
    vector<psa_asset*>::iterator found_asset;
    vector<psa_asset*> *asset_vector;
    int asset_pick;

    if (random_asset != psa_asset_usage::all) {
        // != psa_asset_usage::all means to choose some known asset at random:
        if (random_asset == psa_asset_usage::active) {
            asset_vector = &(test_state->active_policy_asset);
            asset_info.how_asset_found = asset_search::found_active;
        } else if (random_asset == psa_asset_usage::deleted) {
            asset_vector = &(test_state->deleted_policy_asset);
            asset_info.how_asset_found = asset_search::found_deleted;
        } else {
            // "invalid" assets are not currently used.
            cerr << "\nError:  Tool-internal:  Please report error 1102 to " << endl
                 << "TF-Fuzz developers."
                 << endl;
            exit(1102);
        }
        if (asset_vector->size() > 0) {
            /* Pick an active or deleted asset at random: */
            asset_pick = rand() % asset_vector->size();
            found_asset = asset_vector->begin() + asset_pick;
            /* Copy asset information into template tracker: */
            asset_info.id_n = (*found_asset)->asset_info.id_n;
            asset_info.asset_ser_no
                    = (*found_asset)->asset_info.asset_ser_no;
        } else {
            if (random_asset == psa_asset_usage::active) {
                cerr << "\nError:  A policy call asks for a "
                     << "randomly chosen active asset, when none " << endl
                     << "is currently defined." << endl;
                exit(1010);
            } else if (random_asset == psa_asset_usage::deleted) {
                cerr << "\nError:  A policy call asks for a "
                     << "randomly chosen deleted asset, when none " << endl
                     << "is currently defined." << endl;
                exit(1011);
            }  // "invalid" assets are not currently used.
        }
    } else {
        // Find the asset by name:
        asset_info.how_asset_found = test_state->find_or_create_policy_asset (
                            psa_asset_search::name, where,
                            asset_info.get_name(), 0, asset_info.asset_ser_no,
                            create_asset_bool, found_asset );
        if (   asset_info.how_asset_found == asset_search::unsuccessful
            || asset_info.how_asset_found == asset_search::something_wrong ) {
            cerr << "\nError:  Tool-internal:  Please report error 108 to " << endl
                 << "TF-Fuzz developers."
                 << endl;
            exit(108);
        }
    }
    return found_asset;
}

/**********************************************************************************
   End of methods of class policy_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class key_call follow:
**********************************************************************************/

key_call::key_call (tf_fuzz_info *test_state,    // (constructor)
                          long &call_ser_no,
                          asset_search how_asset_found)
                             : crypto_call(test_state, call_ser_no, how_asset_found)
{
    asset_info.the_asset = nullptr;
}
key_call::~key_call (void)
{
    // Nothing further to delete.
    return;  // just to have something to pin a breakpoint onto
}

vector<psa_asset*>::iterator key_call::resolve_asset (bool create_asset_bool,
                                                      psa_asset_usage where) {
    vector<psa_asset*>::iterator found_asset;
    vector<psa_asset*> *asset_vector;
    int asset_pick;

    if (random_asset != psa_asset_usage::all) {
        // != psa_asset_usage::all means to choose some known asset at random:
        if (random_asset == psa_asset_usage::active) {
            asset_vector = &(test_state->active_key_asset);
            asset_info.how_asset_found = asset_search::found_active;
        } else if (random_asset == psa_asset_usage::deleted) {
            asset_vector = &(test_state->deleted_key_asset);
            asset_info.how_asset_found = asset_search::found_deleted;
        } else {
            // "invalid" assets are not currently used.
            cerr << "\nError:  Tool-internal:  Please report error 1103 to " << endl
                 << "TF-Fuzz developers."
                 << endl;
            exit(1103);
        }
        if (asset_vector->size() > 0) {
            /* Pick an active or deleted asset at random: */
            asset_pick = rand() % asset_vector->size();
            found_asset = asset_vector->begin() + asset_pick;
            /* Copy asset information into template tracker: */
            asset_info.id_n = (*found_asset)->asset_info.id_n;
            asset_info.asset_ser_no
                    = (*found_asset)->asset_info.asset_ser_no;
        } else {
            if (random_asset == psa_asset_usage::active) {
                cerr << "\nError:  A key call asks for a "
                     << "randomly chosen active asset, when none " << endl
                     << "is currently defined." << endl;
                exit(1012);
            } else if (random_asset == psa_asset_usage::deleted) {
                cerr << "\nError:  A key call asks for a "
                     << "randomly chosen deleted asset, when none " << endl
                     << "is currently defined." << endl;
                exit(1013);
            }  // "invalid" assets are not currently used.
        }
    } else {
        // Find the asset by name:
        asset_info.how_asset_found = test_state->find_or_create_key_asset (
                            psa_asset_search::name, where,
                            asset_info.get_name(), 0, asset_info.asset_ser_no,
                            create_asset_bool, found_asset );
        if (   asset_info.how_asset_found == asset_search::unsuccessful
            || asset_info.how_asset_found == asset_search::something_wrong ) {
            cerr << "\nError:  Tool-internal:  Please report error 108 to " << endl
                 << "TF-Fuzz developers."
                 << endl;
            exit(108);
        }
    }
    return found_asset;
}

/**********************************************************************************
   End of methods of class key_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class init_policy_call follow:
**********************************************************************************/

init_policy_call::init_policy_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[init_policy]);
    call_description = "initialize-policy call";
}
init_policy_call::~init_policy_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool init_policy_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, yes_create_asset);
}

void init_policy_call::fill_in_prep_code (void)
{
    policy_fill_in_prep_code();
}

void init_policy_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class init_policy_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class reset_policy_call follow:
**********************************************************************************/

reset_policy_call::reset_policy_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[reset_policy]);
    call_description = "policy reset call";
}
reset_policy_call::~reset_policy_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool reset_policy_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, yes_create_asset);
}

void reset_policy_call::fill_in_prep_code (void)
{
    policy_fill_in_prep_code();
}

void reset_policy_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class reset_policy_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class add_policy_usage_call follow:
**********************************************************************************/

add_policy_usage_call::add_policy_usage_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[add_policy_usage]);
    call_description = "policy add-usage call";
}
add_policy_usage_call::~add_policy_usage_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool add_policy_usage_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, yes_create_asset);
}

void add_policy_usage_call::fill_in_prep_code (void)
{
    policy_fill_in_prep_code();
    /* TODO:  The variable this creates should have been declared already.  Should
              this instead produce an error if it doesn't exist? */
}

void add_policy_usage_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$flag", policy.usage_string, call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class add_policy_usage_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class set_policy_lifetime_call follow:
**********************************************************************************/

set_policy_lifetime_call::set_policy_lifetime_call (tf_fuzz_info *test_state,
                                    long &call_ser_no,    // (constructor)
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[set_policy_lifetime]);
    call_description = "policy lifetime-set call";
}
set_policy_lifetime_call::~set_policy_lifetime_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool set_policy_lifetime_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, yes_create_asset);
}

void set_policy_lifetime_call::fill_in_prep_code (void)
{
    policy_fill_in_prep_code();
}

void set_policy_lifetime_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$life",
                      policy.persistent?   "PSA_KEY_LIFETIME_PERSISTENT"
                                         : "PSA_KEY_LIFETIME_VOLATILE",
                      call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class set_policy_lifetime_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class set_policy_size_call follow:
**********************************************************************************/

set_policy_size_call::set_policy_size_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[set_policy_size]);
    call_description = "policy size-set call";
}
set_policy_size_call::~set_policy_size_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool set_policy_size_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, yes_create_asset);
}

void set_policy_size_call::fill_in_prep_code (void)
{
    policy_fill_in_prep_code();
}

void set_policy_size_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$size", to_string (policy.n_bits), call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class set_policy_size_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class set_policy_type_call follow:
**********************************************************************************/

set_policy_type_call::set_policy_type_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[set_policy_type]);
    call_description = "policy type-set call";
}
set_policy_type_call::~set_policy_type_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool set_policy_type_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, yes_create_asset);
}

void set_policy_type_call::fill_in_prep_code (void)
{
    policy_fill_in_prep_code();
}

void set_policy_type_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$type", policy.key_type, call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class set_policy_type_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class set_policy_algorithm_call follow:
**********************************************************************************/

set_policy_algorithm_call::set_policy_algorithm_call (tf_fuzz_info *test_state,
                                    long &call_ser_no,    // (constructor)
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[set_policy_algorithm]);
    call_description = "policy algorithm-set call";
}
set_policy_algorithm_call::~set_policy_algorithm_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool set_policy_algorithm_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, yes_create_asset);
}

void set_policy_algorithm_call::fill_in_prep_code (void)
{
    policy_fill_in_prep_code();
}

void set_policy_algorithm_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$algorithm", policy.key_algorithm, call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class set_policy_algorithm_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class set_policy_usage_call follow:
**********************************************************************************/

set_policy_usage_call::set_policy_usage_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[set_policy_usage]);
    call_description = "policy usage-set call";
}
set_policy_usage_call::~set_policy_usage_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool set_policy_usage_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, yes_create_asset);
}

void set_policy_usage_call::fill_in_prep_code (void)
{
    policy_fill_in_prep_code();
}

void set_policy_usage_call::fill_in_command (void)
{
    find_replace_1st ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$usage", "0", call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class set_policy_usage_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class get_policy_lifetime_call follow:
**********************************************************************************/

get_policy_lifetime_call::get_policy_lifetime_call (tf_fuzz_info *test_state,
                                    long &call_ser_no,    // (constructor)
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_policy_lifetime]);
    call_description = "policy lifetime-get call";
}
get_policy_lifetime_call::~get_policy_lifetime_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool get_policy_lifetime_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, dont_create_asset);
}

void get_policy_lifetime_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name() + "_life";
    vector<variable_info>::iterator assign_variable;

    policy_fill_in_prep_code();  // make sure the policy variable itself is defined
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_policy_lifetime]);
        find_replace_all ("$var", var_name, prep_code);
    }
}

void get_policy_lifetime_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$life", asset_info.get_name() + "_life", call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class get_policy_lifetime_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class get_policy_size_call follow:
**********************************************************************************/

get_policy_size_call::get_policy_size_call (tf_fuzz_info *test_state,
                                    long &call_ser_no,    // (constructor)
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_policy_size]);
    call_description = "policy size-get call";
}
get_policy_size_call::~get_policy_size_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool get_policy_size_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, dont_create_asset);
}

void get_policy_size_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name() + "_size";
    vector<variable_info>::iterator assign_variable;

    policy_fill_in_prep_code();  // make sure the policy variable itself is defined
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_size_t]);
        find_replace_all ("$var", var_name, prep_code);
        find_replace_1st ("$init", to_string(exp_data.data.length()), prep_code);
    }
}

void get_policy_size_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$size", asset_info.get_name() + "_size", call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class get_policy_size_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class get_policy_type_call follow:
**********************************************************************************/

get_policy_type_call::get_policy_type_call (tf_fuzz_info *test_state,
                                    long &call_ser_no,    // (constructor)
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_policy_type]);
    call_description = "policy type-get call";
}
get_policy_type_call::~get_policy_type_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool get_policy_type_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, dont_create_asset);
}

void get_policy_type_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name() + "_type";
    vector<variable_info>::iterator assign_variable;

    policy_fill_in_prep_code();  // make sure the policy variable itself is defined
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_policy_type]);
        find_replace_all ("$var", var_name, prep_code);
    }
}

void get_policy_type_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$type", asset_info.get_name() + "_type", call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class get_policy_type_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class get_policy_algorithm_call follow:
**********************************************************************************/

get_policy_algorithm_call::get_policy_algorithm_call (tf_fuzz_info *test_state,
                                    long &call_ser_no,    // (constructor)
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_policy_algorithm]);
    call_description = "policy algorithm-get call";
}
get_policy_algorithm_call::~get_policy_algorithm_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool get_policy_algorithm_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, dont_create_asset);
}

void get_policy_algorithm_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name() + "_algo";
    vector<variable_info>::iterator assign_variable;

    policy_fill_in_prep_code();  // make sure the policy variable itself is defined
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_policy_algorithm]);
        find_replace_all ("$var", var_name, prep_code);
    }
}

void get_policy_algorithm_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$algorithm", asset_info.get_name() + "_algo", call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class get_policy_algorithm_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class get_policy_usage_call follow:
**********************************************************************************/

get_policy_usage_call::get_policy_usage_call (tf_fuzz_info *test_state,
                                    long &call_ser_no,    // (constructor)
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_policy_usage]);
    call_description = "policy usage-get call";
}
get_policy_usage_call::~get_policy_usage_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool get_policy_usage_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, dont_create_asset);
}

void get_policy_usage_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name() + "_usage";
    vector<variable_info>::iterator assign_variable;

    policy_fill_in_prep_code();  // make sure the policy variable itself is defined
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_policy_usage]);
        find_replace_all ("$var", var_name, prep_code);
    }
}

void get_policy_usage_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$usage", asset_info.get_name() + "_usage", call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class get_policy_usage_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class print_policy_usage_call follow:
**********************************************************************************/

print_policy_usage_call::print_policy_usage_call (tf_fuzz_info *test_state,
                                    long &call_ser_no,    // (constructor)
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[print_policy_usage]);
    call_description = "policy usage-print call";
}
print_policy_usage_call::~print_policy_usage_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool print_policy_usage_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<policy_asset*> (this, dont_create_asset);
}

void print_policy_usage_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name() + "_usage";
    vector<variable_info>::iterator assign_variable;

    policy_fill_in_prep_code();  // make sure the policy variable itself is defined
    // Make sure policy-usage variable is defined:
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_policy_usage]);
        find_replace_all ("$var", var_name, prep_code);
    }
}

void print_policy_usage_call::fill_in_command (void)
{
    string var_name = asset_info.get_name() + "_usage";

    // (call_code already loaded by constructor)
    find_replace_all ("$policy", asset_info.get_name(), call_code);
    find_replace_1st ("$usage_string", policy.usage_string, call_code);
    find_replace_1st ("$usage", var_name, call_code);
    find_replace_1st ("$print_usage_true_string", policy.print_usage_true_string,
                      call_code);
    find_replace_1st ("$print_usage_false_string", policy.print_usage_false_string,
                      call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class print_policy_usage_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class get_key_policy_call follow:
**********************************************************************************/

get_key_policy_call::get_key_policy_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_policy]);
    check_code.assign (test_state->bplate->bplate_string[get_policy_check]);
    call_description = "policy get call";
}
get_key_policy_call::~get_key_policy_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool get_key_policy_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<key_asset*> (this, dont_create_asset);
}

void get_key_policy_call::fill_in_prep_code (void)
{
    // No prep code required.
    return;  // just to have something to pin a breakpoint onto
}

void get_key_policy_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("key", asset_info.get_name(), call_code);
    find_replace_all ("$policy", policy.asset_2_name, call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class get_key_policy_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class generate_key_call follow:
**********************************************************************************/

generate_key_call::generate_key_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : key_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[generate_key]);
    check_code.assign (test_state->bplate->bplate_string[generate_key_check]);
    call_description = "key-generate call";
}
generate_key_call::~generate_key_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool generate_key_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<key_asset*> (this, yes_create_asset);
}

void generate_key_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name();
    vector<variable_info>::iterator assign_variable;

    // Make sure key variable is defined:
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_key]);
        find_replace_all ("$var", var_name, prep_code);
    }
}

void generate_key_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", policy.asset_2_name, call_code);
    find_replace_all ("$key", asset_info.get_name(), call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class generate_key_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class create_key_call follow:
**********************************************************************************/

create_key_call::create_key_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : key_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[create_key]);
    check_code.assign (test_state->bplate->bplate_string[create_key_check]);
    call_description = "key-create call";
}
create_key_call::~create_key_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool create_key_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<key_asset*> (this, yes_create_asset);
}

void create_key_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name();
    vector<variable_info>::iterator assign_variable;
    gibberish gib;
    char gib_buff[500];
    string t_string;

    // Key variable:
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_key]);
        find_replace_all ("$var", var_name, prep_code);
    }
    // Key-data variable:
    var_name = asset_info.get_name() + "_set_data";
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_big_string]);
        find_replace_all ("$var", var_name, prep_code);
        int rand_data_length = 12 + (rand() % 100);
        gib.sentence (gib_buff, gib_buff + rand_data_length - 1);
        t_string = gib_buff;
        find_replace_all ("$init", t_string, prep_code);
    }
}

void create_key_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$policy", policy.asset_2_name, call_code);
    find_replace_all ("$data", asset_info.get_name() + "_set_data", call_code);
    find_replace_all ("$length", to_string (policy.n_bits), call_code);
    find_replace_all ("$key", asset_info.get_name(), call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class create_key_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class copy_key_call follow:
**********************************************************************************/

copy_key_call::copy_key_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : key_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[copy_key]);
    check_code.assign (test_state->bplate->bplate_string[copy_key_check]);
    call_description = "key-copy call";
}
copy_key_call::~copy_key_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool copy_key_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<key_asset*> (this, yes_create_asset);
}

void copy_key_call::fill_in_prep_code (void)
{
    string var_name = asset_info.get_name();
    vector<variable_info>::iterator assign_variable;

    // Make sure key variable is defined:
    assign_variable = test_state->find_var (var_name);
    if (assign_variable == test_state->variable.end()) {
        // No such variable exists, so:
        test_state->make_var (var_name);
        prep_code.append (test_state->bplate->bplate_string[declare_key]);
        find_replace_all ("$var", var_name, prep_code);
    }
}

void copy_key_call::fill_in_command (void)
{
    vector<psa_asset*>::iterator key_asset;
    vector<psa_asset*> *asset_vector;
    int asset_pick;

    // Calculate the expected results:
    asset_search find_result;
    vector<psa_asset*>::iterator asset;
    long dummy = 0L;
    // See if the source key does not exist:

    if (random_asset != psa_asset_usage::all) {
        // != psa_asset_usage::all means to choose some known asset at random:
        if (random_asset == psa_asset_usage::active) {
            asset_vector = &(test_state->active_key_asset);
            find_result = asset_info.how_asset_found = asset_search::found_active;
                // if empty, we'll error out, below.
        } else if (random_asset == psa_asset_usage::deleted) {
            asset_vector = &(test_state->deleted_key_asset);
            find_result = asset_info.how_asset_found = asset_search::found_deleted;
        } else {
            // "invalid" assets are not currently used.
            cerr << "\nError:  Tool-internal:  Please report error 1103 to " << endl
                 << "TF-Fuzz developers."
                 << endl;
            exit(1103);
        }
        if (asset_vector->size() > 0) {
            /* Pick an active or deleted asset at random: */
            asset_pick = rand() % asset_vector->size();
            key_asset = asset_vector->begin() + asset_pick;
            /* Copy asset information into template tracker: */
            asset_info.id_n = (*key_asset)->asset_info.id_n;
            asset_info.asset_ser_no
                    = (*key_asset)->asset_info.asset_ser_no;
        } else {
            if (random_asset == psa_asset_usage::active) {
                cerr << "\nError:  A key call asks for a "
                     << "randomly chosen active asset, when none " << endl
                     << "is currently defined." << endl;
                exit(1012);
            } else if (random_asset == psa_asset_usage::deleted) {
                cerr << "\nError:  A key call asks for a "
                     << "randomly chosen deleted asset, when none " << endl
                     << "is currently defined." << endl;
                exit(1013);
            }  // "invalid" assets are not currently used.
        }
    } else {
        find_result = test_state->
            find_or_create_key_asset (psa_asset_search::name, psa_asset_usage::active,
                                      policy.asset_3_name, (uint64_t) 0, dummy,
                                      dont_create_asset, key_asset);
    }
    if (find_result != asset_search::found_active) {
        exp_data.pf_specified = true;
        exp_data.pf_result_string = "PSA_ERROR_INVALID_ARGUMENT";
            // TODO:  Pull this in from boilerplate file
    } else {
        // See if the new policy does not exist:
        find_result = test_state->
            find_or_create_policy_asset (psa_asset_search::name, psa_asset_usage::active,
                                         policy.asset_2_name, (uint64_t) 0, dummy,
                                         dont_create_asset, asset);
        if (find_result != asset_search::found_active) {
            exp_data.pf_specified = true;
            exp_data.pf_result_string = "PSA_ERROR_INVALID_ARGUMENT";
                // TODO:  Pull this in from boilerplate file
        } else if (!(*asset)->policy.copyable) {
            // See if the source key does not support export:
            // TODO:  Or wait, it's the original policy for the key, right?
            exp_data.pf_specified = true;
            exp_data.pf_result_string = "PSA_ERROR_NOT_PERMITTED";
        }
    }

    // (call_code already loaded by constructor)
    find_replace_all ("$master", (*key_asset)->asset_info.get_name(), call_code);
    find_replace_all ("$policy", policy.asset_2_name, call_code);
    find_replace_all ("$copy", asset_info.get_name(), call_code);
    calc_result_code();
}

/**********************************************************************************
   End of methods of class copy_key_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class read_key_data_call follow:
**********************************************************************************/

read_key_data_call::read_key_data_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : key_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[read_key_data]);
    check_code.assign (test_state->bplate->bplate_string[read_key_data_check]);
    call_description = "key read-data call";
}
read_key_data_call::~read_key_data_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool read_key_data_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<key_asset*> (this, dont_create_asset);
}

void read_key_data_call::fill_in_prep_code (void)
{
    string var_name, length_var_name, actual_length_var_name, var_name_suffix,
           length_var_name_suffix, temp_string;
    vector<variable_info>::iterator expect_variable;
    vector<variable_info>::iterator assign_variable;

    if (exp_data.data_var_specified) {
        var_name.assign (exp_data.data_var + "_data");
        length_var_name.assign (exp_data.data_var + "_length");
        /* If actual-data variable doesn't already exist, create variable tracker,
           and write declaration for it: */
        expect_variable = test_state->find_var (exp_data.data_var);
        if (expect_variable == test_state->variable.end()) {
            // No such variable exists, so:
            test_state->make_var (exp_data.data_var);
            expect_variable = test_state->find_var (exp_data.data_var);
            prep_code.append (test_state->bplate->bplate_string[declare_big_string]);
            find_replace_1st ("$var", var_name, prep_code);
            temp_string = (char *) expect_variable->value;
            find_replace_1st ("$init", temp_string, prep_code);
            // Input data length:
            prep_code.append (test_state->bplate->bplate_string[declare_size_t]);
            find_replace_1st ("$var", length_var_name, prep_code);
            find_replace_1st ("$init", to_string(temp_string.length()), prep_code);
            // TODO:  Is these lengths in bits or bytes?
        }
    }
    // else we're not comparing to named variable, so compare to assigned data.
    // Actual (output) data length:
    if (assign_data_var_specified) {
        var_name.assign (assign_data_var + "_data");
        length_var_name.assign (assign_data_var + "_length");
        actual_length_var_name.assign (assign_data_var + "_act_length");
        /* If actual-data variable doesn't already exist, create variable tracker,
           and write declaration for it: */
        assign_variable = test_state->find_var (assign_data_var);
        if (assign_variable == test_state->variable.end()) {
            // No such variable exists, so:
            test_state->make_var (assign_data_var);
            assign_variable = test_state->find_var (assign_data_var);
            prep_code.append (test_state->bplate->bplate_string[declare_big_string]);
            find_replace_1st ("$var", var_name, prep_code);
            temp_string = (char *) assign_variable->value;
            find_replace_1st ("$init", temp_string, prep_code);
            // Input data length:
            prep_code.append (test_state->bplate->bplate_string[declare_size_t]);
            find_replace_1st ("$var", length_var_name, prep_code);
            find_replace_1st ("$init", to_string(temp_string.length()), prep_code);
        }
    } else {
        // Key data not read into a named variable, so use default C variable:
        var_name = asset_info.get_name() + "_act_data";
        prep_code.append (test_state->bplate->bplate_string[declare_string]);
        find_replace_1st ("$var", var_name, prep_code);
        find_replace_1st ("$init", set_data.get(), prep_code);
        actual_length_var_name.assign (asset_info.get_name() + "_act_length");
        prep_code.append (test_state->bplate->bplate_string[declare_size_t]);
        find_replace_1st ("$var", actual_length_var_name, prep_code);
        find_replace_1st ("$init", to_string(set_data.get().length()), prep_code);
    }
    length_var_name_suffix = "_read_length";
    var_name.assign (asset_info.get_name() + var_name_suffix);
    length_var_name.assign (asset_info.get_name() + length_var_name_suffix);
    prep_code.append (test_state->bplate->bplate_string[declare_size_t]);
    find_replace_1st ("$var", length_var_name, prep_code);
    find_replace_1st ("$init", to_string(set_data.get().length()), prep_code);
}

void read_key_data_call::fill_in_command (void)
{
    string var_name, length_var_name, exp_var_name, exp_length_var_name, 
           actual_length_var_name, var_name_suffix, length_var_name_suffix, 
           temp_string;

    // Fill in the PSA command itself:
    actual_length_var_name.assign (asset_info.get_name() + "_read_length");
    if (assign_data_var_specified) {
        var_name.assign (assign_data_var + "_data");
        length_var_name.assign (assign_data_var + "_length");
    } else {
        var_name_suffix = "_act_data";
        var_name.assign (asset_info.get_name() + var_name_suffix);
        length_var_name_suffix = "_act_length";
        length_var_name.assign (asset_info.get_name() + length_var_name_suffix);
    }
    find_replace_1st ("$data", var_name, call_code);
    find_replace_1st ("$key", asset_info.get_name(), call_code);
    string id_string = to_string((long) asset_info.id_n++);
    find_replace_1st ("$length", length_var_name, call_code);
    find_replace_1st ("$act_size", actual_length_var_name, call_code);

    // Check data:
    if (exp_data.data_var_specified) {
        check_code.assign (test_state->bplate->bplate_string[compare_data]);
        exp_var_name.assign (exp_data.data_var + "_data");
        exp_length_var_name.assign (exp_data.data_var + "_length");
        find_replace_1st ("$act_data", var_name, check_code);
        find_replace_1st ("$exp_data", exp_var_name, check_code);
        find_replace_1st ("$length", exp_length_var_name, check_code);
    }

    // See if the source key did not exist:
    if (!policy.exportable) {
        // See if the source key does not support export:
        exp_data.pf_specified = true;
        exp_data.pf_result_string = "PSA_ERROR_NOT_PERMITTED";
    }
    calc_result_code();
}

/**********************************************************************************
   End of methods of class read_key_data_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class remove_key_call follow:
**********************************************************************************/

remove_key_call::remove_key_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : key_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[remove_key]);
    check_code.assign (test_state->bplate->bplate_string[remove_key_check]);
    call_description = "key-remove call";
}
remove_key_call::~remove_key_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool remove_key_call::copy_call_to_asset (void)
{
    return copy_call_to_asset_t<key_asset*> (this, dont_create_asset);
}

void remove_key_call::fill_in_prep_code (void)
{
    // No prep code required.
    return;  // just to have something to pin a breakpoint onto
}

void remove_key_call::fill_in_command (void)
{
    // (call_code already loaded by constructor)
    find_replace_all ("$key", asset_info.get_name(), call_code);
    // Calculate the expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class remove_key_call.
**********************************************************************************/

