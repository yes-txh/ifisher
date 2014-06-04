/********************************
 FileName: JobsManager/job_master.h
 Author:   ZhangZhang & WangMin
 Date:     2014-04-08
 Version:  0.1
 Description: master of navigating job, making resource request for job
*********************************/

#ifndef JOBS_MANAGER_JOB_MASTER_H
#define JOBS_MANAGER_JOB_MASTER_H

#include <list>
#include "JobsManager/job_match_data_type.h"
#include "JobsManager/job.h"
#include "JobsManager/job_matcher.h"

using std::list;

class JobMaster {
public:
    JobMaster(const JobPtr& job_ptr) : m_navigating_job(job_ptr) {}
    JobMaster() {}
    int32_t MakeMatch();
    int32_t MakeMatchOneTask(const TaskPtr& task_ptr);
    int32_t StartTasks();
    // int ScheduleNavigatingJobAutoLock();

private:
    int32_t MakeMatchAsTasks();
    int32_t MakeMatchOneTaskDefault(const TaskPtr& task_ptr);
    int32_t MakeMatchAsWhole();
    int32_t MakeMatchWideDistribution();
    int32_t MakeMatchOneTaskWD(const TaskPtr& task_ptr);

    int32_t GetTaskMatchList(list<TaskMatchInfo>& task_match_list);
    int32_t GetOneTaskMatchInfo(const TaskPtr& task_info, TaskMatchInfo& task_match);
 
    int ScheduleNavigatingJobAsTask();
    int ScheduleNavigatingJobAsWhole();

private:
    JobMatcher m_job_matcher; // scheduling agent
    JobPtr m_navigating_job;
    list<TaskMatchInfo> m_match_list;

    // constraints among tasks
    // list<TaskClusterPtr> m_task_cluster_list;
    // <task_id, endpoint>   
    //map<int32_t, string> m_id_to_ip_map;
};

#endif
