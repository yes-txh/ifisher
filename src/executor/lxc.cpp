/********************************
 FileName: executor/lxc.cpp
 Author:   WangMin
 Date:     2013-09-17
 Version:  0.1
 Description: lxc(Linux Container), inherit from vm
*********************************/

#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
//#include <sys/unistd.h>
//#include <dirent.h>
#include <fcntl.h> // for func open
//#include <time.h>
//#include <signal.h>

#include <boost/algorithm/string.hpp>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "lxc/lxc.h"
#include "lxc/conf.h"

#include "common/clynn/string_utility.h"
#include "common/lxc/confile.h"
#include "executor/hdfs_wrapper.h"
#include "include/proxy.h"
#include "executor/system.h"
#include "executor/lxc.h"
#include "executor/task_entity_pool.h"
#include "executor/resource_manager.h"

using std::cout;
using std::endl;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using boost::trim;
using log4cplus::Logger;
using clynn::WriteLocker;
using clynn::ReadLocker;

DECLARE_string(lxc_dir);
DECLARE_string(lxc_template);
DECLARE_bool(lxc_create);
DECLARE_bool(lxc_relative_path);

static Logger logger = Logger::getInstance("executor");

static const double DEFAULT_CPU_SHARE = 1.0;

string LXC::m_conf_template = "";

