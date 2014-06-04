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
#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "executor/task_action.h"
#include "executor/task_entity_pool.h"
#include "executor/task_entity.h"
#include "executor/dispatcher.h"


using log4cplus::Logger;
using clynn::WriteLocker;
using clynn::ReadLocker;

static Logger logger = Logger::getInstance("executor");

DECLARE_string(jobs_manager_endpoint);
DECLARE_bool(jobs_manager_up);
DECLARE_bool(debug);

void TaskAction::TaskRunning(TaskID id) {
    TaskPtr task_ptr = TaskPoolI::Instance()->GetTaskPtr(id);
    TaskEntityState::type m_state = task_ptr->GetState();
    //update task state to JM
    if(m_state != TaskEntityState::TASKENTITY_RUNNING) {
        try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskStarted(id.job_id, id.task_id);
            } catch (TException &tx) {
              LOG4CPLUS_ERROR(logger, "Update running of task state to JM error: " << tx.what());
            } 	
    }

    //update task state
    task_ptr->SetState(TaskEntityState::TASKENTITY_RUNNING);
    if(FLAGS_debug) {
        LOG4CPLUS_INFO(logger, "Task has running, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
}

void TaskAction::TaskFinished(TaskID id) {
    //delete task object
    if (!TaskPoolI::Instance()->KillTaskByID(id)) {
            LOG4CPLUS_ERROR(logger, "Failed to kill task, job_id:" << id.job_id << ", task_id:" << id.task_id);
            return;
    }

    TaskPoolI::Instance()->Delete(id);    
    LOG4CPLUS_INFO(logger, "Task finished, handle event of kill task action, job_id:" << id.job_id << ", task_id:" << id.task_id);
    //update task state to JM
    try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskFinished(id.job_id, id.task_id);
        } catch (TException &tx) {
            LOG4CPLUS_ERROR(logger, "Update finished of task state to JM error: " << tx.what());
        }

}

void TaskAction::TaskFailed(TaskID id) {
    //delete task object
    if (!TaskPoolI::Instance()->KillTaskByID(id)) {
            LOG4CPLUS_ERROR(logger, "Failed to kill task, job_id:" << id.job_id << ", task_id:" << id.task_id);
            return ;
    }
    TaskPoolI::Instance()->Delete(id);
    LOG4CPLUS_INFO(logger, "Task failed, handle event of kill task action, job_id:" << id.job_id << ", task_id:" << id.task_id);
    //update task state to JM
    try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskFailed(id.job_id, id.task_id);
        } catch (TException &tx) {
            LOG4CPLUS_ERROR(logger, "Update failed of task state to JM error: " << tx.what());         
    }
}

void TaskAction::TaskMissed(TaskID id) {
    //delete task object
    if (!TaskPoolI::Instance()->KillTaskByID(id)) {
            LOG4CPLUS_ERROR(logger, "Failed to kill task, job_id:" << id.job_id << ", task_id:" << id.task_id);
            return ;
    }
    TaskPoolI::Instance()->Delete(id);
    LOG4CPLUS_INFO(logger, "Task missed, handle event of kill task action, job_id:" << id.job_id << ", task_id:" << id.task_id);
    //update task state to JM
    try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskMissed(id.job_id, id.task_id);
        } catch (TException &tx) {
            LOG4CPLUS_ERROR(logger, "Update missed of task state to JM error: " << tx.what());
    }
}

void TaskAction::TaskTimeout(TaskID id) {
    //delete task object
    if (!TaskPoolI::Instance()->KillTaskByID(id)) {
            LOG4CPLUS_ERROR(logger, "Failed to kill task, job_id:" << id.job_id << ", task_id:" << id.task_id);
            return ;
    }
    TaskPoolI::Instance()->Delete(id);
    LOG4CPLUS_INFO(logger, "Task timeout, handle event of kill task action, job_id:" << id.job_id << ", task_id:" << id.task_id);
    //update task state to JM
    try {
            Proxy<JobsManagerClient> proxy = RpcClient<JobsManagerClient>::GetProxy(FLAGS_jobs_manager_endpoint);
            proxy().TaskTimeout(id.job_id, id.task_id);
        } catch (TException &tx) {
            LOG4CPLUS_ERROR(logger, "Update timeout of task state to JM error: " << tx.what());
    }
}

