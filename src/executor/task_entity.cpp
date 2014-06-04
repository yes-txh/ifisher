/********************************
 FileName: executor/task_entity.cpp
 Author:   WangMin & TangXuehai
 Date:     2013-08-27 & 2014-05-10
 Version:  0.1
 Description: task entity in executor, corresponds to task, corresponds to kvm/lxc
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include <iostream>
#include <classad/classad.h>
#include <classad/classad_distribution.h>

#include "include/classad_attr.h"
#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "executor/task_entity.h"
#include "executor/vm_pool.h"
#include "executor/ip_pool.h"
#include "executor/dispatcher.h"


using log4cplus::Logger;
using clynn::WriteLocker;
using clynn::ReadLocker;

static Logger logger = Logger::getInstance("executor");

DECLARE_string(jobs_manager_endpoint);
DECLARE_bool(jobs_manager_up);
DECLARE_bool(debug);

TaskEntity::TaskEntity(const string& task_info) {
    m_string_info = task_info;
    m_state = TaskEntityState::TASKENTITY_WAITING;
    m_percentage = 0;
}

bool TaskEntity::IsLegal() {
    // classad init, string task_info --> ClassAd *ad_ptr
    ClassAdParser parser;
    ClassAd* ad_ptr = parser.ParseClassAd(m_string_info);

    if (!ad_ptr) {
        LOG4CPLUS_ERROR(logger, "Failed to parse classad, ClassAd pointer is NULL."); 
        return false;
    }
    // task overview
    if (!ad_ptr->EvaluateAttrNumber(ATTR_JOB_ID, m_info.id.job_id)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_JOB_ID << " error.");
        return false;
    }

    if (!ad_ptr->EvaluateAttrNumber(ATTR_TASK_ID, m_info.id.task_id)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_TASK_ID << " error.");
        return false;
    }

    int32_t type = -1;
    if (!ad_ptr->EvaluateAttrNumber(ATTR_VMTYPE, type)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VMTYPE << " error.");
        return false;
    }
    m_info.type = VMType::type(type);
    // printf("VMType: %d\n", m_info.type);

    if (!ad_ptr->EvaluateAttrBool(ATTR_IS_RUN, m_info.is_run)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_IS_RUN << " error.");
        return false;
    }

    // seconds to min
    int32_t timeout;
    if (!ad_ptr->EvaluateAttrNumber(ATTR_TIMEOUT, timeout)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_TIMEOUT << " error.");
        return false;
    }
    m_info.timeout = 60 * timeout;
    
    if (!ad_ptr->EvaluateAttrString(ATTR_USER, m_info.user)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_USER << " error.");
        return false;
    }

    // task vm_info
    if (!ad_ptr->EvaluateAttrNumber(ATTR_MEMORY, m_info.vm_info.memory)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_MEMORY << " error.");
        return false;
    }

    if (!ad_ptr->EvaluateAttrNumber(ATTR_VCPU, m_info.vm_info.vcpu)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VCPU << " error.");
        return false;
    }
    /*
    if (!ad_ptr->EvaluateAttrString(ATTR_IP, m_info.vm_info.ip)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_IP << " error.");
        return false;
    }
    */
    if (!ad_ptr->EvaluateAttrNumber(ATTR_PORT, m_info.vm_info.port)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_PORT << " error.");
        return false;
    }
    /*
    if (!ad_ptr->EvaluateAttrString(ATTR_OS, m_info.vm_info.os)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_OS << " error.");
        return false;
    }
    */
    // only for kvm 
    if (!ad_ptr->EvaluateAttrString(ATTR_Image, m_info.vm_info.img_template)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_IMG << " error.");
        return false;
    }

    if (!ad_ptr->EvaluateAttrNumber(ATTR_SIZE, m_info.vm_info.size)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_SIZE << " error.");
        return false;
    }

    if (!ad_ptr->EvaluateAttrNumber(ATTR_VNC_PORT, m_info.vm_info.vnc_port)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VNC_PORT << " error.");
        return false;
    }

    if (!ad_ptr->EvaluateAttrBool(ATTR_RPC_RUNNING, m_info.vm_info.rpc_running)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_VNC_PORT << " error.");
        return false;
    }
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "IS_RUN?: " << m_info.is_run );
    }
    // is_run = false, no app_info
    if (false == m_info.is_run) {
        // only create vm, not install or run app
        m_id = m_info.id;
        m_state = TaskEntityState::TASKENTITY_WAITING;
        m_percentage = 0;
        return true;
    }

    // task, app_info
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_NAME, m_info.app_info.name)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_APP_NAME << " error.");
        return false;
    }

    // task, app_file
    if (!ad_ptr->EvaluateAttrString(ATTR_APP_FILE, m_info.app_info.app_file)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_APP_FILE << " error.");
        return false;
    }

    //exe and argument
    if (!ad_ptr->EvaluateAttrString(ATTR_EXE_PATH, m_info.app_info.exe)) {
        LOG4CPLUS_ERROR(logger, "Parse " << ATTR_EXE_PATH << " error.");
        return false;
    }
    //get ip from ip_pool
    m_info.vm_info.ip = IPPoolI::Instance()->GetAvailIp();
    m_id = m_info.id;
    return true;
}

