#include <iostream>

#include "config.h"
#include "days.h"
#include "common.h"

using namespace std;

int main() {

#ifndef RUN_BENCHMARKS

#ifdef DAY01
    cout << day01("day01.txt") << endl;
    cout << day01("day01.txt", false) << endl;
#endif

#ifdef DAY02
    cout << day02("day02.txt") << endl;
    day02("day02.txt", false);
#endif

#ifdef DAY03
    cout << day03("day03.txt") << endl;
    cout << day03("day03.txt", false) << endl;
#endif

#ifdef DAY04
    day04("day04.txt");
    day04("day04.txt", false);
#endif

#ifdef DAY05
    day05("day05.txt");
    day05("day05.txt", false);
#endif

#ifdef DAY06
    day06("day06.txt");
    day06("day06.txt", false);
#endif

#ifdef DAY07
    day07("day07.txt");
    day07("day07.txt", false);
#endif

#ifdef DAY08
    //day08("day08_test.txt"); // Passed: 138
    day08("day08.txt");
    //day08("day08_test.txt", false); // Passed: 66
    day08("day08.txt", false);
#endif

#ifdef DAY09
    // day09("day09_tests.txt"); All five tests: Passed
    day09("day09.txt");
    day09("day09.txt", false);
#endif

#ifdef DAY10
    day10("day10.txt");
#endif

#ifdef DAY11
    day11("day11.txt");
    day11("day11.txt", false);
#endif

#ifdef DAY12
    //day12("day12_test.txt"); // Passed: 325
    day12("day12.txt");
    day12("day12.txt", false);
#endif

#ifdef DAY13
    day13("day13.txt");
    day13("day13.txt", false);
#endif

#ifdef DAY14
    day14("day14.txt");
    day14("day14.txt", false);
#endif

#ifdef DAY15
    //day15("day15_test1.txt"); // Passed 36334
    //day15("day15_test2.txt"); // Passed 39514
    //day15("day15_test3.txt"); // Passed 27755
    //day15("day15_test4.txt"); // Passed 28944
    //day15("day15_test5.txt"); // Passed 18740
    day15("day15.txt");
    //day15("day15_test5.txt", false); // Passed 34, 1140
    day15("day15.txt", false);
#endif

#ifdef DAY16
    day16("day16.txt");
    day16("day16.txt", false);
#endif

#ifdef DAY17
    //day17("day17_test.txt"); // Passed 57
    day17("day17.txt");
    //day17("day17.txt", false);
    day17("day17.txt", false);
#endif

#ifdef DAY18
    //day18("day18_test.txt"); // Passed 1147
    day18("day18.txt");
    day18("day18.txt", false);
#endif

#ifdef DAY19
    day19("day19.txt");
    day19("day19.txt", false);
#endif

#ifdef DAY20
    //day20("day20_test1.txt"); // Map: OK, BFS: OK
    //day20("day20_test2.txt"); // Map: OK, BFS: OK
    //day20("day20_test3.txt"); // Map: OK, BFS: OK
    //day20("day20_test4.txt"); // Map: OK, BFS: OK
    day20("day20.txt");
    day20("day20.txt", false);
#endif

#ifdef DAY21
    day21("day21.txt");
    day21("day21.txt", false);
#endif

#ifdef DAY22
    day22("day22.txt");
    day22("day22.txt", false);
#endif

#ifdef DAY23
    day23("day23.txt");
    day23("day23.txt", false);
#endif

#ifdef DAY24
    //day24("day24_test.txt"); // Passed 5216
    day24("day24.txt");
    day24("day24.txt", false);
#endif

#ifdef DAY25
    day25("day25.txt");
#endif

#endif

#ifdef RUN_BENCHMARKS

    vector<string> tasks, tasks_terminal;
    vector<double> time_ms;

    vector<string> day_titles = {
            "Chronal Calibration",
            "Inventory Management System",
            "No Matter How You Slice It",
            "Repose Record",
            "Alchemical Reduction",
            "Chronal Coordinates",
            "The Sum of Its Parts",
            "Memory Maneuver",
            "Marble Mania",
            "The Stars Align",
            "Chronal Charge",
            "Subterranean Sustainability",
            "Mine Cart Madness",
            "Chocolate Charts",
            "Beverage Bandits",
            "Chronal Classification",
            "Reservoir Research",
            "Settlers of The North Pole",
            "Go With The Flow",
            "A Regular Map",
            "Chronal Conversion",
            "Mode Maze",
            "Experimental Emergency Teleportation",
            "Immune System Simulator 20XX",
            "Four-Dimensional Adventure"
    };

    void (*day_functions[])(string, bool, bool) = {
            &day01, &day02, &day03, &day04, &day05,
            &day06, &day07, &day08, &day09, &day10,
            &day11, &day12, &day13, &day14, &day15,
            &day16, &day17, &day18, &day19, &day20,
            &day21, &day22, &day23, &day24, &day25,
    };

    Clock c;
    string day, day_w_title, inputfile, num;
    for (size_t d = 1; d <= 25; d++) {
        if (d < 10) { num = "0"+to_string(d); }
        else {        num = to_string(d); }
        inputfile = "day"+num+".txt";
        cout << endl << "Day "+num << endl;
        tasks_terminal.emplace_back("Day "+num);
        tasks.emplace_back("Day "+num+": "+day_titles[d-1]);
        // Run part one
        c.start();
        day_functions[d-1](inputfile, true,  false);
        c.stop();
        time_ms.push_back(c.read_msec());
        // Run part two
        c.start();
        day_functions[d-1](inputfile, false, false);
        c.stop();
        time_ms.push_back(c.read_msec());
    }

    // Print result to cout
    cout << endl << endl;
    cout << "Task  \t\tP1 [ms]\t\tP2 [ms]\n";
    cout << "------\t\t-------\t\t-------\n";
    cout << setprecision(3) << fixed;
    for (size_t i=0; i<tasks.size(); i++) {
        cout << tasks_terminal[i] << "\t\t" << time_ms[2*i] << "\t\t" << time_ms[2*i+1] << '\n';
    }
    flush(cout);

    // Also print to readme.md
    ofstream out("../README.md");
    out << "# aoc18\n";
    out << "Advent of Code 2018 in C++\n";
    out << "## Computation times (no optimization, both parts run separately).\n";
    out << "Processor: Intel Core i7-7700HQ, single thread unless indicated\n";
    out << endl;
    out << "Day | Part One [ms] | Part Two [ms]\n";
    out << "--- | ---: | ---:\n";
    out << setprecision(3) << fixed;
    for (size_t i=0; i<tasks.size(); i++) {
        out << tasks[i] << " | " << time_ms[2*i] << " | " << time_ms[2*i+1] << '\n';
    }
    out.close();
#endif

    return 0;
}