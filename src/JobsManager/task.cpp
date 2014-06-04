/********************************
 FileName: JobsManager/task.cpp
 Author:   ZhangZhang & WangMin
 Date:     2014-04-10
 Version:  0.1
 Description: task, include ClassAd m_taskad_hard_constraint and list<ClassAd> m_soft_constraint_list
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "common/clynn/rpc.h"
#include "include/proxy.h"
#include "include/classad_attr.h"
#include "JobsManager/task.h"
#include "JobsManager/group_pool.h"
#include "JobsManager/job_master.h"

using log4cplus::Logger;
using clynn::ReadLocker;
using clynn::WriteLocker;

static Logger logger = Logger::getInstance("JobsManager");

DECLARE_int32(max_restart_times);

int32_t Task::s_max_times = 3;

void Task::InitTaskInfo(TaskAdPtr ads, int32_t job_id, int32_t 
                             task_id, int32_t submit_time) {
    // static init
    /* if (m_max_times = -1)
         m_max_times = FLAGS_max_restart_times;*/
    // hard constraint   
    m_taskad_hard_constraint = ads->taskad_hard_constraint;
    m_job_id = job_id;
    m_taskad_hard_constraint->InsertAttr(ATTR_JOB_ID, job_id);
    // TODO 
    // m_task_id = task_id;
    // m_taskad_hard_constraint->InsertAttr(ATTR_TASK_ID, task_id);
    m_taskad_hard_constraint->EvaluateAttrNumber(ATTR_TASK_ID, m_task_id);
    m_taskad_hard_constraint->EvaluateAttrString(ATTR_GROUP, m_group_name);
    m_taskad_hard_constraint->EvaluateAttrNumber(ATTR_PRIO, m_raw_priority);
    m_taskad_hard_constraint->EvaluateAttrNumber(ATTR_SCHED_MODEL, m_sched_model);
    m_taskad_hard_constraint->EvaluateAttrNumber(ATTR_NEED_CPU, m_need_cpu);
    m_taskad_hard_constraint->EvaluateAttrNumber(ATTR_NEED_MEMORY, m_need_memory);
    m_taskad_hard_constraint->EvaluateAttrNumber(ATTR_NEED_MEMORY, m_timeout);
    m_timeout = m_timeout * 60;

    m_restart_times = 0;
    m_failed_times = 0;
    m_timeout_times = 0;
    m_missed_times = 0;


    // hard constraint
    SetTaskHardClassAd();

    // soft constraint list
    for (list<ClassAdPtr>::const_iterator it = ads->soft_constraint_list.begin();
         it != ads->soft_constraint_list.end(); ++it)
    {
        ClassAdPtr soft_ad = *it;
        m_soft_constraint_list.push_back(soft_ad);
    }

    // set state
    SetTaskState(TASK_WAITING);
}

void Task::SetTaskHardClassAd() {
    ClassAdParser parser;
    ExprTree* rank_expr = m_taskad_hard_constraint->Lookup(ATTR_JOB_RANK);
    if (NULL == rank_expr) {
        // add attr_job_rank 
        rank_expr = parser.ParseExpression("1-" + ATTR_LOAD_AVG);
        m_taskad_hard_constraint->Insert(ATTR_JOB_RANK, rank_expr);
    }

    // check hard requirement
    ExprTree* hard_expr = m_taskad_hard_constraint->Lookup(ATTR_HARD_REQUIREMENT);
    if (NULL == hard_expr) {
        // add attr_hard_requirement 
        hard_expr = parser.ParseExpression(
            ATTR_NEED_CPU + " <= " + ATTR_AVAIL_CPU + " && " +
            ATTR_NEED_MEMORY + " <= " + ATTR_AVAIL_MEMORY);
        m_taskad_hard_constraint->Insert(ATTR_HARD_REQUIREMENT, hard_expr);
        // m_taskad_hard_constraint->Insert(ATTR_TASK_REQUIREMENT, hard_expr);
    } else {
        // make up new hard requirement
        string old_hard = "";
        m_taskad_hard_constraint->EvaluateAttrString(ATTR_HARD_REQUIREMENT, old_hard);
        hard_expr = parser.ParseExpression(
            old_hard + " && " +
            ATTR_NEED_CPU + "<=" + ATTR_AVAIL_CPU + " && " +
            ATTR_NEED_MEMORY + "<=" + ATTR_AVAIL_MEMORY);
        m_taskad_hard_constraint->Insert(ATTR_HARD_REQUIREMENT, hard_expr);
        // m_taskad_hard_constraint->Insert(ATTR_TASK_REQUIREMENT, hard_expr); 
    }
}

int32_t Task::GetTaskId() {
    return m_task_id;
}

int32_t Task::GetJobId() {
    return m_job_id;
}

string Task::GetGroupName() {
    return m_group_name;
}

int32_t Task::GetRawPriority() {
    return m_raw_priority;
}

int32_t Task::GetSchedModel() {
    return m_sched_model;
}

double Task::GetNeedCpu() {
    return m_need_cpu;
}

int32_t Task::GetNeedMemory() {
    return m_need_memory;
}

TaskState Task::GetTaskState() {
    ReadLocker locker(m_lock);
    return m_task_state;
}

void Task::SetTaskState(int32_t state) {
    WriteLocker locker(m_lock);
    m_task_state = TaskState(state);
}