/// @brief: public function
// virtual function, from VM
// virtual CreateEnv, include ..
int32_t LXC::Execute() {
    if (CreateVM() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to create lxc, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }

    if (InstallApp() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to install app, lxc name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }

    if (LXC::StartApp() != 0){
        LOG4CPLUS_ERROR(logger, "Failed to start app, lxc name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;	
    }

    return 0;

}

int32_t LXC::CreateVM() {
    if (CreateLXC() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to create lxc, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }
    return 0;
}

// install app from hdfs
int32_t LXC::InstallApp() {
    if (!HDFSMgrI::Instance()->CopyToLocalFile(GetTaskInfo().app_info.app_src_path, m_dir)) {
        LOG4CPLUS_ERROR(logger, "Failed to install app, from hdfs:" << GetTaskInfo().app_info.app_src_path  << ", to local fs:" << m_dir);
        return -1;
    }
    return 0;
}

// execute the task, run the app
int32_t LXC::StartApp() {
    if (ExecuteLXC() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to execute lxc, name:" << GetName() << ", job_id:" << GetID().job_id << ", task_id:" << GetID().task_id);
        return -1;
    }
    return 0;
}

bool LXC::Stop() {
    // 
    string cmd = "lxc-stop -n " + GetName();
    int32_t ret = system(cmd.c_str());
    ret = ret >> 8;
    if (ret != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to stop lxc, name:" << GetName());
        return false;
    }
    return true;
}

bool LXC::Kill() {
    string cmd_stop = "lxc-stop -n " + GetName();
    int32_t ret_stop = system(cmd_stop.c_str());
    ret_stop = ret_stop >> 8;
    if (ret_stop != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to stop lxc, name:" << GetName());
        return false;
    }

    string cmd_destroy = "lxc-destroy -n " + GetName();
    int32_t ret_destroy = system(cmd_destroy.c_str());
    ret_destroy = ret_destroy >> 8;
    if (ret_destroy != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to destroy lxc, name:" << GetName());
        return false;
    }

    string cmd_rmr = "rm -rf " + m_dir;
    int32_t ret_rmr = system(cmd_rmr.c_str());
    ret_rmr = ret_rmr >> 8;
    if (ret_rmr != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to rmr work directory:" << m_dir << ", name:" << GetName());
        return false;
    }
    return true;
}
/*
VMState::type LXC::GetState() {
    // return VMState::VM_SERVICE_ONLINE;
    return m_hb_vm_info.state;
}
*/
HbVMInfo LXC::GetHbVMInfo() {
    // "empty" means NULL
    HbVMInfo empty;
    empty.id = GetID();
    empty.name = GetName();
    empty.type = GetVMType();
    empty.cpu_usage = 0;
    empty.memory_usage = 0;
    empty.bytes_in = 0;
    empty.bytes_out = 0;
    empty.app_state = AppState::APP_RUNNING;
    //empty.app_running = false;

    if (!SetProcesses()) {
        LOG4CPLUS_ERROR(logger, "Failed to set processes for lxc.");
        return empty;
    }

    static bool first = true;
    if (first) {
        m_hb_vm_info.id = GetID();
        m_hb_vm_info.name = GetName();
        m_hb_vm_info.type = GetVMType();
    }

    m_memory_used_in_kb = GetUsedMemory();
    m_hb_vm_info.memory_usage = (double)m_memory_used_in_kb / (GetTaskInfo().vm_info.memory * 1024);
    m_hb_vm_info.cpu_usage = GetCPUUsage();
    m_hb_vm_info.bytes_in = 0;
    m_hb_vm_info.bytes_out = 0;
    if (GetVeth() != "") {
        System::GetNetFlowinBytes(m_veth.c_str(), m_hb_vm_info.bytes_in, m_hb_vm_info.bytes_out);
    }
    m_hb_vm_info.app_state = AppState::APP_RUNNING;
    //m_hb_vm_info.app_running = true;

    return m_hb_vm_info;
}

void LXC::SetHbVMInfo(const string& hb_vm_info_ad) {
}

/// @brief: unique 
pid_t LXC::GetPid() {
    return m_pid;
}

string LXC::GetVeth() {
    return m_veth;
}

/// @brief: private function
// set name
void LXC::SetName() {
    // app_name + "_lxc_" + job_id + "_" + task_id
    TaskID id = GetID();
    stringstream ss_job, ss_task;
    ss_job << id.job_id;
    ss_task << id.task_id;
    string name = GetTaskInfo().app_info.name + "_lxc_" + ss_job.str() + "_" + ss_task.str();
    SetNameByString(name);
}

bool LXC::SetVeth() {
    // no ip, no veth
    if (GetTaskInfo().vm_info.ip == "") {
        m_veth = "";
        return true;
    }
    // set veth name
    TaskID id = GetID();
    stringstream ss_job, ss_task;
    ss_job << id.job_id;
    ss_task << id.task_id;
    m_veth = "veth_" + ss_job.str() + "_" + ss_task.str();
    if (strlen(m_veth.c_str()) > veth_max_length) {
        LOG4CPLUS_ERROR(logger, "veth name is too long.");
        return false;
    }
    return true;
}
// set name m_dir, mk work dir, copy lxc.conf, and change work director to m_dir
int32_t LXC::Init() {
    // set name, work dir
    SetName();
    m_dir = FLAGS_lxc_dir + "/" + GetName() + "/";
    m_conf_bak = m_dir + "lxc_bak.conf";
    m_conf_path = m_dir + "lxc.conf";

    if (!SetVeth()) {
        LOG4CPLUS_ERROR(logger, "Failed to set veth name.");
        return -1;
    }

    if ("" == m_conf_template) {
        m_conf_template = FLAGS_lxc_template;
    }

    // check total work directory
    if (chdir(FLAGS_lxc_dir.c_str()) < 0) {
        LOG4CPLUS_ERROR(logger, "No lxc work directory:" << FLAGS_lxc_dir);
        // task_ptr->TaskFailed();
        return -1;
    }

    // mkdir work dir
    if (access(m_dir.c_str(), F_OK) == -1) {
        if (mkdir(m_dir.c_str(), 0755) != 0) {
           LOG4CPLUS_ERROR(logger, "Failed to create lxc work dir:" << m_dir);
           // task_ptr->TaskFailed();
           return -1;
        }
    }

    // cp default lxc.conf to m_dir
    string cmd1 = "cp " + m_conf_template + " " + m_conf_bak;
    int32_t ret1 = system(cmd1.c_str());
    ret1 = ret1 >> 8;
    if (ret1 != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to copy default lxc.conf for " << m_conf_bak);
        LOG4CPLUS_ERROR(logger, "cmd:" << cmd1);
        // task_ptr->TaskFailed();
        return -1;
    }

    string cmd2 = "cp " + m_conf_bak + " " + m_conf_path;
    int32_t ret2 = system(cmd2.c_str());
    ret2 = ret2 >> 8;
    if (ret1 != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to copy default lxc.conf for " << m_conf_path);
        LOG4CPLUS_ERROR(logger, "cmd:" << cmd2);
        // task_ptr->TaskFailed();
        return -1;
    }
 
    // change work directory
    if (chdir(m_dir.c_str()) < 0) {
        LOG4CPLUS_ERROR(logger, "Failed to change work directory into " << m_dir);
        // task_ptr->TaskFailed();
        return -1;
    }

    // task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 50.0);

    return 0;
}

int32_t LXC::CreateLXC() {
    TaskID id = GetID();
    TaskPtr task_ptr = GetTaskPtr();
  
    if (!task_ptr) {
        LOG4CPLUS_ERROR(logger, "Failed to find task, job_id:" << id.job_id << ", task_id" << id.task_id);
        task_ptr->TaskFailed();
        return -1;
    }
 
    // init
    if (Init() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to initialize lxc");
        task_ptr->TaskFailed();
        return -1;
    }
    task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 40.0); 

    // conf SetIP
    if (GetTaskInfo().vm_info.ip != "") {
        SetIPConf();
    }
    task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 45.0);

    // lxc-create
    if (FLAGS_lxc_create) {
       string cmd_create = "lxc-create -n " + GetName() + " -f " + m_conf_path;
       int32_t ret = system(cmd_create.c_str());
       ret = ret >> 8;
       if (ret != 0) {
           LOG4CPLUS_ERROR(logger, "Failed to create lxc, cmd:" << cmd_create);
           task_ptr->TaskFailed();
           return -1;
       } 
    }
    task_ptr->SetStates(TaskEntityState::TASKENTITY_STARTING, 60.0);

    return 0;    
}

