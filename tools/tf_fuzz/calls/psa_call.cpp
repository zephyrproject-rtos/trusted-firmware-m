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
#include "string_ops.hpp"
#include "data_blocks.hpp"
#include "psa_asset.hpp"
#include "find_or_create_asset.hpp"
#include "template_line.hpp"
#include "tf_fuzz.hpp"
#include "crypto_asset.hpp"
#include "psa_call.hpp"



/**********************************************************************************
   Methods of class psa_call follow:
**********************************************************************************/

//**************** psa_call methods ****************

psa_call::psa_call (tf_fuzz_info *test_state, long &call_ser_no,   // (constructor)
                    asset_search how_asset_found)
{
    this->test_state = test_state;
    this->asset_info.how_asset_found = how_asset_found;
    set_data.string_specified = false;
    set_data.set ("");  // actual data
    assign_data_var.assign ("");  // name of variable assigned (dumped) to
    assign_data_var_specified = false;
    set_data.file_specified = false;
    set_data.file_path.assign ("");
    this->call_ser_no = call_ser_no = unique_id_counter++;
    // These will be set in the lower-level constructors, but...
    prep_code = call_code = check_code = "";
    print_data = hash_data = false;
}

psa_call::~psa_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

void psa_call::write_out_prep_code (ofstream &test_file)
{
    test_file << prep_code;
}

void psa_call::write_out_command (ofstream &test_file)
{
    test_file << call_code;
}

void psa_call::write_out_check_code (ofstream &test_file)
{
    if (!exp_data.pf_nothing) {
        test_file << check_code;
    } else {
        test_file << "    /* (No checks for this PSA call.) */" << endl;
    }
}

/**********************************************************************************
   End of methods of class psa_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class sst_call follow:
**********************************************************************************/

/* calc_result_code() fills in the check_code string member with the correct
   result code (e.g., "PSA_SUCCESS" or whatever).

   This is a big part of where the target modeling -- error modeling -- occurs,
   so lots of room for further refinement here. */
void sst_call::calc_result_code (void)
{
    if (!exp_data.pf_nothing) {
        if (exp_data.pf_pass) {
            find_replace_all ("$expect",
                              test_state->bplate->bplate_string[sst_pass_string],
                              check_code);
        } else {
            if (exp_data.pf_specified) {
                find_replace_all ("$expect", exp_data.pf_result_string,
                                  check_code);
            } else {
                // Figure out what the message should read:
                switch (asset_info.how_asset_found) {
                    case asset_search::found_active:
                    case asset_search::created_new:
                        find_replace_all ("$expect",
                                          test_state->bplate->
                                              bplate_string[sst_pass_string],
                                          check_code);
                        break;
                    case asset_search::found_deleted:
                    case asset_search::not_found:
                        find_replace_all ("$expect",
                                          test_state->bplate->
                                              bplate_string[sst_fail_removed],
                                          check_code);
                        break;
                    default:
                        find_replace_1st ("!=", "==",
                                          check_code);  // for now, just make sure...
                        find_replace_all ("$expect",
                                          test_state->bplate->
                                              bplate_string[sst_pass_string],
                                          check_code);  // ... it's *not* PSA_SUCCESS
                        break;
                }
            }
        }
    }
}

