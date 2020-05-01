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
#include "sst_call.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"
#include "variables.hpp"



/**********************************************************************************
   Methods of class sst_set_call follow:
**********************************************************************************/

sst_set_call::sst_set_call (tf_fuzz_info *test_state,    // (constructor)
                            long &call_ser_no,
                            asset_search how_asset_found)
                                : sst_call(test_state, call_ser_no, how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");  // will fill in, depending upon template line content
    call_code.assign (test_state->bplate->bplate_string[set_sst_call]);
    check_code.assign (test_state->bplate->bplate_string[set_sst_check]);
}
sst_set_call::~sst_set_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool sst_set_call::copy_call_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;

    found_asset = resolve_asset (yes_create_asset, psa_asset_usage::all);
    // Copy over everything relevant:
    if (asset_info.how_asset_found != asset_search::not_found) {
        // will be found for make calls, but not necessarily others
        asset_info.the_asset = reinterpret_cast<sst_asset*>(*found_asset);
            /* Note:  The vector is base-class, but the assets in this list
                      themselves *really are* sst_asset-type objects. */
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

bool sst_set_call::copy_asset_to_call (void)
{
    // Get updated asset info from the asset:
    asset_info.asset_ser_no = asset_info.the_asset->asset_info.asset_ser_no;
    asset_info.id_n = asset_info.the_asset->asset_info.id_n;
    exp_data.n_exp_vars = asset_info.the_asset->exp_data.n_exp_vars;
    exp_data.data = asset_info.the_asset->exp_data.data;
    return true;
}

void sst_set_call::fill_in_prep_code (void)
{
    string var_name, length_var_name, var_name_suffix, length_var_name_suffix,
           temp_string;
    vector<variable_info>::iterator assign_variable;

    if (assign_data_var_specified) {
        var_name.assign (assign_data_var + "_data");
        length_var_name.assign (assign_data_var + "_length");
        /* If actual-data variable doesn't already exist, create variable tracker,
           and write declaration for it: */
        assign_variable = test_state->find_var (assign_data_var);
        if (assign_variable == test_state->variable.end()) {
            test_state->make_var (assign_data_var);
            assign_variable = test_state->find_var (assign_data_var);
            prep_code.append (test_state->bplate->bplate_string[declare_big_string]);
            find_replace_1st ("$var", var_name, prep_code);
            temp_string = (char *) assign_variable->value;
            find_replace_1st ("$init", temp_string, prep_code);
            // Actual-data length:
            temp_string.assign (test_state->bplate->bplate_string[declare_int]);
            find_replace_1st ("static int", "static size_t", temp_string);
            prep_code.append (temp_string);
            find_replace_1st ("$var", length_var_name, prep_code);
            find_replace_1st ("$init", to_string(temp_string.length()), prep_code);
            // Offset (always 0 for now):
            find_replace_1st ("$offset", "0", prep_code);
        }
    } else {
        // Single string of two lines declaring string data and its length:
        var_name_suffix = "_set_data";
        length_var_name_suffix = "_set_length";
        if (set_data.n_set_vars > 0) {
            var_name_suffix += "_" + to_string(set_data.n_set_vars);
            length_var_name_suffix += "_" + to_string(set_data.n_set_vars);
        }
        var_name.assign (asset_info.get_name() + var_name_suffix);
        length_var_name.assign (asset_info.get_name() + length_var_name_suffix);
        prep_code = test_state->bplate->bplate_string[declare_string];
        find_replace_1st ("$var", var_name, prep_code);
        find_replace_1st ("$init", set_data.get(), prep_code);
        temp_string.assign (test_state->bplate->bplate_string[declare_int]);
        find_replace_1st ("static int", "static uint32_t", temp_string);
        prep_code.append (temp_string);
        find_replace_1st ("$var", length_var_name, prep_code);
        find_replace_1st ("$init", to_string(set_data.get().length()), prep_code);
    }
}

void sst_set_call::fill_in_command (void)
{
    string var_name, length_var_name, var_name_suffix, length_var_name_suffix,
           temp_string;

    // Fill in preceding comment:
    if (asset_info.how_asset_found == asset_search::created_new) {
        find_replace_1st ("$op", "Creating", call_code);
    } else {
        find_replace_1st ("$op", "Resetting", call_code);
    }
    if (asset_info.name_specified) {
        find_replace_1st ("$description", "\"" + asset_info.get_name() + ",\"",
                          call_code);
    } else {
        find_replace_1st ("$description",
                          "UID = " + to_string((long) asset_info.id_n), call_code);
    }
    if (set_data.string_specified) {
        find_replace_1st ("$data_source",
                          "\"" + set_data.get().substr (0, 10) + "...\"",
                          call_code);
    } else if (set_data.file_specified) {
        find_replace_1st ("$data_source", "from file " + set_data.file_path,
                          call_code);
    } else {
        find_replace_1st (" $data_source", "", call_code);
    }
    // Fill in the PSA command itself:
    if (assign_data_var_specified) {
        var_name.assign (assign_data_var + "_data");
        length_var_name.assign (assign_data_var + "_length");
    } else {
        var_name_suffix = "_set_data";
        if (set_data.n_set_vars > 0) {
            var_name_suffix += "_" + to_string(set_data.n_set_vars);
        }
        var_name.assign (asset_info.get_name() + var_name_suffix);
        length_var_name_suffix = "_set_length";
        if (set_data.n_set_vars > 0) {
            length_var_name_suffix += "_" + to_string(set_data.n_set_vars);
        }
        length_var_name.assign (asset_info.get_name() + length_var_name_suffix);
    }
    find_replace_1st ("$data", var_name, call_code);
    find_replace_1st ("$flags", flags_string, call_code);
    string id_string = to_string((long) asset_info.id_n);
    find_replace_1st ("$uid", id_string, call_code);
    find_replace_1st ("$length", length_var_name, call_code);
    // Figure out what expected results:
    calc_result_code();
}

/**********************************************************************************
   End of methods of class sst_set_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class sst_get_call follow:
**********************************************************************************/

sst_get_call::sst_get_call (tf_fuzz_info *test_state,    // (constructor)
                            long &call_ser_no,
                            asset_search how_asset_found)
                                 : sst_call(test_state, call_ser_no, how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[get_sst_call]);
    check_code.assign ("");
        // depends upon the particular usage;  will get it in fill_in_command()
}
sst_get_call::~sst_get_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool sst_get_call::copy_call_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;

    found_asset = resolve_asset (dont_create_asset, psa_asset_usage::active);
    if (asset_info.how_asset_found != asset_search::not_found) {
        // will be found for set calls, but not necessarily others
        asset_info.the_asset = reinterpret_cast<sst_asset*>(*found_asset);
           // Note:  Vector is of base-class type, but the assets *are* sst_asset.
        /* Locating the asset is all we need to do here;  copy_asset_to_call() will
           do the rest. */
    }
    return true;
    // TODO:  Shouldn't data be copied over?
}

