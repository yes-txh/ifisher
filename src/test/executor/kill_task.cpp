#include <iostream>
#include <string>
#include <string.h>

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include "common/clynn/rpc.h"
#include "include/proxy.h"
#include "include/classad_attr.h"

using namespace std;

static string usage = "./kill_task job_id task_id";

int main(int argc, char ** argv) {
    if(argc != 3) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    }

    // job id 
    int32_t job_id = atoi(argv[1]);
    if (job_id <= 0) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    }

    // task id 
    int32_t task_id = atoi(argv[2]);
    if (task_id <= 0) {    
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    }

    cout << "kill task, job_id:" << job_id << ", task_id:"<< task_id << endl;
    string endpoint = "127.0.0.1:9999";


    // build a task
    /*TaskInfo task_info;
    task_info.framework_name = "Hadoop1.0.4";
    task_info.id = task_id;
    task_info.cmd = "bash";
    task_info.arguments = "/home/wm/work/hadoop/hadoop-1.0.4/bin/hadoop --config /home/wm/work/hadoop/hadoop-1.0.4/conf tasktracker";
    task_info.need_cpu = 0.5;
    task_info.need_memory = 500;*/

    try {
        Proxy<ExecutorClient> proxy = Rpc<ExecutorClient, ExecutorClient>::GetProxy(endpoint);
        proxy().KillTask(job_id, task_id);
    } catch (TException &tx) {
        cout<<"error"<<endl;
        return -1;
    }

    return 0;
}
