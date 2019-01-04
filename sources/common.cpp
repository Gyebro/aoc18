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