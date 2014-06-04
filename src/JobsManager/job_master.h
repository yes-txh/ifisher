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
#include "JobsManager/job.h"
#include "JobsManager/job_scheduler.h"
#include "JobsManager/default_scheduler.h"
#include "JobsManager/wd_scheduler.h"

using std::list;

class JobMaster {
public:
    JobMaster(const JobPtr& job_ptr);
    // JobMaster();
    int32_t Schedule();
    // int32_t ScheduleOneTask(const TaskPtr& task_ptr);

/*     int32_t MakeMatch();
    int32_t MakeMatchOneTask(const TaskPtr& task_ptr);
    int32_t StartTasks();

private:
    int32_t MakeMatchAsTasks();
    int32_t MakeMatchOneTaskDefault(const TaskPtr& task_ptr);
    int32_t MakeMatchAsWhole();
    int32_t MakeMatchWideDistribution();
    int32_t MakeMatchOneTaskWD(const TaskPtr& task_ptr);

    int32_t GetTaskMatchList(list<TaskMatchInfo>& task_match_list);
    int32_t GetOneTaskMatchInfo(const TaskPtr& task_info, TaskMatchInfo& task_match);
 
    int ScheduleNavigatingJobAsTask();
    int ScheduleNavigatingJobAsWhole();*/

private:
    SchedulerPtr m_scheduler; // scheduling agent
    JobPtr m_navigating_job;
};

#endif
