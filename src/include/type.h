/********************************
 FileName: executor/type.h
 Author:   WangMin
 Date:     2013-08-27
 Version:  0.1
 Description: type, struct
*********************************/

#ifndef SRC_EXECUTOR_TYPE_H
#define SRC_EXECUTOR_TYPE_H

#include <sys/types.h>
// basic data struct in Linux, such as size_t, time_t, pid_t
#include <sys/ipc.h>
// inter process communication
#include <string>
#include <vector>

#include "include/proxy.h"

using std::string;
using std::vector;

// TaskID: job id + task id
struct TaskID {
    int32_t job_id;
    int32_t task_id;
    
    // overwrite comparison operators
    bool operator <(const TaskID& other) const  {
       if (job_id < other.job_id)
           return true;
       else if (job_id == other.job_id)
           return task_id < other.task_id;
       return false;
   }
};

// app
struct AppInfo {
    int32_t id;
    string name;
    // string os;
    // outside vm, hdfs
    string app_file;  
    string app_src_path;
    string app_out_dir;   // result out dir

    // inside vm
    string install_dir;  // install directory in VM
    string exe;          //exe and argument
    string exe_path;     // execute path
    string argument; 
    string stop_path;    // stop
    string out_dir;
};

// resource vm
struct VMInfo {
    
    // resource
    int32_t memory;
    int32_t vcpu;
    string ip;
    int32_t port;
    string os;

    // only for kvm 
    string img_template;
    string iso_location;
    int32_t size;
    int32_t vnc_port;
    bool rpc_running;
};

struct TaskInfo {
    TaskID id;
    // vm resource
    VMType::type type;
    VMInfo vm_info;
    // app
    bool is_run;
    string user;
    int32_t timeout;
    AppInfo app_info;
};

struct HbVMInfo {
    TaskID id;  // job_id + task_id, vm id
    string name;
    VMType::type type;
    double cpu_usage;
    double memory_usage;
    int32_t bytes_in;
    int32_t bytes_out;
    AppState::type app_state;
};

struct HbMachineInfo {
    string ip;
    int32_t port;
    double cpu_usage;
    double memory_usage;
    int32_t bytes_in;
    int32_t bytes_out;
    vector<HbVMInfo> hb_vminfo_list;
};

// for resource manager
struct AllocResource {
    int32_t cpu;
    int32_t memory;
};

#endif