vector<psa_asset*>::iterator sst_call::resolve_asset (bool create_asset_bool,
                                                      psa_asset_usage where) {
    vector<psa_asset*>::iterator found_asset;
    vector<psa_asset*> *asset_vector;
    int asset_pick;

    if (random_asset != psa_asset_usage::all) {
        // != psa_asset_usage::all means to choose some known asset at random:
        if (random_asset == psa_asset_usage::active) {
            asset_vector = &(test_state->active_sst_asset);
            asset_info.how_asset_found = asset_search::found_active;
        } else if (random_asset == psa_asset_usage::deleted) {
            asset_vector = &(test_state->deleted_sst_asset);
            asset_info.how_asset_found = asset_search::found_deleted;
        } else {
            // "invalid" assets are not currently used.
            cerr << "\nError:  Tool-internal:  Please report error 1101 to " << endl
                 << "TF-Fuzz developers."
                 << endl;
            exit(1101);
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
                cerr << "\nError:  An sst call asks for a "
                     << "randomly chosen active asset, when none " << endl
                     << "is currently defined." << endl;
                exit(1008);
            } else if (random_asset == psa_asset_usage::deleted) {
                cerr << "\nError:  An sst call asks for a "
                     << "randomly chosen deleted asset, when none " << endl
                     << "is currently defined." << endl;
                exit(1009);
            }  // "invalid" assets are not currently used.
        }
    } else {
        // Find the asset by name:
        asset_info.how_asset_found = test_state->find_or_create_sst_asset (
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

sst_call::sst_call (tf_fuzz_info *test_state, long &call_ser_no,   // (constructor)
                    asset_search how_asset_found)
                        : psa_call(test_state, call_ser_no, how_asset_found)
{
    asset_info.the_asset = nullptr;
    return;  // just to have something to pin a breakpoint onto
}
sst_call::~sst_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class sst_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class crypto_call follow:
**********************************************************************************/

/* calc_result_code() fills in the check_code string member with the correct
   result code (e.g., "PSA_SUCCESS" or whatever).  This "modeling" needs to be
   improved and expanded upon *massively* more or less mirroring what is seen in
   .../test/suites/crypto/crypto_tests_common.c in the psa_key_interface_test()
   method, (starting around line 20ish). */
void crypto_call::calc_result_code (void)
{
    if (!exp_data.pf_nothing) {
        if (exp_data.pf_pass) {
            find_replace_1st ("$expect", "PSA_SUCCESS", check_code);
        } else {
            if (exp_data.pf_specified) {
                find_replace_1st ("$expect", exp_data.pf_result_string,
                                  check_code);
            } else {
                // Figure out what the message should read:
                switch (asset_info.how_asset_found) {
                    case asset_search::found_active:
                    case asset_search::created_new:
                        find_replace_all ("$expect", "PSA_SUCCESS",
                                          check_code);
                        break;
                    case asset_search::found_deleted:
                        find_replace_all ("$expect", "PSA_ERROR_INVALID_HANDLE",
                                          check_code);
                        break;
                    default:
                        find_replace_1st ("!=", "==",
                                          check_code);  // for now, just make sure...
                        find_replace_all ("$expect", "PSA_SUCCESS",
                                          check_code);  // ... it's *not* PSA_SUCCESS
                        break;
                }
            }
        }
    }
}


crypto_call::crypto_call (tf_fuzz_info *test_state, long &call_ser_no,  // (constructor)
                          asset_search how_asset_found)
                             : psa_call(test_state, call_ser_no, how_asset_found)
{
    // Nothing further to initialize.
    return;  // just to have something to pin a breakpoint onto
}
crypto_call::~crypto_call (void)
{
    // Nothing further to delete.
    return;  // just to have something to pin a breakpoint onto
}

/**********************************************************************************
   End of methods of class crypto_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class security_call follow:
**********************************************************************************/

security_call::security_call (tf_fuzz_info *test_state, long &call_ser_no,  // (constructor)
                          asset_search how_asset_found)
                             : psa_call(test_state, call_ser_no, how_asset_found)
{
    // Nothing further to initialize.
    return;  // just to have something to pin a breakpoint onto
}
security_call::~security_call (void)
{
    // Nothing further to delete.
    return;  // just to have something to pin a breakpoint onto
}

// resolve_asset() doesn't do anything for security_calls, since there's no asset involved.
vector<psa_asset*>::iterator security_call::resolve_asset (bool create_asset_bool,
                                                           psa_asset_usage where)
{
    return test_state->active_sst_asset.end();  // (anything)
}

/* calc_result_code() fills in the check_code string member with the correct result
   code (e.g., "PSA_SUCCESS" or whatever).

   Since there are no actual PSA calls associated with security calls (so far at least),
   this should never be invoked. */
void security_call::calc_result_code (void)
{
    // Currently should not be invoked.
    cerr << "\nError:  Internal:  Please report error #205 to TF-Fuzz developers." << endl;
    exit (205);
}

/**********************************************************************************
   End of methods of class security_call.
**********************************************************************************/