int32_t LXC::ExecuteLXC() {
    // fork child process
    m_pid = fork();
    if (0 == m_pid) {
        // child pid
        // close all the fd inherited from parent
        CloseInheritedFD();
        // TODO log
        RedirectLog();

        // TODO how config ip addr with api
        //***** lxc API *****//

        vector<string> args;
        StringUtility::Split(GetTaskInfo().app_info.argument, ' ', &args);
        // add cmd as args[0], args: cmd, args1, args2, args3 ...
        // test TODO
        string exe_total_path = "";
        if (FLAGS_lxc_relative_path)
            exe_total_path = m_dir + "/" + GetTaskInfo().app_info.exe_path;
        else 
            exe_total_path = GetTaskInfo().app_info.exe_path;
        args.insert(args.begin(), exe_total_path);
        // args.insert(args.begin(), GetTaskInfo().app_info.exe_path);
        // TODO args.insert(args.begin(), m_info.cmd); // bash failed
        // convert string vector to string array
        m_exe_array = StringUtility::CreateArgArray(args); 
        // TODO test cout
        /* stringstream oss;
        for (int i = 0; m_exe_array[i]; ++i)
            oss << m_exe_array[i] << " "; 
        cout << oss << endl; */
  
        char *rcfile = (char *)m_conf_path.c_str();
        lxc_conf* conf = lxc_conf_init();
        if (!conf) {
            LOG4CPLUS_ERROR(logger, "Failed to initialize configuration");
            exit(-1);
        }
        if (rcfile && lxc_config_read(rcfile, conf)) {
            // 0 is true, other err number means error
            LOG4CPLUS_ERROR(logger, "Failed to read configuration file");
        }

        lxc_start(GetName().c_str(), m_exe_array, conf);
        free(conf);

        // lxc-execute
        /*string exe_total = GetTaskInfo().app_info.exe_path + " " 
                         + GetTaskInfo().app_info.argument;
        string cmd2 = "lxc-execute -n " + GetName() + " " + exe_total;
        int32_t ret2 = system(cmd2.c_str());
        ret2 = ret2 >> 8;
        if (ret2 != 0) {
            LOG4CPLUS_ERROR(logger, "Can't execute lxc, cmd:" << cmd2);
            return -1;
        }*/

        exit(-1);
    } else {
        // parent process
        sleep(1);
        TaskPtr task_ptr = GetTaskPtr();
        if (!SetResourceLimit()) {
            LOG4CPLUS_ERROR(logger, "Failed to set resource limit.");
            task_ptr->TaskFailed();
            return -1;
        }
        task_ptr->TaskStarting();
    }
    return 0;
} 

