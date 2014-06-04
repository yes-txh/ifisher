/********************************
 FileName: JobsManager/task.cpp
 Author:   ZhangZhang & WangMin
 Date:     2014-04-10
 Version:  0.1
 Description: task, include ClassAd m_taskad_hard_constraint and list<ClassAd> m_soft_constraint_list
*********************************/

#include "JobsManager/task.h"

using clynn::ReadLocker;
using clynn::WriteLocker;

TaskStatus Task::GetTaskStatus() {
    ReadLocker locker(lock);
    return m_task_status;
}

void Task::SetTaskStatus(int32_t status) {
    WriteLocker locker(lock);
    m_task_status = TaskStatus(status);
}

int Task::GetTaskId() {
    return m_task_id;
}

int Task::GetJobId() {
    return m_job_id;
}

void Task::PrintClassAds() {
    // hard constraint
    string str_hard_classad = ClassAdComplement::AdTostring(m_taskad_hard_constraint);
    printf("task hard classad: %s\n", str_hard_classad.c_str());

    // soft constraint list
    for (list<ClassAdPtr>::iterator it = m_soft_constraint_list.begin();
         it != m_soft_constraint_list.end(); ++it)
    {
        string str_soft_classad = ClassAdComplement::AdTostring(*it);
        printf("task soft classad: %s\n", str_soft_classad.c_str());
    }
 
}

ClassAdPtr Task::GetTaskHardClassAd() {
    return m_taskad_hard_constraint;
}

list<ClassAdPtr> Task::GetTaskSoftList() {
    return m_soft_constraint_list;
}

