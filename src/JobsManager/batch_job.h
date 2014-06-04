#ifndef JOBS_MANAGER_BATCH_JOB_H
#define JOBS_MANAGER_BATCH_JOB_H

#include "JobsManager/job.h"

class BatchJob : public Job {
public:
    bool HaveWaitingTasks();
    bool IsScheduling();
    bool IsRunning();
private:
    virtual TaskPtr NewTask() const;
};
#endif
