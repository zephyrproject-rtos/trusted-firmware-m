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
#include "sst_call.hpp"
#include "sst_template_line.hpp"
#include "sst_asset.hpp"


/**********************************************************************************
   Methods of class set_sst_template_line follow:
**********************************************************************************/

//**************** set_sst_template_line methods ****************

string set_sst_template_line::rand_creation_flags (void)
{
    return ((rand() % 2) == 1)?
        "PSA_STORAGE_FLAG_WRITE_ONCE" : "PSA_STORAGE_FLAG_NONE";
    /* TODO:  There's seems to be some uncertainty as to how many creation-flag
       values are actually used, so for now only using PSA_STORAGE_FLAG_WRITE_ONCE
       and PSA_STORAGE_FLAG_NONE.  If there are more in real-world use, then
       resurrect the commented-out code below to assign them:
    string result = "";
    const int most_flags = 3,
    int n_flags = (rand() % most_flags);

    for (int i = 0;  i < ;  i < n_flags;  ++i) {
        switch (rand() % 5) {
            case 0:
                result += "PSA_STORAGE_FLAG_WRITE_ONCE";
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
        }
        if (i < n_flags-1)
            result += " | ";
    }
    if (result == "") result = "PSA_STORAGE_FLAG_NONE";
*/
}

set_sst_template_line::set_sst_template_line (tf_fuzz_info *resources)
    : sst_template_line (resources)  // constructor)
{
    // Creation flags:
    flags_string = rand_creation_flags();
}


// Default destructor:
set_sst_template_line::~set_sst_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)



/**********************************************************************************
   End of methods of class set_sst_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class read_sst_template_line follow:
**********************************************************************************/

read_sst_template_line::read_sst_template_line (tf_fuzz_info *resources)
    : sst_template_line (resources)  // (constructor)
{
    char gibberish_buffer[4096];  string databuff;
    int data_length;
    set_data.string_specified = (rand()%2) == 1?  true : false;

    // Go ahead and create a literal-data string even if not needed:
    data_length = test_state->gibbergen.pick_sentence_len();
    test_state->gibbergen.sentence (gibberish_buffer, gibberish_buffer + data_length);
    databuff = gibberish_buffer;  set_data.set (databuff);

    set_data.file_specified = (!set_data.string_specified && (rand()%2) == 1)?  true : false;
    set_data.file_path = "";  // can't really devise a random path
}

// Default destructor:
read_sst_template_line::~read_sst_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class read_sst_template_line.
**********************************************************************************/


/**********************************************************************************
   Methods of class remove_sst_template_line follow:
**********************************************************************************/

remove_sst_template_line::remove_sst_template_line (tf_fuzz_info *resources)
    : sst_template_line (resources)  // (constructor)
{
    is_remove = true;  // template_line's constructor defaults this to false
}

// Default destructor:
remove_sst_template_line::~remove_sst_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class remove_sst_template_line.
**********************************************************************************/