bool sst_get_call::copy_asset_to_call (void)
{
    if (asset_info.the_asset != nullptr) {
        // will be found for set calls, but not necessarily others
        set_data.string_specified = asset_info.the_asset->set_data.string_specified;
        set_data.file_specified = asset_info.the_asset->set_data.file_specified;
        set_data.set (asset_info.the_asset->set_data.get());
        flags_string = asset_info.the_asset->flags_string;
        asset_info.id_n = asset_info.the_asset->asset_info.id_n;
        asset_info.asset_ser_no = asset_info.the_asset->asset_info.asset_ser_no;
        asset_info.name_specified = asset_info.the_asset->asset_info.name_specified;
        asset_info.the_asset->exp_data.n_exp_vars++;
        exp_data.n_exp_vars = asset_info.the_asset->exp_data.n_exp_vars;
    }
    return true;
}

void sst_get_call::fill_in_prep_code (void)
{
    string var_base, var_name, length_var_name, temp_string, var_name_suffix,
           expected;
    vector<variable_info>::iterator act_variable, exp_variable;

    if (!(print_data || hash_data)) {
        // Checking asset data verbatim against expected, so:
        if (exp_data.data_var_specified) {
            // Template specified a variable name to "check" against;  use that:
            var_base.assign (exp_data.data_var);
            exp_variable = test_state->find_var (var_base);
            if (exp_variable == test_state->variable.end()) {
                test_state->make_var (var_base);
                exp_variable = test_state->find_var (var_base);
                var_name = var_base + "_data";
                length_var_name = var_base + "_length";
                prep_code.append (test_state->bplate->bplate_string[declare_string]);
                find_replace_1st ("$var", var_name, prep_code);
                temp_string = (char *) exp_variable->value;
                find_replace_1st ("$init", temp_string, prep_code);
                // Expected-data length:
                temp_string.assign (test_state->bplate->bplate_string[declare_int]);
                find_replace_1st ("static int", "static size_t", temp_string);
                prep_code.append (temp_string);
                find_replace_1st ("$var", length_var_name, prep_code);
                find_replace_1st ("$init", to_string(temp_string.length()), prep_code);
            }
        } else {
            if (exp_data.data_specified) {
                // Checking against literal expected data:
                expected.assign (exp_data.data);
            } else {
                // Check against what we believe the asset to contain:
                expected.assign (set_data.get());
            }
            var_name_suffix = "_exp_data";
            if (exp_data.n_exp_vars > 0) {
                var_name_suffix =
                    var_name_suffix + "_" + to_string(exp_data.n_exp_vars);
            }
            var_name.assign (asset_info.get_name() + var_name_suffix);
            prep_code.assign(test_state->bplate->bplate_string[declare_string]);
            find_replace_1st("$var", var_name, prep_code);
            find_replace_1st("$init", expected, prep_code);
        }
    }
    // Actual data:
    if (assign_data_var_specified) {
        var_base.assign (assign_data_var);
    } else {
        var_base.assign (asset_info.get_name() + "_act");
    }
    var_name.assign (var_base + "_data");
    length_var_name.assign (var_base + "_length");
    /* If actual-data variable doesn't already exist, create variable tracker,
       and write declaration for it: */
    act_variable = test_state->find_var (var_base);
    if (act_variable == test_state->variable.end()) {
        test_state->make_var (var_base);
        act_variable = test_state->find_var (var_base);
        prep_code.append (test_state->bplate->bplate_string[declare_big_string]);
        find_replace_1st ("$var", var_name, prep_code);
        temp_string = (char *) act_variable->value;
        find_replace_1st ("$init", temp_string, prep_code);
        // Actual-data length:
        temp_string.assign (test_state->bplate->bplate_string[declare_int]);
        find_replace_1st ("static int", "static size_t", temp_string);
        prep_code.append (temp_string);
        find_replace_1st ("$var", length_var_name, prep_code);
        find_replace_1st ("$init", to_string(temp_string.length()), prep_code);
        // Offset (always 0 for now):
        find_replace_1st ("$offset", "0", prep_code);
    }
    // If hashing the (actual) data, then create a variable for that:
    if (hash_data && !act_variable->hash_declared) {
        var_name = var_base + "_hash";
        prep_code.append (test_state->bplate->bplate_string[declare_generic]);
            // where to put the hash of the data
        find_replace_1st ("$type", "uint32_t", prep_code);
        find_replace_1st ("$var", var_name, prep_code);
        find_replace_1st ("$init", "0", prep_code);  // for now...
        act_variable->hash_declared = true;
    }
}

