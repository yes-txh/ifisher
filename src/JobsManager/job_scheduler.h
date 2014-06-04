/********************************
 FileName: JobsManager/job_scheduler.h
 Author:   WangMin
 Date:     2014-05-21
 Version:  0.2
 Description: scheduler of navigating job, making resource request for job
*********************************/

#ifndef JOBS_MANAGER_JOB_SCHEDULER_H
#define JOBS_MANAGER_JOB_SCHEDULER_H

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

#include "JobsManager/job.h"
#include "JobsManager/task.h"

// used in child class
#include "common/clynn/string_utility.h"
#include "JobsManager/job_enum.h"
#include "JobsManager/job_pool.h"
#include "JobsManager/group_pool.h"
#include "JobsManager/event.h"
#include "JobsManager/dispatcher.h"

using std::string;
using std::vector;
using boost::shared_ptr;

class JobScheduler {
public:
    JobScheduler(SchedModel model) : m_model(model) {}
    virtual int32_t ScheduleOneJob(JobPtr& job_ptr) = 0;
    virtual int32_t ScheduleOneTask(const TaskPtr& task_ptr) = 0;
protected:
    // int32_t MatchTaskList(list<TaskMatchInfo>& task_match_list);
    int32_t SendRequest(const string& classad, string& match_result);
    int32_t SendRequestConstraint(const string& taskad_hard, const vector<string>& soft_list, string& match_result);
    SchedModel m_model;
};

typedef shared_ptr<JobScheduler> SchedulerPtr;

#endif
