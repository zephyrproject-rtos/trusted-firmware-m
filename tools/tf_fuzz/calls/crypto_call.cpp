/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <cstdlib>

#include "class_forwards.hpp"

#include "boilerplate.hpp"
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

policy_call::policy_call (tf_fuzz_info *test_state,    // (constructor)
                          long &call_ser_no,
                          asset_search how_asset_found)
                             : crypto_call(test_state, call_ser_no, how_asset_found)
{
    // Randomize key-policy usage and algorithm:
    policy_usage = rand_key_usage();
    policy_algorithm = rand_key_algorithm();
    asset_info.the_asset = nullptr;
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
    // Note:  Similar random initialization for asset and template.
    // Randomize handle:
    // TODO:  Key handles appear to be a lot more complex a question than the below.
    string wrong_data;
    gibberish *gib = new gibberish;
    handle_str = gib->word (false, const_cast<char*>(wrong_data.c_str()),
                            const_cast<char*>(wrong_data.c_str())
                                                        + set_data.get().length()-1);
    // Randomize key type:
    key_type = rand_key_type();
    // Randomize lifetime:
    lifetime_str = ((rand() % 2) == 1)?
                       "PSA_KEY_LIFETIME_VOLATILE" : "PSA_KEY_LIFETIME_PERSISTENT";
    // Choose a random expected key size in bits:
    expected_n_bits = to_string(rand()%10000);
    delete gib;
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
   Methods of class policy_set_call follow:
**********************************************************************************/

policy_set_call::policy_set_call (tf_fuzz_info *test_state,    // (constructor)
                                  long &call_ser_no,
                                  asset_search how_asset_found)
                                     : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[set_policy]);
    check_code.assign (test_state->bplate->bplate_string[set_policy_check]);
}
policy_set_call::~policy_set_call (void)
{
    // Nothing further to delete.
    return;  // just to have something to pin a breakpoint onto
}

bool policy_set_call::copy_call_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;

    asset_search how_found = test_state->find_or_create_sst_asset (
                        psa_asset_search::name, psa_asset_usage::all,
                        asset_info.get_name(), 0, asset_info.asset_ser_no,
                        dont_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* an sst_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(108);
    }
    // Copy *the other way* on asset reads!
    if (how_found != asset_search::not_found) {
        asset_info.the_asset = reinterpret_cast<policy_asset*>(*found_asset);
           // Note:  Vector is of base-class type, but the assets *are* policy_asset.
        int i = asset_info.the_asset->set_data.n_set_vars;  // save this
        asset_info.the_asset->set_data = set_data;
        asset_info.the_asset->set_data.n_set_vars = set_data.n_set_vars = ++i;
        asset_info.the_asset->flags_string = flags_string;
        if (asset_info.how_asset_found == asset_search::created_new) {
            asset_info.the_asset->asset_info.name_specified = asset_info.name_specified;
            asset_info.the_asset->asset_info.set_name (asset_info.get_name());
            asset_info.the_asset->asset_info.asset_ser_no = asset_info.asset_ser_no;
            asset_info.the_asset->asset_info.id_n = asset_info.id_n;
        }
    }
    return true;
}

bool policy_set_call::copy_asset_to_call (void)
{
    // Get updated asset info from the asset:
    asset_info.asset_ser_no = asset_info.the_asset->asset_info.asset_ser_no;
    asset_info.id_n = asset_info.the_asset->asset_info.id_n;
    return true;
}

void policy_set_call::fill_in_prep_code (void)
{
    // No prep code required.
    return;  // just to have something to pin a breakpoint onto
}

void policy_set_call::fill_in_command (void)
{
    vector<psa_asset*>::iterator found_asset;

    // Is this search really needed?
    asset_info.how_asset_found = test_state->find_or_create_policy_asset (
                                      psa_asset_search::serial, psa_asset_usage::all,
                                      "", 0, asset_info.asset_ser_no,
                                      yes_create_asset, found_asset );
    if (asset_info.how_asset_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(103);
    }
    string var_name = asset_info.get_name() + "_data";
    string id_string = to_string((long) asset_info.id_n);
    // Fill in the PSA command itself:
    find_replace_1st ("$policy", id_string, call_code);
    find_replace_1st ("$usage", policy_usage, call_code);
    find_replace_1st ("$alg", policy_algorithm, call_code);
    // Figure out what expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class policy_set_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class policy_get_call follow:
**********************************************************************************/

policy_get_call::policy_get_call (tf_fuzz_info *test_state,    // (constructor)
                                  long &call_ser_no,
                                  asset_search how_asset_found)
                                     : policy_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_policy]);
    check_code.assign (test_state->bplate->bplate_string[get_policy_check]);
}
policy_get_call::~policy_get_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool policy_get_call::copy_call_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;

    asset_search how_found = test_state->find_or_create_sst_asset (
                        psa_asset_search::name, psa_asset_usage::all,
                        asset_info.get_name(), 0, asset_info.asset_ser_no,
                        dont_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* an sst_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(108);
    }
    // Copy *the other way* on asset reads!
    if (how_found != asset_search::not_found) {
        asset_info.the_asset = reinterpret_cast<policy_asset*>(*found_asset);
           // Note:  Vector is of base-class type, but the assets *are* policy_asset.
    }
    return true;
}