TaskEntityState::type TaskEntity::GetState() {
    ReadLocker locker(m_lock);
    return m_state;
}


double TaskEntity::GetPercentage() {
    ReadLocker locker(m_lock);
    return m_percentage;
}

bool TaskEntity::SetStates(const TaskEntityState::type state, const double percentage) {
   WriteLocker locker(m_lock);
   m_state = state;
   m_percentage = percentage;
   return true;
}

bool TaskEntity::SetState(const TaskEntityState::type state) {
    WriteLocker locker(m_lock);
    m_state = state;
    return true;
}


bool TaskEntity::SetPercentage(const double percentage) {
    WriteLocker locker(m_lock);
    m_percentage = percentage;
    return true;
}

void TaskEntity::TaskStarting() {
    WriteLocker locker(m_lock);
    m_state = TaskEntityState::TASKENTITY_STARTING;
    m_percentage = 0.0;
    if(FLAGS_debug) {
        LOG4CPLUS_INFO(logger, "Task has starting, job_id:" << m_id.job_id << ", task_id:" << m_id.task_id);
    }
}

void TaskEntity::TaskRunning() {
    //update task state to JM
    if(m_state != TaskEntityState::TASKENTITY_RUNNING) {
        try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskStarted(m_id.job_id, m_id.task_id);
            } catch (TException &tx) {
              LOG4CPLUS_ERROR(logger, "Update running of task state to JM error: " << tx.what());
            } 	
    }

    //update task state
    WriteLocker locker(m_lock);
    m_state = TaskEntityState::TASKENTITY_RUNNING;
    if(FLAGS_debug) {
        LOG4CPLUS_INFO(logger, "Task has running, job_id:" << m_id.job_id << ", task_id:" << m_id.task_id);
    }
}

void TaskEntity::TaskFinished() {
    //update task state to JM
    if(m_state != TaskEntityState::TASKENTITY_FINISHED) {
        try {
                Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
                proxy().TaskFinished(m_id.job_id, m_id.task_id);
            } catch (TException &tx) {
                LOG4CPLUS_ERROR(logger, "Update finished of task state to JM error: " << tx.what());
            }
    }

    //update task state
    WriteLocker locker(m_lock);
    m_state = TaskEntityState::TASKENTITY_FINISHED;
    m_percentage = 100.0;
    
    if(FLAGS_debug) {
        LOG4CPLUS_INFO(logger, "Task has finished, job_id:" << m_id.job_id << ", task_id:" << m_id.task_id);
    } else { 
        //kill task and vm
        //new event
        EventPtr event(new KillActionEvent(m_id));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    }
}

