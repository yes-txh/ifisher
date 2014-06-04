/********************************
 FileName: JobsManager/wd_scheduler.h
 Author:   WangMin
 Date:     2014-05-21
 Version:  0.1
 Description: wide distribution Scheduler, inherit from JobScheduler
*********************************/

#ifndef JOBS_MANAGER_WD_SCHEDULER_H
#define JOBS_MANAGER_WD_SCHEDULER_H

#include "JobsManager/job_scheduler.h"

class WDScheduler : public JobScheduler{
public:
    WDScheduler() : JobScheduler(WIDE_DISTRIBUTION) {}
    int32_t ScheduleOneJob(JobPtr& job_ptr);
    int32_t ScheduleOneTask(const TaskPtr& task_ptr);
};

#endif