bool policy_get_call::copy_asset_to_call (void)
{
    if (asset_info.the_asset != nullptr) {
        set_data.string_specified = asset_info.the_asset->set_data.string_specified;
        set_data.file_specified = asset_info.the_asset->set_data.file_specified;
        flags_string = asset_info.the_asset->flags_string;
        asset_info.id_n = asset_info.the_asset->asset_info.id_n;
        asset_info.name_specified = asset_info.the_asset->asset_info.name_specified;
    }
    return true;
}

void policy_get_call::fill_in_prep_code (void)
{
    return;  // just to have something to pin a breakpoint onto
}

void policy_get_call::fill_in_command (void)
{
    find_replace_1st ("$policy", id_string, call_code);
    // Figure out what expected results:
// TODO:  Make data checks contingent upon the PSA call itself passing!
    calc_result_code();
    /* TODO:  DOESN'T SOMETHING HAVE TO CHECK THE ALGORITHM AND USAGE (perhaps
              that's a different call)? */
}

/**********************************************************************************
   End of methods of class policy_get_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class set_key_call follow:
**********************************************************************************/

set_key_call::set_key_call (tf_fuzz_info *test_state,    // (constructor)
                                  long &call_ser_no,
                                  asset_search how_asset_found)
                                     : key_call(test_state, call_ser_no,
                                                how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign (test_state->bplate->bplate_string[declare_generic]);
    call_code.assign (test_state->bplate->bplate_string[set_key]);
    check_code.assign (test_state->bplate->bplate_string[set_key_check]);
}
set_key_call::~set_key_call (void)
{
}

bool set_key_call::copy_call_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;

    // Find the asset by serial number (must search;  may have moved).
    asset_search how_found = test_state->find_or_create_key_asset (
                        psa_asset_search::name, psa_asset_usage::all,
                        asset_info.get_name(), 0, asset_info.asset_ser_no,
                        yes_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* a key_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(109);
    }
    // Copy over everything relevant:
    if (how_found != asset_search::not_found) {
        asset_info.the_asset = reinterpret_cast<key_asset*>(*found_asset);
           // Note:  Vector is of base-class type, but the assets *are* key_asset.
        int i = asset_info.the_asset->set_data.n_set_vars;  // save this
        asset_info.the_asset->set_data = set_data;
        asset_info.the_asset->set_data.n_set_vars = set_data.n_set_vars = ++i;
        asset_info.the_asset->flags_string = flags_string;
        if (asset_info.how_asset_found == asset_search::created_new) {
            asset_info.the_asset->asset_info.name_specified = asset_info.name_specified;
            asset_info.the_asset->asset_info.set_name (asset_info.get_name());
            asset_info.the_asset->asset_info.asset_ser_no = asset_info.asset_ser_no;
            asset_info.the_asset->asset_info.id_n = asset_info.id_n;
        }
    }
    return true;
}

bool set_key_call::copy_asset_to_call (void)
{
    // Get updated asset info from the asset:
    asset_info.asset_ser_no = asset_info.the_asset->asset_info.asset_ser_no;
    asset_info.id_n = asset_info.the_asset->asset_info.id_n;
    return true;
}

void set_key_call::fill_in_prep_code (void)
{
    // Create declaration of lifetime's holder variable:
    find_replace_1st ("$type", "psa_key_lifetime_t", prep_code);
    find_replace_1st ("$var", handle_str + "_lifetime", prep_code);
    find_replace_1st ("$init", lifetime_str, prep_code);
}

