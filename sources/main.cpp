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
    day15("day15.txt");
    //day15("day15.txt", false);
#endif

    return 0;
}