void TaskEntity::TaskFailed() {
    //update task state to JM
    if(m_state != TaskEntityState::TASKENTITY_FAILED) {
        try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskFailed(m_id.job_id, m_id.task_id);
            } catch (TException &tx) {
              LOG4CPLUS_ERROR(logger, "Update failed of task state to JM error: " << tx.what());
            }
    }

    //update task state
    WriteLocker locker(m_lock);
    m_state = TaskEntityState::TASKENTITY_FAILED;
    m_percentage = 0.0;
    
    if(FLAGS_debug) {
        LOG4CPLUS_INFO(logger, "Task has failed, job_id:" << m_id.job_id << ", task_id:" << m_id.task_id);
    } else {
        //kill task and vm
        //new event
        EventPtr event(new KillActionEvent(m_id));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    }
}

void TaskEntity::TaskMissed() {
    //update task state to JM
    if(m_state != TaskEntityState::TASKENTITY_MISSED) {
        try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskMissed(m_id.job_id, m_id.task_id);
            } catch (TException &tx) {
              LOG4CPLUS_ERROR(logger, "Update missed of task state to JM error: " << tx.what());
            }
    }
  
    //update task state
    WriteLocker locker(m_lock);
    m_state = TaskEntityState::TASKENTITY_MISSED;
    m_percentage = 0.0;

    if(FLAGS_debug) {
        LOG4CPLUS_INFO(logger, "Task has missed, job_id:" << m_id.job_id << ", task_id:" << m_id.task_id);
    } else {
        //kill task and vm
        //new event
        EventPtr event(new KillActionEvent(m_id));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    }
}

void TaskEntity::TaskTimeout() {
    //update task state to JM
    try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskTimeout(m_id.job_id, m_id.task_id);
         } catch (TException &tx) {
              LOG4CPLUS_ERROR(logger, "Update timeout of task state to JM error: " << tx.what());
         }

    //update task state
    WriteLocker locker(m_lock);
    m_state = TaskEntityState::TASKENTITY_FAILED;
    m_percentage = 0.0;

    if(FLAGS_debug) {
        LOG4CPLUS_INFO(logger, "Task has timeout, job_id:" << m_id.job_id << ", task_id:" << m_id.task_id);
    } else {
        //kill task and vm
        //new event
        EventPtr event(new KillActionEvent(m_id));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    }
}


// TODO
bool TaskEntity::Start() {
    //WriteLocker locker(m_lock);
    // LOG4CPLUS_DEBUG(logger, "Begin to start the task, job_id:" << m_id.job_id << ", task_id:" << m_id.task_id);

    if (GetVMType() == VMType::VM_KVM) {
        // init vm
        VMPtr ptr(new KVM(m_info));
        // insert VMPtr into VMPool
        VMPoolI::Instance()->Insert(ptr);
    } else if (GetVMType() == VMType::VM_LXC) {
        // init vm
        VMPtr ptr(new LXC(m_info));
        // insert VMPtr into VMPool
        VMPoolI::Instance()->Insert(ptr);
    } else {
        LOG4CPLUS_ERROR(logger, "Failed to start task, job_id:" << m_id.job_id << ", task_id:" << m_id.task_id 
                        << ", because there is no the VMType " << m_info.type);
        return false;
    }
    // update task  state into starting
    // new startActionEvent
    EventPtr event(new StartingTaskEvent(m_id));
    // Push event into Queue
    EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);

    return true;
}

bool TaskEntity::Stop() {
    if (!VMPoolI::Instance()->StopVMByTaskID(m_id)) {
        LOG4CPLUS_ERROR(logger, "Failed to stop task, job_id:" << m_id.job_id << ", task_id" << m_id.task_id);
        return false;
    }
    //TaskStoped();
    return true;
}

bool TaskEntity::Kill() {
    if (!VMPoolI::Instance()->KillVMByTaskID(m_id)) {
        LOG4CPLUS_ERROR(logger, "Failed to kill task, job_id:" << m_id.job_id << ", task_id" << m_id.task_id);
        return false;
    }
    //TaskFinished();
    //reback ip to ip_pool
    IPPoolI::Instance()->ReleaseIp(m_info.vm_info.ip);
    return true;
}
