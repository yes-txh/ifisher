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

using std::string;
using boost::shared_ptr;
using clynn::RWLock;
using clynn::ReadLocker;
using clynn::WriteLocker;

int32_t main(int argc, char* argv[]){
    RWLock lock;
    ReadLocker locker1(lock);
    locker1.Unlock();
    WriteLocker locker2(lock);
    locker2.Unlock();
    locker1.Lock();
    return 0;
}
