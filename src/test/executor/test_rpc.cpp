#include <iostream>
#include <string>
#include <string.h>

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include "common/clynn/rpc.h"
#include "include/proxy.h"
#include "include/classad_attr.h"

using namespace std;

int main(int argc, char ** argv) {
    string endpoint = "10.138.80.66:9997";
    try {
        Proxy<TestClient> proxy = RpcClient<TestClient>::GetProxy(endpoint);
        proxy().Helloworld();
    } catch (TException &tx) {
        cout<<"error"<<endl;
        return -1;
    }

    return 0;
}
