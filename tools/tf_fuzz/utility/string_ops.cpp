/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "string_ops.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

// Replace first occurrence of find_str within orig of replace_str:
size_t find_replace_1st (const string &find_str, const string &replace_str,
                         string &orig) {
    size_t where = 0;
    where = orig.find(find_str, where);
    if (where != string::npos) {
        orig.replace(where, find_str.length(), replace_str);
    }
    return where;
}

// Replace all occurrences of find_str in "this" string, with replace_str:
size_t find_replace_all (const string &find_str, const string &replace_str,
                         string &orig) {
    size_t where = 0;
    do {
        where = orig.find(find_str, where);
        if (where != string::npos) {
            orig.replace(where, find_str.length(), replace_str);
        }
    } while (where != string::npos);
    return where;
}


string formalize (string input, string prefix) {
    // First capitalize the input string:
    for (auto cp = input.begin();  cp < input.end();  cp++) {
        *cp = (char) toupper (*cp);
    }
    // If it already begins with the prefix...
    if (input.substr (0, prefix.length()) == prefix) {
        // then return it as capitalized:
        return input;
    } else {
        // If not, prefix it with that prefix:
        return prefix + input;
    }
}

/* This implementation assumes ASCII character encoding and no "special characters" --
   loosely speaking, "English." */
string string_or_hex (string input, int clump_size) {
    uint32_t n_alphanum = 0;  // the number of alphanumeric characters
    bool prose = true;  // the string is alphanumeric, space, or common punctuation
    ostringstream hex_stream;

    for (auto cp = input.begin();  cp < input.end();  cp++) {
        if ((int) *cp < 0) {
            prose = false;
            break;
        }
        if (   (*cp >= '0' && *cp <= '9')
            || (*cp >= 'a' && *cp <= 'z') || (*cp >= 'A' && *cp <= 'Z')
            || (*cp == ' ') || (*cp == ',') || (*cp == '.')
            || (*cp == '?') || (*cp == '!')
           ) {
            n_alphanum++;
        }
    }
    if (   prose  // so far at least!
        && (  (input.length() - n_alphanum) // number of chars that are not "English"
            < (input.length() >> 3)  // 1/8 of the length of the string
       )   ) {
        return input;  // take it at face-value
    }
    // It's not run-of-the-mill text, so create a hex string:
    int i = 0;
    for (auto cp = input.begin();  cp < input.end();  cp++) {
        hex_stream << setfill('0') << setw(2) << hex << (((unsigned) *cp) & 0xff);
        if (++i >= clump_size) {
            hex_stream << " ";
            i = 0;
        }
    }
    return hex_stream.str();
}

string binary_from_hex (string input) {
    stringstream hex_stream;
    uint8_t aByte;  // each byte as we grab it
    string result = "";
    string holder_string = "";

    hex_stream.str(input);
    hex_stream.width(2);
    while (!hex_stream.eof()) {
        hex_stream >> skipws >> setw(2) >> holder_string;
        try {   // TODO:  May not catch all bad-chars.
            aByte = stoi(holder_string, 0, 16);
        }
        catch (const invalid_argument &arg_err) {
            cerr << "Error 2345:  Non-hexadecimal character found in binary-data string ("
                 << arg_err.what() << endl;
            exit (2345);
        }
        result += aByte;
    }
    return result;
}
