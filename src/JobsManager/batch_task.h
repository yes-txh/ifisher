#ifndef JOBS_MANAGER_BATCH_TASK_H
#define JOBS_MANAGER_BATCH_TASK_H

#include <string>

#include "JobsManager/task.h"
#include "common/classad/classad_complement.h"

using std::string;

class BatchTask : public Task {
public:
    // void InitTaskInfo(TaskAdPtr ad, int32_t job_id, int32_t task_id, int32_t submit_time = 0);
    // bool SetTaskStartSuccess();
};
#endif
