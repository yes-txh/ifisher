/********************************
 FileName: JobsManager/job_master.cpp
 Author:   ZhangZhang & WangMin
 Date:     2014-04-08
 Version:  0.1
 Description: master of navigating job, making resource request for job
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "include/proxy.h"
#include "include/classad_attr.h"
#include "common/clynn/rpc.h"
#include "common/clynn/rwlock.h"
#include "common/clynn/string_utility.h"
#include "common/classad/classad_complement.h"

#include "JobsManager/job_enum.h"
#include "JobsManager/job_master.h"
#include "JobsManager/job_pool.h"
#include "JobsManager/group.h"
#include "JobsManager/group_pool.h"
#include "JobsManager/job_match_data_type.h"
#include "JobsManager/event.h"
#include "JobsManager/dispatcher.h"

using clynn::ReadLocker;
using clynn::WriteLocker;
using log4cplus::Logger;

static Logger logger = Logger::getInstance("JobsManager");

int32_t JobMaster::MakeMatch() {
    printf("Making match.\n");

    if (NULL == m_navigating_job) { 
        // 错误码TODO
        return -1;
    }

    int32_t rc;
    int32_t sched_model = m_navigating_job->GetSchedModel();
 
    if (WIDE_DISTRIBUTION == sched_model) {
        rc = MakeMatchWideDistribution();
    } else {
        if (false == m_navigating_job->GetConstraintsAmongTasks()) {
            rc = MakeMatchAsTasks();
        } else {
            rc = MakeMatchAsWhole();
        }
    }

    return rc;
}

int32_t JobMaster::MakeMatchOneTask(const TaskPtr& task_ptr) {
    printf("Making match One Task.\n");

    int32_t rc;
    int32_t sched_model = task_ptr->GetSchedModel();


     if (WIDE_DISTRIBUTION == sched_model) {
        rc = MakeMatchOneTaskWD(task_ptr);
    } else {
        rc = MakeMatchOneTaskDefault(task_ptr);
    }

    return rc;
}

int32_t JobMaster::StartTasks() {
    for (list<TaskMatchInfo>::iterator it = m_match_list.begin();
        it != m_match_list.end(); ++it)
    {
        EventPtr event(new StartTaskEvent(it->job_id, it->task_id, it->es_address, it->taskad_hard_constraint));
        // Push event into Queue
        EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);

        // state
        JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(it->job_id);
        if (NULL == job_ptr) {
            return -1;
        }
        TaskPtr task_ptr = job_ptr->GetTaskPtr(it->task_id);
        if (NULL == task_ptr) {
            return -1;
        }
        task_ptr->TaskStarting();
    }
    return 0;
}
 
int32_t JobMaster::MakeMatchAsTasks() {
    int32_t rc = GetTaskMatchList(m_match_list);
    if (rc != 0) {
        return rc;
    }

    rc = m_job_matcher.MatchTaskList(m_match_list);
    if (rc != 0) {
        return rc;
    }

    for (list<TaskMatchInfo>::iterator it = m_match_list.begin();
        it != m_match_list.end(); ++it) 
    {
        // string parse
        vector<string> vec_ip_port;
        StringUtility::Split(it->es_address.c_str(), vec_ip_port, ":");
        if (vec_ip_port.size() != 2) {
            LOG4CPLUS_ERROR(logger, "es_address is illegal" << it->es_address);
            return -1;
        }
        m_navigating_job->WriteMap(it->task_id, vec_ip_port[0]);
        TaskPtr task_ptr = m_navigating_job->GetTaskPtr(it->task_id);
        task_ptr->SetEsIp(vec_ip_port[0]); 
    }
    return 0;
} 

int32_t JobMaster::MakeMatchAsWhole() {
    printf("AsWhole\n");
    // 1 build m_task_cluster_list 
    // 2 sort m_task_cluster_list according tasks cpu
    // 3 init m_id_to_ip_map
    // 4 bianli m_task_cluster_list
    //     5 bianli task
    //         build constraints according adj_edge_list
    //         build constraints according reserved resource, reserved value
    return 0;
}

int32_t JobMaster::MakeMatchWideDistribution() {
    printf("WideDistribution\n");
    int rc;
    list<TaskPtr> task_list = m_navigating_job->GetTaskList();
    for (list<TaskPtr>::const_iterator it = task_list.begin();
        it != task_list.end(); ++it)
    {
        // 只匹配等待状态的task
        if((*it)->GetTaskState() != TASK_WAITING) {
            continue;
        }
        // TaskMatchInfo task_match_info;
        // rc = GetOneTaskMatchInfo((*it), task_match_info, 0);
        rc = MakeMatchOneTaskWD((*it));
        // if (0 == rc) {
        //    task_match_list.push_back(task_match_info);
        // }
    }
    return 0;
}

int32_t JobMaster::MakeMatchOneTaskDefault(const TaskPtr& task_ptr) {
    // set task_ad & hard constraints
    ClassAdPtr taskad_hard_constraint(new ClassAd(*task_ptr->GetTaskHardClassAd()));
    ClassAdParser parser;
    // check rank
    ExprTree* rank_expr = taskad_hard_constraint->Lookup(ATTR_JOB_RANK);
    if (NULL == rank_expr) {
        // add attr_job_rank 
        // 这个值可能为负数
        rank_expr = parser.ParseExpression("1-" + ATTR_LOAD_AVG);
        taskad_hard_constraint->Insert(ATTR_JOB_RANK, rank_expr);
    }

    // check hard requirement
    ExprTree* hard_expr = taskad_hard_constraint->Lookup(ATTR_HARD_REQUIREMENT);
    if (NULL == hard_expr) {
        // add attr_hard_requirement 
        hard_expr = parser.ParseExpression(
            ATTR_NEED_CPU + " <= " + ATTR_AVAIL_CPU + " && " +
            ATTR_NEED_MEMORY + " <= " + ATTR_AVAIL_MEMORY);
        taskad_hard_constraint->Insert(ATTR_HARD_REQUIREMENT, hard_expr);
        // TODO
        // taskad_hard_constraint->Insert(ATTR_TASK_REQUIREMENT, hard_expr);
    } else {
        // make up new hard requirement
        string old_hard = "";
        taskad_hard_constraint->EvaluateAttrString(ATTR_HARD_REQUIREMENT, old_hard);
        hard_expr = parser.ParseExpression(
            old_hard + " && " +
            ATTR_NEED_CPU + "<=" + ATTR_AVAIL_CPU + " && " +
            ATTR_NEED_MEMORY + "<=" + ATTR_AVAIL_MEMORY);
        taskad_hard_constraint->Insert(ATTR_HARD_REQUIREMENT, hard_expr);
        // TODO
        // taskad_hard_constraint->Insert(ATTR_TASK_REQUIREMENT, hard_expr); 
    }
    string taskad_hard = ClassAdComplement::AdTostring(taskad_hard_constraint);

    // set soft constraints
    vector<string> soft_list;
    list<ClassAdPtr> classad_soft_list = task_ptr->GetTaskSoftList();
    for (list<ClassAdPtr>::iterator it = classad_soft_list.begin();
         it != classad_soft_list.end(); ++it)
    {
        string soft_constraint = ClassAdComplement::AdTostring(*it);
        soft_list.push_back(soft_constraint);
    }
    
    // match
    string result;
    if (m_job_matcher.SendRequestConstraint(taskad_hard, soft_list, result) != 0) {
        // TODO
        // continue;
        return -1;
    }
    if ("" == result) {
        // TODO
        return -1;
    }
    int32_t task_id = task_ptr->GetTaskId();
    int32_t job_id = task_ptr->GetJobId();
    JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(job_id);
    printf("Match result:\n job_id: %d, task_id: %d, result: %s\n", job_id, task_id, result.c_str());  

    // new action event
    EventPtr event(new StartTaskEvent(job_id, task_id, result, taskad_hard));
    // Push event into Queue
    EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event); 
    task_ptr->TaskStarting();
 
    // write map & task
    vector<string> vec_ip_port;
    StringUtility::Split(result.c_str(), vec_ip_port, ":");
    if (vec_ip_port.size() != 2) {
        LOG4CPLUS_ERROR(logger, "result is illegal" << result);
        return -1;
    }
    job_ptr->WriteMap(task_id, vec_ip_port[0]);
    task_ptr->SetEsIp(vec_ip_port[0]);
    return 0;
}

int32_t JobMaster::MakeMatchOneTaskWD(const TaskPtr& task_ptr) {
    // set task_ad & hard constraints
    ClassAdPtr taskad_hard_constraint(new ClassAd(*task_ptr->GetTaskHardClassAd()));
    ClassAdParser parser;
    // check rank
    ExprTree* rank_expr = taskad_hard_constraint->Lookup(ATTR_JOB_RANK);
    if (NULL == rank_expr) {
        // add attr_job_rank 
        // 这个值可能为负数
        rank_expr = parser.ParseExpression("1-" + ATTR_LOAD_AVG);
        taskad_hard_constraint->Insert(ATTR_JOB_RANK, rank_expr);
    }

    // check hard requirement
    ExprTree* hard_expr = taskad_hard_constraint->Lookup(ATTR_HARD_REQUIREMENT);
    if (NULL == hard_expr) {
        // add attr_hard_requirement 
        hard_expr = parser.ParseExpression(
            ATTR_NEED_CPU + " <= " + ATTR_AVAIL_CPU + " && " +
            ATTR_NEED_MEMORY + " <= " + ATTR_AVAIL_MEMORY);
        taskad_hard_constraint->Insert(ATTR_HARD_REQUIREMENT, hard_expr);
        // TODO
        // taskad_hard_constraint->Insert(ATTR_TASK_REQUIREMENT, hard_expr);
    } else {
        // make up new hard requirement
        string old_hard = "";
        taskad_hard_constraint->EvaluateAttrString(ATTR_HARD_REQUIREMENT, old_hard);
        hard_expr = parser.ParseExpression(
            old_hard + " && " +
            ATTR_NEED_CPU + "<=" + ATTR_AVAIL_CPU + " && " +
            ATTR_NEED_MEMORY + "<=" + ATTR_AVAIL_MEMORY);
        taskad_hard_constraint->Insert(ATTR_HARD_REQUIREMENT, hard_expr);
        // TODO
        // taskad_hard_constraint->Insert(ATTR_TASK_REQUIREMENT, hard_expr); 
    }
    string taskad_hard = ClassAdComplement::AdTostring(taskad_hard_constraint);

    // set soft constraints
    vector<string> soft_list;
    list<ClassAdPtr> classad_soft_list = task_ptr->GetTaskSoftList();
    for (list<ClassAdPtr>::iterator it = classad_soft_list.begin();
         it != classad_soft_list.end(); ++it)
    {
        string soft_constraint = ClassAdComplement::AdTostring(*it);
        soft_list.push_back(soft_constraint);
    }
    
    // read map & set soft constraints
    int32_t task_id = task_ptr->GetTaskId();
    int32_t job_id = task_ptr->GetJobId();
    JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(job_id);
    if (NULL == job_ptr) {
        LOG4CPLUS_ERROR(logger, "No this job, job_id: " << job_id); 
    }
    list<string> wd_soft_list = job_ptr->ReadMapAndGetSoftList(task_id);
    for (list<string>::iterator it = wd_soft_list.begin();
         it != wd_soft_list.end(); ++it)
    {
        soft_list.push_back(*it);
    } 

    // test print
    for (vector<string>::iterator it = soft_list.begin();
         it != soft_list.end(); ++it)
    {
        printf("soft: %s\n", (*it).c_str());
    }

    // match
    string result;
    if (m_job_matcher.SendRequestConstraint(taskad_hard, soft_list, result) != 0) {
        // TODO
        // continue;
        return -1;
    }
    if ("" == result) {
        // TODO
        return -1;
    }
    printf("Match result:\n job_id: %d, task_id: %d, result: %s\n", job_id, task_id, result.c_str());  

    // new action event
    EventPtr event(new StartTaskEvent(job_id, task_id, result, taskad_hard));
    // Push event into Queue
    EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event); 
    task_ptr->TaskStarting();
 
    // write map & task
    vector<string> vec_ip_port;
    StringUtility::Split(result.c_str(), vec_ip_port, ":");
    if (vec_ip_port.size() != 2) {
        LOG4CPLUS_ERROR(logger, "result is illegal" << result);
        return -1;
    }
    job_ptr->WriteMap(task_id, vec_ip_port[0]);
    task_ptr->SetEsIp(vec_ip_port[0]);
    return 0;
}

int32_t JobMaster::GetTaskMatchList(list<TaskMatchInfo>& task_match_list) {
    int rc;
    list<TaskPtr> task_list = m_navigating_job->GetTaskList();
    for (list<TaskPtr>::const_iterator it = task_list.begin(); 
        it != task_list.end(); ++it)
    {
        // 只匹配等待状态的task
        if((*it)->GetTaskState() != TASK_WAITING) {
            continue;         
        } 
        TaskMatchInfo task_match_info;
        rc = GetOneTaskMatchInfo((*it), task_match_info);
        if (0 == rc) {
            task_match_list.push_back(task_match_info);
        } 
    }
    if (task_match_list.empty()) {
        return 1;
    }
    return 0;
}

int32_t JobMaster::GetOneTaskMatchInfo(const TaskPtr& task, TaskMatchInfo& task_match) {
    ClassAdPtr taskad_hard_constraint(new ClassAd(*task->GetTaskHardClassAd()));
    ClassAdParser parser;
    // check rank
    ExprTree* rank_expr = taskad_hard_constraint->Lookup(ATTR_JOB_RANK);
    if (NULL == rank_expr) {
        // add attr_job_rank 
        // 这个值可能为负数
        rank_expr = parser.ParseExpression("1-" + ATTR_LOAD_AVG);
        taskad_hard_constraint->Insert(ATTR_JOB_RANK, rank_expr);
    }

    // check hard requirement
    ExprTree* hard_expr = taskad_hard_constraint->Lookup(ATTR_HARD_REQUIREMENT);
    if (NULL == hard_expr) {
        // add attr_hard_requirement 
        hard_expr = parser.ParseExpression(
            ATTR_NEED_CPU + " <= " + ATTR_AVAIL_CPU + " && " +
            ATTR_NEED_MEMORY + " <= " + ATTR_AVAIL_MEMORY);
        taskad_hard_constraint->Insert(ATTR_HARD_REQUIREMENT, hard_expr);
        // TODO
        // taskad_hard_constraint->Insert(ATTR_TASK_REQUIREMENT, hard_expr);
    } else {
        // make up new hard requirement
        string old_hard = "";
        taskad_hard_constraint->EvaluateAttrString(ATTR_HARD_REQUIREMENT, old_hard);
        hard_expr = parser.ParseExpression(
            old_hard + " && " +
            ATTR_NEED_CPU + "<=" + ATTR_AVAIL_CPU + " && " +
            ATTR_NEED_MEMORY + "<=" + ATTR_AVAIL_MEMORY);
        taskad_hard_constraint->Insert(ATTR_HARD_REQUIREMENT, hard_expr);
        // TODO
        // taskad_hard_constraint->Insert(ATTR_TASK_REQUIREMENT, hard_expr); 
    }

    task_match.job_id = task->GetJobId();
    task_match.task_id = task->GetTaskId();
    task_match.taskad_hard_constraint = ClassAdComplement::AdTostring(taskad_hard_constraint);
    // TODO test
   
    // soft constraint list
    list<ClassAdPtr> soft_list = task->GetTaskSoftList();
    for (list<ClassAdPtr>::iterator it = soft_list.begin();
         it != soft_list.end(); ++it)
    {
        string soft_constraint = ClassAdComplement::AdTostring(*it);
        task_match.soft_constraint_list.push_back(soft_constraint);
        // TODO test
        printf("task soft classad: %s\n", soft_constraint.c_str());
    } 
    return 0;
} 