// Set IP Conf with lxc.conf
int32_t LXC::SetIPConf() {
    ifstream in(m_conf_bak.c_str());
    ofstream out(m_conf_path.c_str());
    string tmp;
    while (getline(in, tmp, '\n')) {
        int index_ipv4 = tmp.find("ipv4");
        if (index_ipv4 > 0 && index_ipv4 < 500) {
            out << tmp.substr(tmp.find('#') + 2, tmp.find('='));
            out << GetTaskInfo().vm_info.ip << endl;
            /*LOG(ERROR) << tmp.substr(tmp.find('#') + 2, tmp.find('='));
            LOG(ERROR) << m_info.lxc_ip;*/
            continue;
        } 
        int index_veth = tmp.find("veth.pair");
        if (index_veth > 0 && index_veth < 500) {
            out << tmp.substr(tmp.find('#') + 2, tmp.find('='));
            out << GetVeth() << endl;
            continue;
        }
        out << tmp.substr(tmp.find('#') + 2, strlen(tmp.c_str()) - 2)
            << endl;
            // LOG(ERROR) << tmp.substr(tmp.find('#') + 2, strlen(tmp.c_str()) - 2);
    }
    in.close();
    out.close();
    return 0;
}

// close inherited(ji cheng)
// must in child process
int32_t LXC::CloseInheritedFD() {
    // get proc path
    stringstream ss;
    string pid;
    ss << getpid();
    ss >> pid;
    string path = "/proc/" + pid + "/fd";
    // c style
    // char path[30] = {0};
    // snprintf(path, sizeof(path), "/proc/%d/fd", getpid());

    DIR* dp = opendir(path.c_str());
    if (!dp) {
       // open error 
       LOG4CPLUS_ERROR(logger, "Failed to open proc path:" << path <<
                       ", when close inherited FD");
       return -1;
    }

    dirent* ep = NULL;
    while ((ep = readdir(dp))) {
        int fd = atoi(ep->d_name);
        // get rid of stdin, stdout, stderr which is 0,1,2
        if (fd > 2)
            close(fd);
    }

    closedir(dp);
    return 0;
}

// redirect log
int32_t LXC::RedirectLog() {
    // create log name and path
    char c_timestamp[30] = {0};
    System::GetCurrentTime(c_timestamp, sizeof(c_timestamp));
    string timestamp = c_timestamp;

    string log_name = GetName() + "_" + timestamp;
    string log_path = m_dir + log_name;
    
    // open the log file redirect stdout and stderr
    int fd = open(log_path.c_str(), (O_RDWR | O_CREAT), 0644);
    // LOG(ERROR) << "fd:" << fd;
    // stdout->fd
    dup2(fd, 1);
    // stderr->stdout
    dup2(fd, 2);
    // dup2(1, 2);
    close(fd);

    return 0;
}

// set resource limit
bool LXC::SetResourceLimit() {
    // set memory
    char data[32] = {0};
    snprintf(data, sizeof(data), "%d", GetTaskInfo().vm_info.memory * 1024 * 1024);
    if (0 != lxc_cgroup_set(GetName().c_str(), "memory.limit_in_bytes", data)) {
        LOG4CPLUS_ERROR(logger, "Failed to set cgroup memory.limit_in_bytes.");
        return false;
    }
    static const double DEFAULT_CPU_SHARE = 1.0;
    // set cpu
    memset(data, 0, sizeof(data));
    snprintf(data, sizeof(data), "%d",
             static_cast<int>(GetTaskInfo().vm_info.vcpu * 1024 / DEFAULT_CPU_SHARE));
    // DEFAULT_CPU_SHARE = 1.0
    if (0 != lxc_cgroup_set(GetName().c_str(), "cpu.shares", data)) {
        LOG4CPLUS_ERROR(logger, "Failed to set cgroup cpu.shares.");
        return false;
    }
    return true;
}

