#include "JobsManager/batch_job.h"
#include "JobsManager/batch_task.h"

//下面的有些函数在调用时候注意上锁

TaskPtr BatchJob::NewTask() const {
     return TaskPtr(new BatchTask());
}

bool BatchJob::HaveWaitingTasks() {
    for(list<TaskPtr>::const_iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it) {
        if((*it)->GetTaskState() == TASK_WAITING) {
            return true;
        }
    }
    return false;
}

// @brief:是否处于下发状态
bool BatchJob::IsScheduling() {
    bool is_all_scheduling = false;
    bool have_scheduling_task = false;
    
    for(list<TaskPtr>::const_iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it) {
        if((*it)->GetTaskState() == TASK_WAITING) {
            have_scheduling_task = true;
        }
        if ((*it)->GetTaskState() != TASK_STARTING &&
            (*it)->GetTaskState() != TASK_RUNNING &&
            (*it)->GetTaskState() != TASK_FINISHED &&
            (*it)->GetTaskState() != TASK_FAILED) {
            is_all_scheduling = false;
            break;
        } 
    }
    return is_all_scheduling && have_scheduling_task; 
}

bool BatchJob::IsRunning() {
    bool is_all_running = false; 
    bool have_run_task = false;
    for(list<TaskPtr>::const_iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it) {
        if((*it)->GetTaskState() == TASK_RUNNING) {
            have_run_task = true;
        } 
        if((*it)->GetTaskState() != TASK_RUNNING && 
           (*it)->GetTaskState() != TASK_FINISHED &&
           (*it)->GetTaskState() != TASK_FAILED) {
           is_all_running = false;
           break;
        }
    }
    return is_all_running && have_run_task;
}
