/********************************
 FileName: JobsManager/default_scheduler.h
 Author:   WangMin
 Date:     2014-05-21
 Version:  0.1
 Description: default Scheduler, inherit from JobScheduler
*********************************/

#ifndef JOBS_MANAGER_DEFAULT_SCHEDULER_H
#define JOBS_MANAGER_DEFAULT_SCHEDULER_H

#include "JobsManager/job_scheduler.h"

class DefaultScheduler : public JobScheduler{
public:
    DefaultScheduler() : JobScheduler(DEFAULT_MODEL) {}
    int32_t ScheduleOneJob(JobPtr& job_ptr);
    int32_t ScheduleOneTask(const TaskPtr& task_ptr);
};

#endif
