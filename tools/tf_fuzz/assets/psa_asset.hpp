/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef PSA_ASSET_HPP
#define PSA_ASSET_HPP

#include <string>
#include <vector>
#include <cstdint>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs. */

using namespace std;

class psa_asset
{
public:
    /* Data members -- not all PSA assets have all of these, but they need to be
       accessible polymorphically via a psa_asset iterator: */
        set_data_info set_data;
            /* For a PSA-asset tracker, this is really more about an asset's
               on-going, real-time asset data value than about *setting* its data
               value.  On a template_line or a psa_call, it's about setting its
               value at one particular time. */
        expect_info exp_data;
            /* For now at least, this is here only for its n_exp_vars member, to
               keep track of how many expected-data variables in the test. */
        asset_name_id_info asset_info;  // everything about the asset(s) for this line
        key_policy_info policy;  // (specific to crypto, but have to put this here)
        vector<int> template_ref;
            // list of template line #s that reference this asset
        vector<psa_call> call_ref;  // list of PSA calls that reference this asset
        string handle_str;  // the text name of the key's "handle"
        bool asset_name_specified;
            /* true if the template specified the asset_name, as opposed to us
               having inferred it. */
    // Methods:
        void set_name (string set_val);
        string get_name (void);
        virtual bool simulate (void);
            /* simulate() tells this asset to react to its current state information.
               Initially, this won't really do much, but will allow assets to react
               to each other, if that is relevant.  It returns true if anything
               in the state of the asset changed, in which case all assets' simulate()
               methods will be invoked again to react again.  That will repeat until
               all assets read a quiescent state. */
        psa_asset();  // (constructor)
        ~psa_asset();

protected:
    // Data members:
        // These are initially copied over from the call (or possibly template line):
        string data;  // String describing current data value.
        string asset_name;  // human-meaningful name
        static long unique_id_counter;  // counts off unique IDs for assets
    // Methods:

private:
    // Data members:
    // Methods:
};

#endif  // PSA_ASSET_HPP