bool LXC::SetProcesses() {
    m_p_list.clear();
    // cmd
    string cmd = "lxc-ps -n " + GetName() + " -- -ef";
    FILE *stream = popen(cmd.c_str(),"r");
    if (NULL == stream) {
        LOG4CPLUS_ERROR(logger, "error in cmd: " << cmd);
        return false;
    }

    int32_t process_num = 0;
    char buf[1024] = {0};
    string s_buf = "";
    // read each line, count process number as line number
    // one line ~ one process
    // first line is 'schema'
    fgets(buf, sizeof(buf) - 1, stream);
    while (fgets(buf, sizeof(buf) - 1, stream)) {
        process_num++;

        // save old buf
        string old_buf = buf;

        // split
        const char* separator = " ";
        char *p;
        int32_t count = 0;
        vector<string> process_buf;
        p = strtok(buf, separator);
        while(p && count < 8) {
            process_buf.push_back(p);
            p = strtok(NULL,separator);
            count++;
        }
        // last attr cmd, bao liu " "
        process_buf.push_back( old_buf.substr(p-buf, old_buf.size() - (p-buf) -1) );

        if (9 != process_buf.size()) {
            LOG4CPLUS_ERROR(logger, "process_buf size != 9");
            return false;
        }

        // read data
        ProcessInfo info;
        if (( info.pid = atoi(process_buf[2].c_str()) ) == 0) {
            LOG4CPLUS_ERROR(logger, "error in set pid, string to int, buf: " << process_buf[2]);
            return false;
        }

        if (( info.ppid = atoi(process_buf[3].c_str()) ) == 0) {
            LOG4CPLUS_ERROR(logger, "error in set ppid, string to int, buf: " << process_buf[3]);
            return false;
        }

        info.cmd = process_buf[8];
        // info.stime = process_buf[5];
        // info.time = process_buf[7];

        // into process_list
        m_p_list.push_back(info);
    }

    // close file
    pclose(stream);
    m_process_number = process_num ? process_num : 1;
    return true;
}

double LXC::GetCPUUsage() {
    vector<ProcessInfo>::iterator it;
    char cmd1[256] = {0};
    FILE *stream1;
    double percent = 0.0;
    for (it = m_p_list.begin(); it != m_p_list.end(); ++it) {
        /* cpu */
        // set cmd
        memset(cmd1, 0, sizeof(char)*256);
        sprintf(cmd1, "ps -p %d -o pcpu | grep -v CPU", it->pid);
        stream1 = popen(cmd1, "r");
        if (NULL == stream1) {
            LOG4CPLUS_ERROR(logger, "error in cmd: " << cmd1);
            return false;
        }

        // get output
        char buf1[256] = {0};
        fgets(buf1, sizeof(buf1) - 1, stream1);
        // check ASCII 0
        if (0 == buf1[0]) {
            pclose(stream1);
            LOG4CPLUS_ERROR(logger, "Failed to parse cpu percentage, lxc:" << GetName());
            return false;
        }
        string s_buf1 = buf1;
        trim(s_buf1);

        // string to float
        double p = 0.0;
        stringstream ss1;
        ss1 << s_buf1;
        ss1 >> p;
        percent += p;
    }
    return percent/100;
}

// in kb
int32_t LXC::GetUsedMemory() {
    vector<ProcessInfo>::iterator it;
    char cmd2[256] = {0};
    FILE *stream2;
    int32_t memory_used = 0;
    for (it = m_p_list.begin(); it != m_p_list.end(); ++it) {
        /* mem */
        // set cmd
        memset(cmd2, 0, sizeof(char)*256);
        sprintf(cmd2, "ps -p %d -o rss | grep -v RSS", it->pid);
        stream2 = popen(cmd2, "r");
        if (NULL == stream2) {
            LOG4CPLUS_ERROR(logger, "error in cmd: " << cmd2);
            return false;
        }

        // get output
        char buf2[256] = {0};
        fgets(buf2, sizeof(buf2) - 1, stream2);
        // check ASCII 0
        if (0 == buf2[0]) {
            pclose(stream2);
            LOG4CPLUS_ERROR(logger, "Failed to parse memory usage, lxc:" << GetName());
            return false;
        }
        string s_buf2 = buf2;
        trim(s_buf2);

        // string to int64_t
        int64_t mem = 0;
        stringstream ss2;
        ss2 << s_buf2;
        ss2 >> mem;
        memory_used += mem;
    }
    return memory_used;
}
