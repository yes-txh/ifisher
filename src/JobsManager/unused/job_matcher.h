#ifndef JOBS_MANAGER_SCHED_JOB_MATCHER_H
#define JOBS_MANAGER_SCHED_JOB_MATCHER_H

#include <string>
#include <list>
#include "JobsManager/job_match_data_type.h"

using std::string;
using std::list;
using std::vector;

class JobMatcher {
    public:
        int32_t MatchTaskList(list<TaskMatchInfo>& task_match_list);
        int32_t SendRequest(const string& classad, string& match_result);
        int32_t SendRequestConstraint(const string& taskad_hard, const vector<string>& soft_list, string& match_result);
};

#endif
