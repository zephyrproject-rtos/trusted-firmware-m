/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CRYPTO_CALL_HPP
#define CRYPTO_CALL_HPP

#include <string>
#include <vector>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "psa_call.hpp"
#include "crypto_asset.hpp"
*/

template<class T> bool copy_call_to_asset_t (psa_call *call, bool create_asset_bool)
{
    vector<psa_asset*>::iterator found_asset;

    found_asset = call->resolve_asset (create_asset_bool, psa_asset_usage::all);
    // Copy over everything relevant:
    if (call->asset_info.how_asset_found != asset_search::not_found) {
        call->asset_info.the_asset = reinterpret_cast<T>(*found_asset);
            /* Note:  The vector is base-class, but the assets in this list
                      themselves *really are* policy_asset-type objects. */
        int i = call->asset_info.the_asset->set_data.n_set_vars;  // save this
        call->asset_info.the_asset->exp_data.data = call->exp_data.data;
        call->asset_info.the_asset->set_data = call->set_data;
        call->asset_info.the_asset->set_data.n_set_vars = call->set_data.n_set_vars = ++i;
        call->asset_info.the_asset->policy = call->policy;
        call->asset_info.the_asset->handle_str = call->asset_info.get_name();
        if (call->asset_info.how_asset_found == asset_search::created_new) {
            call->asset_info.the_asset->asset_info.name_specified
                = call->asset_info.name_specified;
            call->asset_info.the_asset->asset_info.set_name (call->asset_info.get_name());
            call->asset_info.the_asset->asset_info.asset_ser_no
                = call->asset_info.asset_ser_no;
            call->asset_info.the_asset->asset_info.id_n = call->asset_info.id_n;
        }
    }
    return true;
}


using namespace std;

class policy_call : public crypto_call
{
public:
    // Data members:  // (low value in hiding these behind setters and getters)
    // Methods:
        vector<psa_asset*>::iterator resolve_asset (bool create_asset_bool,
                                                    psa_asset_usage where);
        void policy_fill_in_prep_code (void);
        policy_call (tf_fuzz_info *test_state, long &asset_ser_no,
                    asset_search how_asset_found);  // (constructor)
        ~policy_call (void);

protected:
    // Data members:
    // Methods:
//        void calc_result_code (void);  for *now* keep this in crypto_call::

private:
    // Data members:
    // Methods:
};


class key_call : public crypto_call
{
public:
    // Data members:  // (low value in hiding these behind setters and getters)
    // Methods:
        vector<psa_asset*>::iterator resolve_asset (bool create_asset_bool,
                                                    psa_asset_usage where);
        key_call (tf_fuzz_info *test_state, long &asset_ser_no,
                    asset_search how_asset_found);  // (constructor)
        ~key_call (void);

protected:
    // Data members:
    // Methods:
//        void calc_result_code (void);  for *now* keep this in crypto_call::

private:
    // Data members:
    // Methods:
};


/**********************************************************************************
   Derived classes of class policy_call follow:
**********************************************************************************/


class init_policy_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        init_policy_call (tf_fuzz_info *test_state, long &asset_ser_no,
                          asset_search how_asset_found);  // (constructor)
        ~init_policy_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class reset_policy_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        reset_policy_call (tf_fuzz_info *test_state, long &asset_ser_no,
                           asset_search how_asset_found);  // (constructor)
        ~reset_policy_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class set_policy_usage_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        set_policy_usage_call (tf_fuzz_info *test_state, long &asset_ser_no,
                               asset_search how_asset_found);  // (constructor)
        ~set_policy_usage_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


/* set_policy_usage_call(), above, gives a policy particular attribute, and resets
   all others.  add_policy_usage_call(), below, ORs in an additional usage
   attribute, to whatever is already in there. */

