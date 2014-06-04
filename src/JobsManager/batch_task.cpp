#include "include/classad_attr.h"
#include "JobsManager/batch_task.h"

using clynn::ReadLocker;
using clynn::WriteLocker;

/* void BatchTask::InitTaskInfo(TaskAdPtr ads, int32_t job_id,
                             int32_t task_id, int32_t submit_time) {

} */

/* bool BatchTask::SetTaskStartSuccess() {
    WriteLocker locker(m_lock);
    if(m_task_state != TASK_STARTING && 
       m_task_state != TASK_WAITING) {
       return false;
    }   
    m_task_state = TASK_RUNNING;
    m_taskad_hard_constraint->InsertAttr(ATTR_TASK_STATUS, m_task_state);
    m_run_time = time(0);
    m_taskad_hard_constraint->InsertAttr(ATTR_TASK_START_TIME, static_cast<int>(m_run_time));
    return true; 
} */
