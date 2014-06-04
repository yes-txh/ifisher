/********************************
 FileName: test/scheduler/submit-job.cpp
 Author:   WangMin
 Date:     2014-3-19
 Version:  0.1
 Description: submit a job to scheduler (with xml file)
*********************************/

#include <iostream>
#include <string>
#include <string.h>

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include "common/clynn/rpc.h"
#include "include/proxy.h"
#include "include/classad_attr.h"

using namespace std;

static string usage = "./submit-job group_name prio job_type task_num";

int main(int argc, char ** argv) {
    if(argc != 5) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    }
    
    // group name
    string name = argv[1];
    string group_name = "Group " + name;

    // priority
    int32_t prio = atoi(argv[2]);
    
    // job_type
    string job_type = argv[3];

    // task_num 
    int32_t task_num = atoi(argv[4]);
    if (task_num <= 0) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        cout << "Task_num must be a positive interger." << endl;
        return -1;
    }

    cout << "submit job, job_type:" << job_type << ", task_num:" << task_num << endl;
    string endpoint = "192.168.10.80:9999"; // hy
    // string endpoint = "192.168.120.88:9999"; // iie

    // build job ClassAd 
    ClassAd ad;
    // overview
    ad.InsertAttr(ATTR_JOB_TYPE, job_type);
    ad.InsertAttr(ATTR_GROUP, group_name);
    ad.InsertAttr(ATTR_PRIO, prio);
    ad.InsertAttr(ATTR_TOTAL_TASKS, task_num);
    ad.InsertAttr(ATTR_NEED_CPU, 1);
    ad.InsertAttr(ATTR_NEED_MEMORY, 10);
    ad.InsertAttr(ATTR_EXE_PATH, "my_loop");

    // classad -> string
    ClassAdUnParser unparser;
    string str_ad;
    // Serialization, convert ClassAd into string str_ad
    unparser.Unparse(str_ad, &ad);
 
    struct SubmitJobResult result;

    cout << str_ad << endl;
 
    try {
        Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(endpoint);
        proxy().SubmitJob(result, str_ad);
    } catch (TException &tx) {
        cout<<"error"<<endl;
        return -1;
    }

    return 0;
}
