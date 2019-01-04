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


    return 0;
}