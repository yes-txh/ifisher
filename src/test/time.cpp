#include <iostream>

#include "common/clynn/timer.h"

using namespace std;
using clynn::Timer;

int32_t main(int argc, char* argv[]){
    Timer timer;
    usleep(50000);
    printf("%f\n", timer.ElapsedSeconds());
    return 0;
}
