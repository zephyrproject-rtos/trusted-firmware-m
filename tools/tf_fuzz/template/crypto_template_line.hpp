/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef CRYPTO_TEMPLATE_LINE_HPP
#define CRYPTO_TEMPLATE_LINE_HPP

#include <cstdint>

/* This project's header files #including other project headers quickly becomes
   unrealistically complicated.  The only solution is for each .cpp to include
   the headers it needs.
#include "psa_asset.hpp"
//class psa_asset;  // just need a forward reference
#include "template_line.hpp"
#include "psa_call.hpp"
*/
using namespace std;


class set_policy_template_line : public policy_template_line
{
public:
    // Data members:
    // Methods:
        void setup_call (set_data_info set_info, bool random_data,
                         bool fill_in_template, bool create_call,
                         template_line *temLin, tf_fuzz_info *rsrc) {
            /* If the name of the primary asset is known, then add calls at
               random, otherwise append to end.  If not, then we do not know
               what asset-name barrier to search for. */
            bool add_to_end_bool = (random_asset != psa_asset_usage::all);

            // Add the calls of interest:
            define_call<init_policy_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_random_after_barrier, yes_set_barrier  );
            define_call<reset_policy_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_random_after_barrier, yes_set_barrier  );
            policy_info.usage_string.assign ("0");  // clear out all usages
            define_call<set_policy_usage_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_random_after_barrier, yes_set_barrier  );
            if (policy_info.exportable) {
                policy_info.usage_string.assign ("PSA_KEY_USAGE_EXPORT");
                define_call<add_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end_bool, dont_set_barrier  );
            }
            if (policy_info.copyable) {
                policy_info.usage_string.assign ("PSA_KEY_USAGE_COPY");
                define_call<add_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end_bool, dont_set_barrier  );
            }
            if (policy_info.can_encrypt) {
                policy_info.usage_string.assign ("PSA_KEY_USAGE_ENCRYPT");
                define_call<add_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end_bool, dont_set_barrier  );
            }
            if (policy_info.can_decrypt) {
                policy_info.usage_string.assign ("PSA_KEY_USAGE_DECRYPT");
                define_call<add_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end_bool, dont_set_barrier  );
            }
            if (policy_info.can_sign) {
                policy_info.usage_string.assign ("PSA_KEY_USAGE_SIGN");
                define_call<add_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end_bool, dont_set_barrier  );
            }
            if (policy_info.can_verify) {
                policy_info.usage_string.assign ("PSA_KEY_USAGE_VERIFY");
                define_call<add_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end_bool, dont_set_barrier  );
            }
            if (policy_info.derivable) {
                policy_info.usage_string.assign ("PSA_KEY_USAGE_DERIVE");
                define_call<add_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end_bool, dont_set_barrier  );
            }
            define_call<set_policy_lifetime_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end_bool, dont_set_barrier  );
            define_call<set_policy_algorithm_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end_bool, dont_set_barrier  );
            define_call<set_policy_type_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end, yes_set_barrier  );
            define_call<set_policy_size_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end, yes_set_barrier  );
        }
        set_policy_template_line (tf_fuzz_info *resources);  // (constructor)
        ~set_policy_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class read_policy_template_line : public policy_template_line
{
public:
    // Data members:
    // Methods:
        void setup_call (set_data_info set_info, bool random_data,
                         bool fill_in_template, bool create_call,
                         template_line *temLin, tf_fuzz_info *rsrc) {
            /* If the name of the primary asset is known, then add calls at
               random, otherwise append to end.  If not, then we do not know
               what asset-name barrier to search for. */
            bool add_to_end_bool = (random_asset != psa_asset_usage::all);

            if (policy_info.get_policy_from_key) {
                define_call<get_key_policy_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end_bool, yes_set_barrier  );
            }
            define_call<get_policy_usage_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end_bool, yes_set_barrier  );
            define_call<get_policy_lifetime_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end_bool, dont_set_barrier  );
            define_call<get_policy_algorithm_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end_bool, dont_set_barrier  );
            define_call<get_policy_type_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end_bool, dont_set_barrier  );
            define_call<get_policy_size_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end_bool, dont_set_barrier  );
            if (print_data) {
                /* Printing data, so we need to read and filter usage for various
                   attributes, one by one: */
                policy_info.print_usage_true_string.assign ("key can be exported.");
                policy_info.print_usage_false_string.assign ("key cannot be exported.");
                policy_info.usage_string.assign ("PSA_KEY_USAGE_EXPORT");
                define_call<print_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, dont_set_barrier  );
                policy_info.print_usage_true_string.assign ("key can be copied.");
                policy_info.print_usage_false_string.assign ("key cannot be copied.");
                policy_info.usage_string.assign ("PSA_KEY_USAGE_COPY");
                define_call<print_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, dont_set_barrier  );
                policy_info.print_usage_true_string.assign ("key works for encryption.");
                policy_info.print_usage_false_string.assign ("key is not for encryption.");
                policy_info.usage_string.assign ("PSA_KEY_USAGE_ENCRYPT");
                define_call<print_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, dont_set_barrier  );
                policy_info.print_usage_true_string.assign ("key works for decyption.");
                policy_info.print_usage_false_string.assign ("key is not for decyption.");
                policy_info.usage_string.assign ("PSA_KEY_USAGE_DECRYPT");
                define_call<print_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, dont_set_barrier  );
                policy_info.print_usage_true_string.assign ("key works for signing.");
                policy_info.print_usage_false_string.assign ("key is not for signing.");
                policy_info.usage_string.assign ("PSA_KEY_USAGE_SIGN");
                define_call<print_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, dont_set_barrier  );
                policy_info.print_usage_true_string.assign ("key can be used to verify.");
                policy_info.print_usage_false_string.assign ("key not for verify.");
                policy_info.usage_string.assign ("PSA_KEY_USAGE_VERIFY");
                define_call<print_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, dont_set_barrier  );
                policy_info.print_usage_true_string.assign ("key can derive other keys.");
                policy_info.print_usage_false_string.assign ("key cannot derive other keys.");
                policy_info.usage_string.assign ("PSA_KEY_USAGE_DERIVE");
                define_call<print_policy_usage_call> (set_data, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, yes_set_barrier  );
            }
        }
        read_policy_template_line (tf_fuzz_info *resources);  // (constructor)
        ~read_policy_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};