void set_key_call::fill_in_command (void)
{
    find_replace_1st ("$lifetime", handle_str + "_lifetime", call_code);
    find_replace_1st ("$handle", handle_str, call_code);
    // Figure out what expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class set_key_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class get_key_info_call follow:
**********************************************************************************/

get_key_info_call::get_key_info_call (tf_fuzz_info *test_state,    // (constructor)
                                      long &call_ser_no,
                                      asset_search how_asset_found)
                                         : key_call(test_state, call_ser_no,
                                                    how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign (test_state->bplate->bplate_string[declare_generic]);
    call_code.assign (test_state->bplate->bplate_string[get_key]);
    check_code.assign (test_state->bplate->bplate_string[get_key_check]);
}
get_key_info_call::~get_key_info_call (void)
{
}

bool get_key_info_call::copy_call_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;
    asset_search how_found = test_state->find_or_create_sst_asset (
                        psa_asset_search::name, psa_asset_usage::all,
                        asset_info.get_name(), 0, asset_info.asset_ser_no,
                        dont_create_asset, found_asset );
    // The vector is base-class, but this itself *really is* an sst_asset object:
    if (how_found == asset_search::unsuccessful) {
        cerr << "\nError:  Tool-internal:  TF-Fuzz failed to find a required asset."
             << endl;
        exit(108);
    }
    // Copy *the other way* on asset reads!
    if (how_found != asset_search::not_found) {
        asset_info.the_asset = reinterpret_cast<key_asset*>(*found_asset);
           // Note:  Vector is of base-class type, but the assets *are* key_asset.
        // Found the asset;  copy_asset_to_call() will do exactly that.
    }
    return true;
}

bool get_key_info_call::copy_asset_to_call (void)
{
    if (asset_info.the_asset != nullptr) {
        set_data.string_specified = asset_info.the_asset->set_data.string_specified;
        set_data.file_specified = asset_info.the_asset->set_data.file_specified;
        asset_info = asset_info.the_asset->asset_info;
    }
    return true;
}

void get_key_info_call::fill_in_prep_code (void)
{
    // Create declaration of size_t variable to accept #bits info into:
    find_replace_1st ("$type", "size_t", prep_code);
    find_replace_1st ("$var", handle_str + "_n_bits", prep_code);
    find_replace_1st ("$init", to_string(rand()%10000), prep_code);
}

void get_key_info_call::fill_in_command (void)
{
    // The call itself:
    find_replace_1st ("$handle", handle_str + "_lifetime", call_code);
    find_replace_1st ("$type", key_type, call_code);
    find_replace_1st ("$bits", handle_str + "_n_bits", call_code);
    // Figure out what expected return code:
    calc_result_code();
    // But also need to write code to check #bits returned:
    if (!exp_data.pf_nothing) {
        // TODO:  Make data checks contingent upon the PSA call itself passing!
        call_code.append (test_state->bplate->bplate_string[get_key_check_n_bits]);
        find_replace_1st ("$n_bits", handle_str + "_n_bits", check_code);
        find_replace_1st ("$m_bits", expected_n_bits, check_code);
    }
}

/**********************************************************************************
   End of methods of class get_key_info_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class destroy_key_call follow:
**********************************************************************************/

destroy_key_call::destroy_key_call (tf_fuzz_info *test_state,    // (constructor)
                                    long &call_ser_no,
                                    asset_search how_asset_found)
                                        : key_call(test_state, call_ser_no,
                                                   how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[remove_key]);
    check_code.assign (test_state->bplate->bplate_string[remove_key_check]);
}
destroy_key_call::~destroy_key_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool destroy_key_call::copy_call_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;

    found_asset = resolve_asset (dont_create_asset, psa_asset_usage::all);
    if (asset_info.how_asset_found != asset_search::not_found) {
        asset_info.the_asset = reinterpret_cast<key_asset*>(*found_asset);
           // Note:  Vector is of base-class type, but the assets *are* key_asset.
        if (asset_info.how_asset_found == asset_search::found_active) {
            // Delete asset;  move it from active vector to deleted vector:
            test_state->deleted_key_asset.push_back (asset_info.the_asset);
            test_state->active_key_asset.erase (found_asset);
        }  /* if not active, we'll deem the call expected to fail. */
    }
    return true;
}

bool destroy_key_call::copy_asset_to_call (void)
{
    if (asset_info.the_asset != nullptr) {  // will be found for make calls, but not necessarily others
        set_data.string_specified = asset_info.the_asset->set_data.string_specified;
        set_data.file_specified = asset_info.the_asset->set_data.file_specified;
        // Get updated asset info from the asset:
        asset_info = asset_info.the_asset->asset_info;
    }
    return true;
}

void destroy_key_call::fill_in_prep_code (void)
{
    // No prep code required.
    return;  // just to have something to pin a breakpoint onto
}

void destroy_key_call::fill_in_command (void)
{
    find_replace_1st ("$handle", handle_str + "_lifetime", call_code);
    // Figure out what expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class destroy_key_call.
**********************************************************************************/

