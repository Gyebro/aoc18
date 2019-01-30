//
// Created by Gyebro on 2019-01-04.
//

#include <algorithm>
#include <fstream>
#include "common.h"

string sort_string(const string& s) {
    string sorted = s;
    sort(sorted.begin(), sorted.end());
    return sorted;
}

vector< pair<char, size_t> > string_tally(const string& s) {
    string sorted = sort_string(s);
    size_t i=0;
    vector< pair<char, size_t> > t;
    t.push_back( {sorted[0], 0} );
    for (char c : sorted) {
        if (c == t.back().first) {
            t.back() = { t.back().first, t.back().second+1 };
        } else {
            t.push_back( {c, 1} );
        }
    }
    sort(t.begin(), t.end(), [](auto &l, auto &r) { return l.second < r.second; });
    return t;
}

vector<string> get_lines(string filename) {
    ifstream in(filename);
    vector<string> lines;
    string line;
    while(in.good()) {
        getline(in, line);
        lines.push_back(line);
    }
    in.close();
    return lines;
}

size_t string_diff_chars(const string& u, const string& v) {
    size_t su = u.size();
    size_t sv = v.size();
    if (su != sv) {
        return max(su, sv);
    } else {
        size_t diffs = 0;
        for (size_t i=0; i<su; i++) {
            if (u[i] != v[i]) diffs++;
        }
        return diffs;
    }
}

string string_union(const string& u, const string& v) {
    size_t su = u.size();
    size_t sv = v.size();
    if (su != sv) {
        return "";
    } else {
        string uni = "";
        for (size_t i=0; i<su; i++) {
            if (u[i] == v[i]) uni+=u[i];
        }
        return uni;
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}

string trim_spaces(string value) {
    // Remove leading spaces
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), std::bind1st(std::not_equal_to<char>(), ' ')));
    // Remove trailing ones
    value.erase(std::find_if(value.rbegin(), value.rend(), std::bind1st(std::not_equal_to<char>(), ' ')).base(), value.end());
    return value;
}

Clock::Clock() {
    start();
}

void Clock::start() {
    t_start = chrono::high_resolution_clock::now();
}

void Clock::stop() {
    t_stop = chrono::high_resolution_clock::now();
}

double Clock::read_sec() {
    return read_millisec()/1000.0;
}

double Clock::read_msec() {
    return read_microsec()/1000.0;
}

long long int Clock::read_millisec() {
    auto dt = t_stop - t_start;
    return chrono::duration_cast<std::chrono::milliseconds>(dt).count();
}

long long int Clock::read_microsec() {
    auto dt = t_stop - t_start;
    return chrono::duration_cast<std::chrono::microseconds>(dt).count();
}

long long int Clock::read_nanosec() {
    auto dt = t_stop - t_start;
    return chrono::duration_cast<std::chrono::nanoseconds>(dt).count();
}