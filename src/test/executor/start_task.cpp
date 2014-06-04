#include <iostream>
#include <string>
#include <string.h>

#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include "common/clynn/rpc.h"
#include "include/proxy.h"
#include "include/classad_attr.h"

using namespace std;

static string usage = "./start_task job_id task_id vnc_port vm_type";

int main(int argc, char ** argv) {
    if(argc != 5) {
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

    // vnc port
    int32_t vnc_port = atoi(argv[3]);
    if (vnc_port < 0) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    }

    // vm type
    string vm_type = argv[4];
    int32_t vm_t = 0;
    bool is_run = false;
    if ("" == vm_type) {
        cout << "Usage is wrong." << endl;
        cout << "Usage is: " << usage << endl;
        return -1;
    } else if ("KVM" == vm_type) {
        vm_t = 1;
        is_run = false; 
    } else if ("LXC" == vm_type) {
        vm_t = 2;
        is_run = true;
    } else {
        cout << "No the VM Type '" << vm_type << "'." << endl;
    }
    
    cout << "start task, job_id:" << job_id << ", task_id:" << task_id << endl;
    //string endpoint = "192.168.11.51:9997"; // hy
    // string endpoint = "192.168.120.88:9999"; // iie
    string endpoint = "10.138.80.66:9997";
    // build task    
    ClassAd ad;
    // overview
    ad.InsertAttr(ATTR_JOB_ID, job_id);
    ad.InsertAttr(ATTR_TASK_ID, task_id);
    ad.InsertAttr(ATTR_VMTYPE, vm_t);
    ad.InsertAttr(ATTR_IS_RUN, true);
    ad.InsertAttr(ATTR_USER, "common");

    // vm info
    ad.InsertAttr(ATTR_MEMORY, 1024);
    ad.InsertAttr(ATTR_VCPU, 1);
    ad.InsertAttr(ATTR_IP, "192.168.10.163");
    ad.InsertAttr(ATTR_PORT, 9991);
    ad.InsertAttr(ATTR_OS, "ubuntu");
    ad.InsertAttr(ATTR_Image, "ubuntu.qcow2");
    ad.InsertAttr(ATTR_SIZE, 10);
    ad.InsertAttr(ATTR_ISO, "");
    ad.InsertAttr(ATTR_VNC_PORT, vnc_port);
    ad.InsertAttr(ATTR_RPC_RUNNING, false);
    // app info
    ad.InsertAttr(ATTR_APP_ID, 1);
    ad.InsertAttr(ATTR_APP_NAME, "getURL");
    // ad.InsertAttr(ATTR_APP_SRC_PATH, "/user/root/");
    // ad.InsertAttr(ATTR_APP_OUT_DIR, "");
    // ad.InsertAttr(ATTR_INSTALL_DIR, "/usr/bin/");
    ad.InsertAttr(ATTR_EXE_PATH, "getURL");
    // ad.InsertAttr(ATTR_EXE_PATH, "a");
    // ad.InsertAttr(ATTR_ARGUMENT, "-a -b -c");
    // ad.InsertAttr(ATTR_STOP_PATH, "killall a");
    // ad.InsertAttr(ATTR_OUT_DIR, "");

    // classad -> string
    ClassAdUnParser unparser;
    string str_ad;
    // Serialization, convert ClassAd into string str_ad
    unparser.Unparse(str_ad, &ad);

    // build a task
    /*TaskInfo task_info;
    task_info.framework_name = "Hadoop1.0.4";
    task_info.id = task_id;
    task_info.cmd = "bash";
    task_info.arguments = "/home/wm/work/hadoop/hadoop-1.0.4/bin/hadoop --config /home/wm/work/hadoop/hadoop-1.0.4/conf tasktracker";
    task_info.need_cpu = 0.5;
    task_info.need_memory = 500;*/

    cout << str_ad << endl;
 
    try {
        Proxy<ExecutorClient> proxy = RpcClient<ExecutorClient>::GetProxy(endpoint);
        proxy().StartTask(str_ad);
    } catch (TException &tx) {
        cout<<"error"<<endl;
        return -1;
    }

    return 0;
}
