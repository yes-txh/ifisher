#include <stdio.h>
#include <zookeeper/zookeeper.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include <boost/shared_ptr.hpp>
#include "common/clynn/rwlock.h"
#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include <boost/thread/thread.hpp>
#include <iostream>
#include <time.h>
#include <cstdatomic>


using namespace std;
long total = 0;
//atomic_long total(0);

void click() {
    for(int i=0; i<1000000;++i)
    {
        total += 1;
    }
}

int main(int argc, char* argv[]) {
    clock_t start = clock();
    boost::thread_group threads;
    for(int i=0; i<100;++i) 
    {
        threads.create_thread(click);
    }

    threads.join_all();
    clock_t finish = clock();
    cout << "result:" << total << endl;
    cout << "duration:" << finish -start << "ms" << endl;
    return 0;
}