void sst_get_call::fill_in_command (void)
{
    string exp_var_name, act_var_name, act_data_length, hash_var_name,
    id_string, var_name_suffix;

/* TODO:  Flesh-out/fix this (it was a good try/start, but not quite right):
    // Fill in preceding comment:
    if (asset_info.how_asset_found == asset_search::created_new) {
        find_replace_1st ("$op", "Creating", call_code);
    } else {
        find_replace_1st ("$op", "Resetting", call_code);
    }
    if (asset_info.name_specified) {
        find_replace_1st ("$description", "\"" + asset_info.get_name() + ",\"",
                          call_code);
    } else {
        find_replace_1st ("$description",
                          "UID = " + to_string((long) asset_info.id_n), call_code);
    }
    if (set_data.string_specified) {
        find_replace_1st ("$data_source",
                          "\"" + data.substr (0, 10) + "...\"",
                          call_code);
    } else if (set_data.file_specified) {
        find_replace_1st ("$data_source", "from file " + set_data.file_path,
                          call_code);
    } else {
        find_replace_1st (" $data_source", "", call_code);
    }
*/    // Fill in the call itself:
    if (print_data || hash_data) {
        // Dump to variable;  no data-check code needed:
        check_code.assign (test_state->bplate->bplate_string[get_sst_check]);
    } else {
        // Check either against literal or variable, so need data-check code too:
        check_code.assign (test_state->bplate->bplate_string[get_sst_check_all]);
    }
    /* Note:  Can fill in the check code identically between the dump-to-variable
              and check-data cases, because the boilerplate for the former is just an
              abbreviated version of the latter.  The find_replace_1st() calls for
              the check-data stuff will just simply not have any effect. */
    if (exp_data.data_var_specified) {
        // Check against data in variable:
        exp_var_name.assign (exp_data.data_var);
    } else {
        var_name_suffix = "_exp_data";
        if (exp_data.n_exp_vars > 0) {
            var_name_suffix =
                var_name_suffix + "_" + to_string(exp_data.n_exp_vars);
        }
        exp_var_name.assign (asset_info.get_name() + var_name_suffix);
    }
    if (assign_data_var_specified) {
        act_var_name.assign (assign_data_var + "_data");
        act_data_length.assign (assign_data_var + "_length");
    } else {
        act_var_name.assign (asset_info.get_name() + "_act_data");
        act_data_length.assign (asset_info.get_name() + "_act_length");
    }

    id_string = to_string((long) asset_info.id_n);
    // Fill in the PSA command itself:
    find_replace_1st ("$uid", id_string, call_code);
    find_replace_all ("$length", to_string(set_data.get().length()), call_code);
    find_replace_1st ("$offset", "0", call_code);
    find_replace_1st ("$exp_data", exp_var_name, call_code);
    find_replace_all ("$act_data", act_var_name, call_code);
    find_replace_all ("$act_length", act_data_length, call_code);
    // Perform most of the same substitutions in the check_code:
// TODO:  Make data checks contingent upon the PSA call itself passing?
    find_replace_1st ("$offset", "0", check_code);
    find_replace_1st ("$exp_data", exp_var_name, check_code);
    find_replace_all ("$act_data", act_var_name, check_code);
    find_replace_all ("$length", act_data_length, check_code);
    if (print_data) {
        check_code.append (test_state->bplate->bplate_string[test_log]);
        find_replace_1st ("$message", act_var_name, check_code);
    }
    if (hash_data) {
        hash_var_name.assign (asset_info.get_name() + "_hash");
            // this is where to put the hash of the data
        check_code.append (test_state->bplate->bplate_string[get_sst_hash]);
        find_replace_all ("$act_data_var", act_var_name, check_code);
        find_replace_all ("$hash_var", hash_var_name, check_code);
    }
    // Figure out what expected results:
    calc_result_code();  // this only fills $expect check_code
    // Fill in expected data, actual data, and length:
}

