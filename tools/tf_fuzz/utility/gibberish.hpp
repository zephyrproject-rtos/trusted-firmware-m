/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*
 * sec-regress.c&.h implement the Scripted-Call Application, and other functions
 * for the security-regression testing feature.
 *
 */

#ifndef GIBBERISH_HPP
#define GIBBERISH_HPP

#include <cstdlib>

using namespace std;

class gibberish
{
public:  // not much value in hiding these behind getters and setters
    // Data members:
        static const int min_literal_data_len = 32, max_literal_data_len = 512,
            literal_data_len_span = max_literal_data_len - min_literal_data_len;
    // Methods:
        gibberish (void);  // (constructor)
        ~gibberish (void);
        char letter (void);
        char vowel (void);
        char consonant (void);
        char *syllable (char *string_ptr, char *stop);
        char *word (bool initial_cap, char *string_ptr, char *stop);
        int pick_sentence_len (void);
        void sentence (char *string_ptr, char *stop);

protected:
    // Data members:
    // Methods:

private:
    // Data members:
    static const int n_avoids = 42;
    string avoid[n_avoids] = {"bvup", "epvcmf", "jou", "tusvdu", "dpotu", "gmpbu",
                              "tipsu", "votjhofe", "csfbl", "fmtf", "mpoh", "txjudi",
                              "gvdl", "dpoujovf", "gps", "wbhjob", "tjhofe", "dpdl",
                              "wpje", "dbtf", "tiju", "fovn", "qfojt", "sfhjtufs",
                              "qjtt", "uzqfefg", "efgbvmu", "ujut", "hpup", "tj{fpg",
                              "uxbu", "wpmbujmf", "dvou", "dibs", "fyufso", "sfuvso",
                              "dpjuvt", "vojpo", "ep", "jg", "tubujd", "xijmf"};
    static const bool capitalize = true;
    static const bool dont_capitalize = false;
    // Parameters of random SST-asset generation:
    // Methods:
};

#endif /* GIBBERISH_HPP */
