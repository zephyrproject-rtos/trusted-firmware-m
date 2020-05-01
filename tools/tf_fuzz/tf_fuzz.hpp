/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef TF_FUZZ_HPP
#define TF_FUZZ_HPP

#include <string>
#include <vector>
#include <iostream>
#include <fstream>


/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "psa_call.hpp"
#include "sst_asset.hpp"
#include "crypto_asset.hpp"
#include "boilerplate.hpp"
*/

using namespace std;

// class tf_fuzz_info mostly just groups together everything needed generate the test.
class tf_fuzz_info
{
    /* In creating a test, TF-Fuzz parses the test template, creating a vector of
       PSA-call-tracker objects.  TF-Fuzz then performs a simulation of those calls
       -- simulation only in enough detail to predict expected results of those PSA
       calls. After that simulation phase, write_test() writes it all out.  The
       process of creating these structures also requires the boilerplate text
       strings. */

public:
    // Data members (this class is mostly just to group stuff together, so public):
        vector<string> prep_code;  // variable declarations to write out to test file
        vector<psa_call*> calls;
            /* the calls to perform:  Note:  must be vector *psa_call;  a vector of
               psa_call does not allow (run-time) polymorphism. */
        boilerplate *bplate;  // the boilerplate text for building the test
        gibberish gibbergen;  // the gibberish asset-data generator
        crc32 hashgen;  // simple 32-bit LFSR-based hashing generator
        /* Note:  The following asset-lists are kept in base-class type to allow a
                  common template-line processing function in tf_fuzz_grammar.y. */
        vector<psa_asset*> active_sst_asset;  // list of known and usable SST assets
        vector<psa_asset*> deleted_sst_asset;  // deleted SST assets
        vector<psa_asset*> invalid_sst_asset;  // SST assets with invalid attributes
        vector<psa_asset*> active_key_asset;  // list of known and usable keys
        vector<psa_asset*> deleted_key_asset;  // deleted keys
        vector<psa_asset*> invalid_key_asset;  // keys with invalid attributes
        vector<psa_asset*> active_policy_asset;  // list of known, usable policies
        vector<psa_asset*> deleted_policy_asset;  // deleted policies
        vector<psa_asset*> invalid_policy_asset;  // policies with invalid attrs
        /* The "variable" vector tracks variables in the generated code.  Actually,
           it tracks variables explicitly named in the test template, and actual-data
           holder variables from reading an asset.  It does not track asset set-data
           or asset expect-data variables, because there are multiple versions of
           those, and it's easier to just to "count off" those multiple versions.
           Notice that, unlike the above vectors, the variable vector is not a
           vector of pointers to variable_info objects, but of the objects themselves.
           This is because polymorphism is not of concern here. */
        vector<variable_info> variable;
        string test_purpose;  // what the test tests
        long rand_seed;  // the original random seed, whether passed in or defaulted
        string template_file_name, test_output_file_name;
        FILE *template_file;
            /* handle to the test-template input file.  Unfortunately I can't seem to
               get lex/yacc to understand C++ file references, probably because I'm
               "extern C"ing the Lex content (Yacc/Bison turns out to be a lot easier
               to coerce into generating C++ code than (F)Lex). */
        ofstream output_C_file;  // handle to the output C test file
        bool verbose_mode;  // true to "think aloud"
        bool include_hashing_code;  // true to instantiate the hashing code
    // Methods:
        asset_search find_or_create_sst_asset (
            psa_asset_search criterion,  // what to search on
            psa_asset_usage where,  // where to search
            string target_name,  // ignored if not searching on name
            uint64_t target_id,  // ignored if not searching on ID (e.g., SST UID)
            long &serial_no,  // search by asset's unique serial number
            bool create_asset,  // true to create the asset if it doesn't exist
            vector<psa_asset*>::iterator &asset  // returns a pointer to asset
        );
        asset_search find_or_create_key_asset (
            psa_asset_search criterion,  // what to search on
            psa_asset_usage where,  // where to search
            string target_name,  // ignored if not searching on name
            uint64_t target_id,  // ignored if not searching on ID (e.g., SST UID)
            long &serial_no,  // search by asset's unique serial number
            bool create_asset,  // true to create the asset if it doesn't exist
            vector<psa_asset*>:: iterator &asset  // returns iterator to asset
        );
        asset_search find_or_create_policy_asset (
            psa_asset_search criterion,  // what to search on
            psa_asset_usage where,  // where to search
            string target_name,  // ignored if not searching on name
            uint64_t target_id,  // also ignored if not searching on ID (e.g., SST UID)
            long &serial_no,  // search by asset's unique serial number
            bool create_asset,  // true to create the asset if it doesn't exist
            vector<psa_asset*>::iterator &asset  // returns iterator to asset
        );
        asset_search find_or_create_psa_asset (
            psa_asset_type asset_type,  // what type of asset to find
            psa_asset_search criterion,  // what to search on
            psa_asset_usage where,  // where to search
            string target_name,  // ignored if not searching on name
            uint64_t target_id,  // also ignored if not searching on ID (e.g., SST UID)
            long &serial_no,  // search by asset's unique serial number
            bool create_asset,  // true to create the asset if it doesn't exist
            vector<psa_asset*>::iterator &asset  // returns iterator to asset
        );
        vector<variable_info>::iterator find_var (string var_name);
        bool make_var (string var_name);
        void teardown_test(void);  // removes any PSA resources used in the test
        void simulate_calls (void);
            // goes through generated calls calculating expected results
        void write_test (void);  // writes out the test's .c file
        void parse_cmd_line_params (int argc, char* argv[]);
            // parse command-line parameters, and open files
        tf_fuzz_info (void);  // (constructor)
        ~tf_fuzz_info (void);

protected:
    // Data members:
        vector<string> teardown_calls;
            // list of PSA commands to remove assets left over upon test completion
    // Methods:

private:
    // Data members:
    // Methods:
};


/*--------------------------------------------------------------
   Helper functions:
--------------------------------------------------------------*/


template<typename CALL_TYPE>
void define_call (set_data_info set_data, bool random_data, bool fill_in_template,
                 bool create_call, template_line *temLin, tf_fuzz_info *rsrc
) {
    CALL_TYPE *the_call;
    gibberish gib;
    char gib_buff[1000];
    string t_string;

    if (fill_in_template) {
        if (set_data.literal_data_not_file) {
            if (random_data) {
                int rand_data_length = 12 + (rand() % 800);
                gib.sentence (gib_buff, gib_buff + rand_data_length - 1);
                t_string = gib_buff;
                temLin->set_data.set_calculated (t_string);
            }
        } else {
            if (set_data.file_path == "") {  // catch the most likely failure at least!
                cerr << "Error:  Tool-internal:  Please report error "
                     << "#407 to the TF-Fuzz developers." << endl;
                exit(407);
            }
            temLin->set_data.set_file (set_data.file_path);
                // set in sst_asset_make_file_path
        }
    }
    if (create_call) {
        the_call = new CALL_TYPE (rsrc, temLin->call_ser_no,
                                  temLin->asset_info.how_asset_found);
        rsrc->calls.push_back(the_call);  // (note: this is not a memory leak!)
//        temLin->copy_template_to_asset();  TODO:  Remove this "note-to-self" comment
        if (!temLin->copy_template_to_call()) {
            cerr << "Error:  Tool-internal:  Please report error "
                 << "#402 to the TF-Fuzz developers." << endl;
            exit(402);
        }
    }
}

#endif  // #ifndef TF_FUZZ_HPP
