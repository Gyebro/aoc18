#include <fstream>
#include "config.h"
#include "days.h"

int day01(string inputfile, bool partone) {
    // Parse input
    ifstream in(inputfile);
    vector<int> freqlist;
    string line;
    while(in.good()) {
        getline(in, line);
        freqlist.push_back(stoi(line));
    }
    int result = 0;
    if (partone) {
        // Part one
        for (int i : freqlist) { result += i; }
    } else {
        // Part two
        // Create a bool array to represent the number line
        int offset = 50000;
        vector<bool> n(500000,false);
        // Start applying the sequence, freq is the pointer to the number line
        int freq = 0;
        int i = 0;
        while (n[freq+offset] == false) {
            n[freq+offset] = true;
            freq += freqlist[i];
            i++;
            if (i == freqlist.size()) { i=0; }
        }
        result = freq;
    }
    return result;
}