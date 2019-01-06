//
// Created by Gyebro on 2019-01-04.
//

#ifndef AOC18_COMMON_H
#define AOC18_COMMON_H

#include <vector>
#include <string>
#include <sstream>

using namespace std;

string sort_string(const string& s);

vector< pair<char, size_t> > string_tally(const string& s);

vector<string> get_lines(string filename);

size_t string_diff_chars(const string& u, const string& v);

string string_union(const string& u, const string& v);

vector<string> split(const string &s, char delim);

//// Templates ////

template <class U, class V>
bool vector_pair_query(vector< pair<U,V> >& pairs, const U& u, const V& v, bool first=true, bool second=false) {
    bool match = false;
    bool m;
    for (pair<U,V>& p : pairs) {
        m = true; // Assume match
        // Test first element ?= u if required
        if (first)  { m &= p.first == u; }
        if (second) { m &= p.second == v; }
        if (m) {
            match = true;
            break;
        }
    }
    return match;
}

template<class T>
void split(const std::string &s, char delim, T result) {
    stringstream ss(s); string item;
    while (getline(ss, item, delim)) {
        *(result++) = item;
    }
}

#endif //AOC18_COMMON_H