class set_key_template_line : public key_template_line
{
public:
   // Data members:
    // Methods:
        void setup_call (set_data_info set_info, bool random_data,
                         bool fill_in_template, bool create_call,
                         template_line *temLin, tf_fuzz_info *rsrc) {
            if (policy_info.copy_key) {
                // Copying a key:
                define_call<copy_key_call> (set_info, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, yes_set_barrier);
            } else if (set_data.string_specified || set_data.random_data) {
                // Key data (key material) supplied:
                define_call<create_key_call> (set_info, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, yes_set_barrier);
            } else {
                // Generate from scratch:
                define_call<generate_key_call> (set_info, random_data,
                                fill_in_template, create_call, temLin, rsrc,
                                add_to_end, yes_set_barrier);
            }
        }
        set_key_template_line (tf_fuzz_info *resources);  // (constructor)
        ~set_key_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class read_key_template_line : public key_template_line
{
public:
    // Data members:
    // Methods:
        void setup_call (set_data_info set_info, bool random_data,
                         bool fill_in_template, bool create_call,
                         template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<read_key_data_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end, yes_set_barrier);
        }
        read_key_template_line (tf_fuzz_info *resources);  // (constructor)
        ~read_key_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

class remove_key_template_line : public key_template_line
{
public:
    // Data members:
    // Methods:
        void setup_call (set_data_info set_info, bool random_data,
                         bool fill_in_template, bool create_call,
                         template_line *temLin, tf_fuzz_info *rsrc) {
            define_call<remove_key_call> (set_data, random_data,
                            fill_in_template, create_call, temLin, rsrc,
                            add_to_end, yes_set_barrier);
        }
        remove_key_template_line (tf_fuzz_info *resources);  // (constructor)
        ~remove_key_template_line (void);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    // Methods:
};

#endif  // #ifndef CRYPTO_TEMPLATE_LINE_HPP
