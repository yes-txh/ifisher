/********************************
 FileName: executor/vm_pool.cpp
 Author:   WangMin
 Date:     2013-09-04
 Version:  0.1
 Description: the pool of vm, include kvm and lxc
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include <classad/classad.h>
#include <classad/classad_distribution.h>
#include "include/classad_attr.h"

#include "executor/vm_pool.h"

using log4cplus::Logger;
using clynn::ReadLocker;
using clynn::WriteLocker;
 
static Logger logger = Logger::getInstance("executor");

DECLARE_string(libvirt_dir);
DECLARE_string(lxc_dir);
// DECLARE_string(lxc_template);

bool VMPool::Init() {
    // check libvirt work dir
    if (access(FLAGS_libvirt_dir.c_str(), F_OK) == -1) {
        string cmd = "mkdir -p " + FLAGS_libvirt_dir;
        int32_t ret = system(cmd.c_str());
        ret = ret >> 8;
        if (ret != 0) {
           LOG4CPLUS_ERROR(logger, "Failed to create libvirt(kvm) work dir: " << FLAGS_libvirt_dir);
           return false;
        }
    }

    // check lxc work dir
    if (access(FLAGS_lxc_dir.c_str(), F_OK) == -1) {
        string cmd = "mkdir -p " + FLAGS_lxc_dir;
        int32_t ret = system(cmd.c_str());
        ret = ret >> 8;
        if (ret != 0) {
            LOG4CPLUS_ERROR(logger, "Failed to create lxc work dir, cmd: " << cmd);
            return false;
        }
    }

    return true;
}

// TODO just for test
void VMPool::PrintAll() {
    ReadLocker locker(m_lock);
    printf("vm_pool.cpp\n");
    printf("************ VMs ************\n");
    for (map<TaskID, VMPtr>::iterator it = m_vm_map.begin();
         it != m_vm_map.end(); ++it) {
        printf("VM job_id:%d, task_id:%d\n", (it->first).job_id, (it->first).task_id);
    }
    printf("*****************************\n");
}

// insert VMPtr into map and queue
void VMPool::Insert(const VMPtr& ptr) {
    InsertIntoPool(ptr);
    InsertIntoQueue(ptr);
}

// insert VMPtr into map
void VMPool::InsertIntoPool(const VMPtr& ptr) {
    WriteLocker locker(m_lock);
    m_vm_map[ptr->GetID()] = ptr;
    m_vm_state_map[ptr->GetID()] = true;
}

// insert VMPtr into queue
void VMPool::InsertIntoQueue(const VMPtr& ptr) {
    WriteLocker locker(m_lock);
    m_queue.push(ptr);
}

// delete VMPtr from map
void VMPool::DeleteFromPool(const TaskID id) {
    WriteLocker locker(m_lock);
    // erase() will invoke destructor(xi gou) func
    m_vm_map.erase(id);
}

bool VMPool::FindByTaskID(const TaskID id) {
    ReadLocker locker(m_lock);
    // ptr->GetID() is TaskID(job_id, task_id)
    map<TaskID, VMPtr>::iterator it = m_vm_map.find(id);
    return it != m_vm_map.end();
}

// @brief: find a waiting VM, and start it
int32_t VMPool::StartVM() {
    WriteLocker locker(m_lock);
    // queue is empty
    if (m_queue.empty()) {
        return -1;
    }
    
    // get first vm_ptr
    VMPtr ptr = m_queue.front();
    m_queue.pop();
    
    // vm is exist in pool(map)?
    TaskID id = ptr->GetID();
    map<TaskID, VMPtr>::iterator it = m_vm_map.find(id);
    if (m_vm_map.end() == it) {
        LOG4CPLUS_ERROR(logger, "Failed to find the VM in the Pool, job_id:" << id.job_id << ", task_id:" << id.task_id);
        LOG4CPLUS_ERROR(logger, "Can't start the VM, job_id:" << id.job_id << ", task_id:" << id.task_id); 
        return -1;
    }

    // LOG4CPLUS_DEBUG(logger, "Begin to start the VM, job_id:" << id.job_id << ", task_id:" << id.task_id);
   
    // execute, maybe Synchronous and Asynchronous
    if (0 == ptr->Execute()) {
        // success start
        // LOG4CPLUS_INFO(logger, "Create environment successfully for VM, name:" << ptr->GetName() << ", job_id:" << id.job_id << ", task_id:" << id.task_id);
    } else {
        LOG4CPLUS_ERROR(logger, "Failed to Execute task for VM, name:" << ptr->GetName() << ", job_id:" << id.job_id << ", task_id:" << id.task_id);
        return -1;
    }

    return 0;
}

// kill vm by task id
bool VMPool::KillVMByTaskID(const TaskID id) {
    WriteLocker locker(m_lock);

    // find the vm
    map<TaskID, VMPtr>::iterator it = m_vm_map.find(id);
    if (m_vm_map.end() == it) {
        LOG4CPLUS_ERROR(logger, "Failed find the VM, job_id:" << id.job_id << ", task_id:" << id.task_id);
        LOG4CPLUS_INFO(logger, "Kill task directly, job_id:" << id.job_id << ", task_id:" << id.task_id);
        return true;
    }

    if (!(it->second)->Kill()) {
        LOG4CPLUS_ERROR(logger, "Failed to kill VM, job_id:" << id.job_id << ", task_id:" << id.task_id);
        return false;
    }

    // Delete task from Pool(map)
    m_vm_map.erase(id);
    // LOG4CPLUS_INFO(logger, "Kill VM successfully, job_id:" << id.job_id << ", task_id:" << id.task_id);
    
    return true;
}

// stop vm by task id
bool VMPool::StopVMByTaskID(const TaskID id) {
    WriteLocker locker(m_lock);

    // find the vm
    map<TaskID, VMPtr>::iterator it = m_vm_map.find(id);
    if (m_vm_map.end() == it) {
        LOG4CPLUS_ERROR(logger, "Failed find the VM, can't stop it, job_id:" << id.job_id << ", task_id:" << id.task_id);
        // LOG4CPLUS_ERROR(logger, "Stop task directly, job_id:" << id.job_id << ", task_id:" << id.task_id);
        return false;
    }

    if (!(it->second)->Stop()) {
        LOG4CPLUS_ERROR(logger, "Failed to stop VM, job_id:" << id.job_id << ", task_id:" << id.task_id);
        return false;
    }

    // LOG4CPLUS_INFO(logger, "Stop VM successfully, job_id:" << id.job_id << ", task_id:" << id.task_id);
    return true;
}

// get VMPtr from 
VMPtr VMPool::GetVMPtr(const TaskID id) {
    ReadLocker locker(m_lock);
    map<TaskID, VMPtr>::iterator it = m_vm_map.find(id);
    if (it != m_vm_map.end()) {
        return it->second;
    }
    // not find then return NULL
    return VMPtr();
}

double VMPool::GetAllocatedCPU() {
    ReadLocker locker(m_lock);
    double allocated_cpu = 0;
    for (map<TaskID, VMPtr>::iterator it = m_vm_map.begin();
        it != m_vm_map.end(); ++it) {
        allocated_cpu += (it->second)->GetTaskInfo().vm_info.vcpu;
    }
    return allocated_cpu;
}

int32_t VMPool::GetAllocatedMemory() {
    ReadLocker locker(m_lock);
    int32_t allocated_mem = 0;
    for (map<TaskID, VMPtr>::iterator it = m_vm_map.begin();
        it != m_vm_map.end(); ++it) {
        allocated_mem += (it->second)->GetTaskInfo().vm_info.memory;
    }
    return allocated_mem;
}

vector<HbVMInfo> VMPool::GetAllHbVMInfo() {
    ReadLocker locker(m_lock);
    vector<HbVMInfo> vm_list;
    for (map<TaskID, VMPtr>::iterator it = m_vm_map.begin();
        it != m_vm_map.end(); ++it) {
        // if vm is down, then not send the heartbeat
        //if((it->second)->GetState() != VMState::VM_OFFLINE){
            vm_list.push_back((it->second)->GetHbVMInfo());
        //}
    }
    return vm_list;
}

bool VMPool::ProcessHbVMInfo(const string& hb_vm_info_ad) {
    WriteLocker locker(m_lock);
    // find the VMPtr
    TaskID id;
     // classad init, string vmhb_info --> ClassAd *ad_ptr
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(hb_vm_info_ad);

    if (!ad_ptr) {
        LOG4CPLUS_ERROR(logger, "Failed to parse classad, ClassAd pointer is NULL.");
        return false;
    }
    //job id and task id  
    if (!ad_ptr->EvaluateAttrNumber(ATTR_JOB_ID, id.job_id)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_JOB_ID << " error.");
        return false;
    }
    if (!ad_ptr->EvaluateAttrNumber(ATTR_TASK_ID, id.task_id)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_TASK_ID << " error.");
        return false;
    }

    map<TaskID, bool>::iterator iter = m_vm_state_map.find(id);
    // can't find the VM state
    if (iter == m_vm_state_map.end()) {
        LOG4CPLUS_ERROR(logger, "Failed to find the VM state , job_id:" << id.job_id << ", task_id:" << id.task_id);
        return false;
    }
    // get VM_State
    bool vm_state = iter->second;
    if (!vm_state) {
        LOG4CPLUS_ERROR(logger, "VM state is false, job_id:" << id.job_id << ", task_id:" << id.task_id);
        return false;
    }


    map<TaskID, VMPtr>::iterator it = m_vm_map.find(id);
    // can't find the VMPtr
    if (it == m_vm_map.end()) {
        LOG4CPLUS_ERROR(logger, "Failed to find the VM, job_id:" << id.job_id << ", task_id:" << id.task_id);
        return false;
    }
    // get VMPtr 
    VMPtr vm_ptr = it->second;
    if (!vm_ptr) {
        LOG4CPLUS_ERROR(logger, "VMPtr is NULL, job_id:" << id.job_id << ", task_id:" << id.task_id); 
        return false;
    }

    // hb_vm_info include cpu, mem and flow
    // TODO: flow can be got with libvirt
    
    vm_ptr->SetHbVMInfo(hb_vm_info_ad);
    return true; 
}


void VMPool::SetVMStateByTaskID(const TaskID id, bool task_state) {
   WriteLocker locker(m_new_lock);
   map<TaskID, bool>::iterator it = m_vm_state_map.find(id);
   if(it != m_vm_state_map.end()) {
       it->second = task_state;
   }
   LOG4CPLUS_INFO(logger, "Set vm state of false, job_id:" << id.job_id << ", task_id:" << id.task_id);
}

bool VMPool::GetVMStateByTaskID(const TaskID id) {
   ReadLocker locker(m_new_lock);
   map<TaskID, bool>::iterator it = m_vm_state_map.find(id);
   if(it != m_vm_state_map.end()) {
       bool vm_state = it->second;
       return vm_state;
   }
   return false;
}
