#include <stdio.h>
#include <zookeeper/zookeeper.h>
#include <errno.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>
#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include "common/clynn/rwlock.h"
#include "include/proxy.h"
#include "common/clynn/rpc.h"

using std::string;
using std::map;
using boost::shared_ptr;
using boost::thread;
using clynn::RWLock;
using clynn::ReadLocker;
using clynn::WriteLocker;

class A {
public:
    A(int id){
        m_id = id;
    }
    ~A(){
        printf("%d is over\n", m_id);
    }
    int m_id;
};

typedef shared_ptr<A> APtr;

map<int ,APtr> map1;

void Test(){
   APtr ptr(new A(1));
   APtr ptr2(new A(2));
   map1[1] = ptr;
   map1[1] = ptr2;
   printf("Test is over\n"); 
}

int32_t main(int argc, char* argv[]){
    Test();
    printf("main is over\n");
    return 0;
}