class add_policy_usage_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        add_policy_usage_call (tf_fuzz_info *test_state, long &asset_ser_no,
                               asset_search how_asset_found);  // (constructor)
        ~add_policy_usage_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class set_policy_lifetime_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        set_policy_lifetime_call (tf_fuzz_info *test_state, long &asset_ser_no,
                                  asset_search how_asset_found);  // (constructor)
        ~set_policy_lifetime_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class set_policy_algorithm_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        set_policy_algorithm_call (tf_fuzz_info *test_state, long &asset_ser_no,
                                   asset_search how_asset_found);  // (constructor)
        ~set_policy_algorithm_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class set_policy_size_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        set_policy_size_call (tf_fuzz_info *test_state, long &asset_ser_no,
                              asset_search how_asset_found);  // (constructor)
        ~set_policy_size_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class set_policy_type_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        set_policy_type_call (tf_fuzz_info *test_state, long &asset_ser_no,
                              asset_search how_asset_found);  // (constructor)
        ~set_policy_type_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class get_policy_lifetime_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        get_policy_lifetime_call (tf_fuzz_info *test_state, long &asset_ser_no,
                                  asset_search how_asset_found);  // (constructor)
        ~get_policy_lifetime_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class get_policy_size_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        get_policy_size_call (tf_fuzz_info *test_state, long &asset_ser_no,
                              asset_search how_asset_found);  // (constructor)
        ~get_policy_size_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class get_policy_type_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        get_policy_type_call (tf_fuzz_info *test_state, long &asset_ser_no,
                              asset_search how_asset_found);  // (constructor)
        ~get_policy_type_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class get_policy_algorithm_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        get_policy_algorithm_call (tf_fuzz_info *test_state, long &asset_ser_no,
                                   asset_search how_asset_found);  // (constructor)
        ~get_policy_algorithm_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class get_policy_usage_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        get_policy_usage_call (tf_fuzz_info *test_state, long &asset_ser_no,
                               asset_search how_asset_found);  // (constructor)
        ~get_policy_usage_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


/* This is not actually a PSA call;  it just emits code to print out, to the log,
   information about the usage info retrieved in a get_policy_usage_call above. */
class print_policy_usage_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        print_policy_usage_call (tf_fuzz_info *test_state, long &asset_ser_no,
                               asset_search how_asset_found);  // (constructor)
        ~print_policy_usage_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class get_key_policy_call : public policy_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        get_key_policy_call (tf_fuzz_info *test_state, long &asset_ser_no,
                         asset_search how_asset_found);  // (constructor)
        ~get_key_policy_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


/**********************************************************************************
   End of derived classes of class policy_call.
**********************************************************************************/


/**********************************************************************************
   Derived classes of class key_call follow:
**********************************************************************************/


class generate_key_call : public key_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        generate_key_call (tf_fuzz_info *test_state, long &asset_ser_no,
                         asset_search how_asset_found);  // (constructor)
        ~generate_key_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class create_key_call : public key_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        create_key_call (tf_fuzz_info *test_state, long &asset_ser_no,
                         asset_search how_asset_found);  // (constructor)
        ~create_key_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class copy_key_call : public key_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        copy_key_call (tf_fuzz_info *test_state, long &asset_ser_no,
                         asset_search how_asset_found);  // (constructor)
        ~copy_key_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class read_key_data_call : public key_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        read_key_data_call (tf_fuzz_info *test_state, long &asset_ser_no,
                            asset_search how_asset_found);  // (constructor)
        ~read_key_data_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class remove_key_call : public key_call
{
public:
    // Data members:
    // Methods:
        bool copy_call_to_asset (void);
        void fill_in_prep_code (void);
        void fill_in_command (void);
        remove_key_call (tf_fuzz_info *test_state, long &asset_ser_no,
                         asset_search how_asset_found);  // (constructor)
        ~remove_key_call (void);


protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


/**********************************************************************************
   End of derived classes of class key_call.
**********************************************************************************/

#endif  // CRYPTO_CALL_HPP
