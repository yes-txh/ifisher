#ifndef JOBS_MANAGER_JOB_MATCH_DATA_TYPE_H
#define JOBS_MANAGER_JOB_MATCH_DATA_TYPE_H

#include <string>

using std::string;
using std::list;

struct TaskMatchInfo {
    int32_t job_id;
    int32_t task_id;
    string taskad_hard_constraint;
    list<string> soft_constraint_list;
    string es_name;
    string es_address;
    TaskMatchInfo(): job_id(0), task_id(0) {}
};

#endif
