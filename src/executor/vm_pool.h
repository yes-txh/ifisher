/********************************
 FileName: executor/vm_pool.h
 Author:   WangMin
 Date:     2013-09-04
 Version:  0.1
 Description: the pool of vm, include kvm and lxc
*********************************/

#ifndef SRC_EXECUTOR_VM_POOL_H
#define SRC_EXECUTOR_VM_POOL_H

#include <map>
#include <queue>
#include <vector>
#include <tr1/functional>
#include <sys/types.h>

#include "common/clynn/singleton.h"
#include "common/clynn/rwlock.h"
#include "executor/vm.h"
#include "executor/kvm.h"
#include "executor/lxc.h"

using std::map;
using std::queue;
using std::vector;
using std::tr1::function;
using std::tr1::placeholders::_1;
using std::tr1::placeholders::_2;
using clynn::RWLock;

class VMPool {
public:
    // function pointer, from std::tr1::function
    typedef function<void(VM*)> VMFunc;

    // check & mkdir libvirt & lxc
    bool Init();

    // TODO print all
    void PrintAll();

    // insert VMPtr into map and queue
    void Insert(const VMPtr& ptr);

    // insert VMPtr into map
    void InsertIntoPool(const VMPtr& ptr);

    // insert VMPtr into queue
    void InsertIntoQueue(const VMPtr& ptr);

    // delete VMPtr from map
    void DeleteFromPool(const TaskID id);

    // find vm by job_id and task_id
    bool FindByTaskID(const TaskID id);

    // @brief: find a waiting VM, and start it
    int32_t StartVM();

    // @brief: stop vm by job_id and task_id 
    bool StopVMByTaskID(const TaskID id);

    // @brief: kill vm by job_id and task_id
    bool KillVMByTaskID(const TaskID id);

    // get VMPtr from 
    VMPtr GetVMPtr(const TaskID id);
   
    // get all HbVMInfo
    vector<HbVMInfo> GetAllHbVMInfo();

    // process HbKVM
    bool ProcessHbVMInfo(const string& hb_vm_info);

    double GetAllocatedCPU();

    int32_t GetAllocatedMemory();

    void SetVMStateByTaskID(const TaskID id, bool task_state);

    bool GetVMStateByTaskID(const TaskID id);
private:
    RWLock m_lock;
    RWLock m_new_lock;
    // task_id, VMPtr
    map<TaskID, VMPtr> m_vm_map;
    //task_id, vm_state
    map<TaskID, bool> m_vm_state_map;
    // to be processed queue
    queue<VMPtr> m_queue;
};

typedef Singleton<VMPool> VMPoolI;

#endif 