bool Task::StartTask(const string& endpoint, const string& taskad) {
    try {
        Proxy<ExecutorClient> proxy = RpcClient<ExecutorClient>::GetProxy(endpoint);
        proxy().StartTask(taskad);
    } catch (TException &tx) {
        LOG4CPLUS_ERROR(logger, "StartTaskUnSuccess To " << endpoint);
        return false;
    }
    LOG4CPLUS_INFO(logger, "StartTask job_id: " << m_job_id << ", task_id: " << m_task_id << " To " << endpoint << " TaskAd: " << taskad);
    return true;
}

void Task::TaskStarting() {
    SetTaskState(TaskState::TASK_STARTING);
    m_start_time = time(NULL);    
    LOG4CPLUS_INFO(logger, "job id: " << m_job_id << ", task id: " << m_task_id << " starting");
}

void Task::TaskStarted() {
    SetTaskState(TaskState::TASK_RUNNING);
    LOG4CPLUS_INFO(logger, "job id: " << m_job_id << ", task id: " << m_task_id << " started");
}

void Task::TaskFinished() {
    SetTaskState(TaskState::TASK_FINISHED);
    LOG4CPLUS_INFO(logger, "job id: " << m_job_id << ", task id: " << m_task_id << " finished");
    GroupPoolI::Instance()->ReleaseUsedResource(shared_from_this());
}

void Task::TaskFailed() {
    m_failed_times++;
    SetTaskState(TaskState::TASK_FAILED);
    LOG4CPLUS_INFO(logger, "job id: " << m_job_id << ", task id: " << m_task_id << " failed");
    RestartTask();
}

void Task::TaskTimeout() {
    m_timeout_times++;
    LOG4CPLUS_INFO(logger, "job id: " << m_job_id << ", task id: " << m_task_id << " timeout");
    RestartTask();
}

void Task::TaskMissed() {
    m_missed_times++;
    SetTaskState(TaskState::TASK_MISSED);
    LOG4CPLUS_INFO(logger, "job id: " << m_job_id << ", task id: " << m_task_id << " missed");
    RestartTask();
}

void Task::RestartTask() {
    if (m_restart_times < s_max_times) {
        // restart   
        m_restart_times++;

        // new job scheduler 
        SchedulerPtr scheduler;
        switch (GetSchedModel()) {
            case DEFAULT_MODEL:
                scheduler = SchedulerPtr(new DefaultScheduler());
                break;
            case WIDE_DISTRIBUTION:
                scheduler = SchedulerPtr(new WDScheduler());
                break;
            default:
                scheduler = SchedulerPtr(new DefaultScheduler());
        }

        if (scheduler->ScheduleOneTask(shared_from_this()) != 0) {
            LOG4CPLUS_ERROR(logger, "Failed to Make Resource Request.");
        } 
        LOG4CPLUS_INFO(logger, "Restarting task, job id: " << m_job_id << ", task id: " << m_task_id << ", restart times is " << m_restart_times);
    } else {
        SetTaskState(TaskState::TASK_FINISHED);
        // TODO
        LOG4CPLUS_INFO(logger, "job id: " << m_job_id << ", task id: " << m_task_id << " failed totally");
        GroupPoolI::Instance()->ReleaseUsedResource(shared_from_this());
    }
}

bool Task::IsTimeout() {
    if (m_task_state == TASK_STARTING || m_task_state == TASK_RUNNING)
        if (time(NULL) - m_start_time > 2 * m_timeout) 
            return true;
    return false;
}

bool Task::KillforFT() {
    return true;
}

void Task::AdjEdgeListPushBack(const AdjEdgePtr& edge) {
    m_adj_edge_list.push_back(edge);
}

void Task::PrintClassAds() {
    // hard constraint
    string str_hard_classad = ClassAdComplement::AdTostring(m_taskad_hard_constraint);
    printf("task hard classad: %s\n", str_hard_classad.c_str());

    // soft constraint list
    for (list<ClassAdPtr>::iterator it = m_soft_constraint_list.begin();
         it != m_soft_constraint_list.end(); ++it)
    {
        string str_soft_classad = ClassAdComplement::AdTostring(*it);
        printf("task soft classad: %s\n", str_soft_classad.c_str());
    }
  
    // adj edge list
    for (list<AdjEdgePtr>::iterator it = m_adj_edge_list.begin();
         it != m_adj_edge_list.end(); ++it)
    {
        printf("start: %d, end: %d, weight: %d\n", m_task_id, (*it)->end, (*it)->weight);
    }
}

ClassAdPtr Task::GetTaskHardClassAd() {
    return m_taskad_hard_constraint;
}

list<ClassAdPtr> Task::GetTaskSoftList() {
    return m_soft_constraint_list;
}

void Task::SetEsIp(const string& es_ip) {
    m_es_ip = es_ip;
}

void Task::GetTaskStateInfo(TaskStateInfo& task_state_info) {
    task_state_info.job_id = m_job_id;
    task_state_info.task_id = m_task_id;
    task_state_info.state = m_task_state;
    task_state_info.machine_ip = m_es_ip;
    task_state_info.restart_times = m_restart_times;
    task_state_info.failed_times = m_failed_times;
    task_state_info.timeout_times = m_timeout_times;
    task_state_info.missed_times = m_missed_times;

}
