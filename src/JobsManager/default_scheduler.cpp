/********************************
 FileName: JobsManager/default_scheduler.cpp
 Author:   WangMin
 Date:     2014-05-21
 Version:  0.2
 Description: Default Scheduler, inherit from JobScheduler
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "common/clynn/timer.h"
#include "common/clynn/rpc.h"
#include "common/classad/classad_complement.h"
#include "include/classad_attr.h"

#include "JobsManager/default_scheduler.h"

DECLARE_string(resource_scheduler_endpoint);

using log4cplus::Logger;
using clynn::Timer;

static Logger logger = Logger::getInstance("JobsManager");

int32_t DefaultScheduler::ScheduleOneJob(JobPtr& job_ptr) {
    printf("Default Scheduler\n");

    bool has_success = false;
    bool has_no_success = false;

    list<TaskPtr> task_list = job_ptr->GetTaskList();
    for (list<TaskPtr>::const_iterator it = task_list.begin();
        it != task_list.end(); ++it)
    {
        // only schedule waiting task
        if((*it)->GetTaskState() != TASK_WAITING) {
            continue;
        }

        if (ScheduleOneTask(*it) != 0) {
            has_no_success = true; // has task no success
        } else {
            has_success = true; // has task success 
        }
    }

     if (!has_no_success) {
        // all tasks success
        job_ptr->SetState(JOB_RUNNING);
        return 0;
    } else if (has_success) {
        // partly tasks success
        job_ptr->SetState(JOB_SCHEDULING);
        return -1;
    } else {
        // all tasks no success
        return -2;
    }
    return 0;

}

int32_t DefaultScheduler::ScheduleOneTask(const TaskPtr& task_ptr) {
    // set task_ad & hard constraints
    ClassAdPtr taskad_hard_constraint(new ClassAd(*task_ptr->GetTaskHardClassAd()));
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
    if (SendRequestConstraint(taskad_hard, soft_list, result) != 0) {
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
 
    // write map & task
    vector<string> vec_ip_port;
    StringUtility::Split(result.c_str(), vec_ip_port, ":");
    if (vec_ip_port.size() != 2) {
        LOG4CPLUS_ERROR(logger, "result is illegal" << result);
        return -1;
    }
    job_ptr->WriteMap(task_id, vec_ip_port[0]);
    task_ptr->SetEsIp(vec_ip_port[0]);

    // add used resource
    GroupPoolI::Instance()->AddUsedResource(task_ptr);
    task_ptr->TaskStarting();

    return 0;
}
