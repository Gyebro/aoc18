#include <iostream>

#include "config.h"
#include "days.h"

using namespace std;

int main() {

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

    // Days 4 to 12

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

    return 0;
}