/********************************
 FileName: executor/vm.h
 Author:   WangMin
 Date:     2013-08-27
 Version:  0.1
 Description: virtual machine, include kvm and lxc
*********************************/

#ifndef SRC_EXECUTOR_VM_H
#define SRC_EXECUTOR_VM_H

#include <string>
#include <boost/shared_ptr.hpp>

#include "include/proxy.h"
#include "include/type.h"
#include "common/clynn/rwlock.h"
#include "executor/task_entity_pool.h"

using std::string;
using boost::shared_ptr;
using clynn::RWLock;

class VM {
public:
    // TODO
    explicit VM(const TaskInfo& info) {
        m_id = info.id;
        m_info = info;
        m_type = info.type;
    }

    ~VM() {}

    TaskID GetID() const;

    string GetName() const;

    VMType::type GetVMType() const;

    TaskInfo GetTaskInfo() const;

    // VMState::type GetState();

    TaskPtr GetTaskPtr();

    //RWLock& GetLock() {
    //    return m_lock;
    // }

    // HbVMInfo GetHbVMInfo();

    bool IsRun() const;

    string GetAppName() const;

    // void SetState(VMState::type state);

    void SetNameByString(string name);

    // key function
    // virtual void SetName();
    virtual int32_t Execute() = 0; // total entry

    virtual int32_t CreateVM() = 0; // create enviroment, kvm or lxc

    //virtual int32_t InstallApp() = 0; // install the app 

    //virtual int32_t StartApp() = 0;  // start the app

    virtual bool Stop() = 0;

    virtual bool Kill() = 0;

    //virtual VMState::type GetState() = 0;

    virtual HbVMInfo GetHbVMInfo() = 0; // get heartbeart

    virtual void SetHbVMInfo(const string& hb_vm_info_ad) = 0; // in vm send hb

    // general

private:
    virtual void SetName() = 0;

private:
    TaskID m_id;
    string m_name;     // vm name, maybe kvm name or lxc name
    VMType::type m_type; // VM_KVM or VM_LXC
    TaskInfo m_info;   // TODO
    // VMState::type m_state;
protected:
    RWLock m_lock;

    // hb and state
    // HbVMInfo m_hb_vm_info;
    // time_t m_start_time;
    // int32_t m_timestamp;
    // int32_t m_time_to_death;
};

typedef shared_ptr<VM> VMPtr;

#endif
