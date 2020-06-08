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
    return;  // just to have something to pin a breakpoint onto
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

// Default destructor:
read_key_template_line::~read_key_template_line (void)
{
    return;  // just to have something to pin a breakpoint onto
}

// (Default constructor not used)


/**********************************************************************************
   End of methods of class read_key_template_line.
**********************************************************************************/