/**********************************************************************************
   End of methods of class sst_get_call.
**********************************************************************************/


/**********************************************************************************
   Methods of class sst_remove_call follow:
**********************************************************************************/

sst_remove_call::sst_remove_call (tf_fuzz_info *test_state,    // (constructor)
                                  long &call_ser_no,
                                  asset_search how_asset_found)
                                 : sst_call(test_state, call_ser_no, how_asset_found)
{
    // Copy the boilerplate text into local buffers:
    prep_code.assign ("");
    call_code.assign (test_state->bplate->bplate_string[remove_sst]);
    check_code.assign (test_state->bplate->bplate_string[remove_sst_check]);
}
sst_remove_call::~sst_remove_call (void)
{
    return;  // just to have something to pin a breakpoint onto
}

bool sst_remove_call::copy_call_to_asset (void)
{
    vector<psa_asset*>::iterator found_asset;

    found_asset = resolve_asset (dont_create_asset, psa_asset_usage::all);
    if (asset_info.how_asset_found != asset_search::not_found) {
        asset_info.the_asset = reinterpret_cast<sst_asset*>(*found_asset);
           // Note:  Vector is of base-class type, but the assets *are* sst_asset.
        if (asset_info.how_asset_found == asset_search::found_active) {
            // Delete asset;  move it from active vector to deleted vector:
            test_state->deleted_sst_asset.push_back (asset_info.the_asset);
            test_state->active_sst_asset.erase (found_asset);
        }  /* if not active, we'll deem the call expected to fail. */
    }
    return true;
}

bool sst_remove_call::copy_asset_to_call (void)
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

void sst_remove_call::fill_in_prep_code (void)
{
    // No prep-code.
    return;  // just to have something to pin a breakpoint onto
}

void sst_remove_call::fill_in_command (void)
{
    // Fill in the call:
    string id_string = to_string((long) asset_info.id_n);
    find_replace_1st ("$uid", id_string, call_code);
    // Fill in expected results:
    calc_result_code();  // this only fills $expect check_code
}

/**********************************************************************************
   End of methods of class sst_remove_call.
**********************************************************************************/

