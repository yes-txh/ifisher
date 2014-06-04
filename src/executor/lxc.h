/********************************
 FileName: executor/lxc.h
 Author:   WangMin
 Date:     2013-09-17
 Version:  0.1
 Description: lxc(Linux Container), inherit from vm
*********************************/

#ifndef SRC_EXECUTOR_LXC_H
#define SRC_EXECUTOR_LXC_H

#include "sys/types.h"
#include "executor/vm.h"

class LXC : public VM {
public:
    explicit LXC(const TaskInfo& info) : VM(info) {
        m_first = true;
        m_prev_cpu = 0.0;
        m_prev_total = 0.0;
    }

    ~LXC() {
        // clear the work directory
        if (!m_dir.empty()) {
            string cmd = "rm -r " + m_dir;
            system(cmd.c_str());
        }    
    }

    // virtual function, from VM
    int32_t Execute();

    int32_t CreateVM(); // create LXC

    // TODO
    int32_t InstallApp();

    // @brief: fork process and run the task
    int32_t StartApp();

    bool Stop();

    bool Kill();

    //VMState::type GetState();

    HbVMInfo GetHbVMInfo();    //get heartbeart

    void SetHbVMInfo(const string& hb_vm_info_ad);

    pid_t GetPid();

    string GetVeth();
private:
    // virtual function, from VM
    void SetName();

    bool SetVeth();

    // set Member Variable 
    int32_t Init();

    int32_t CreateLXC();

    int32_t ExecuteLXC();

    // @brief: set ip addr with conf
    int32_t SetIPConf();

    // @brief: close all the fd inherited from parent according to /proc/pid/fd
    int32_t CloseInheritedFD();

    // @brief: re-direct log to work directory
    int32_t RedirectLog();
    
    // @brief: set container resource limit
    bool SetResourceLimit(); 

    bool SetProcesses();

    double GetCPUUsage();

    int32_t GetUsedMemory();

private:
    pid_t m_pid;    // 进程号
    string m_dir;   // work dir
    string m_conf_bak;  // lxc.conf path
    string m_conf_path; // lxc.conf path
    string m_veth;
    HbVMInfo m_hb_vm_info;
    static string m_conf_template;

    struct ProcessInfo {
        int32_t pid;
        int32_t ppid;
        string cmd;
    };
    // lxc-execute
    char **m_exe_array;
    // report resource
    int32_t m_process_number;
    int32_t m_memory_used_in_kb;
    vector<ProcessInfo> m_p_list;
    // is first?
    bool m_first;
    double m_prev_cpu;
    double m_prev_total;

    static const uint32_t veth_max_length = 15; //VETH_MAX_LENGTH = 15;
    //static const double DEFAULT_CPU_SHARE = 1.0;
    static const double DEFAULT_CPU_SHARE ;

};

#endif
