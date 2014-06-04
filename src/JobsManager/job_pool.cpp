/********************************
 FileName: JobsManager/job_pool.cpp
 Author:   ZhangZhang & WangMin
 Date:     2014-04-08
 Version:  0.1
 Description: pool of jobs, including map, list
*********************************/

#include "JobsManager/job_pool.h"

using clynn::ReadLocker;
using clynn::WriteLocker;
using std::make_pair;

bool JobPool::InsertIfAbsent(const JobPtr& job_ptr) {
    WriteLocker locker(m_map_lock);
    if (FindById(job_ptr->GetJobId()))
        return false;
    Insert(job_ptr);
    return true;    
}

JobPtr JobPool::GetJobPtr(int32_t job_id) {
    ReadLocker locker(m_map_lock);
    map<int, JobPtr>::iterator it = m_id_map.find(job_id);
    if (it != m_id_map.end()) {
        return it->second;
    }
    return JobPtr();
}

bool JobPool::IsExist(int32_t job_id) {
    ReadLocker locker(m_map_lock);
    return FindById(job_id);
}

void JobPool::Insert(const JobPtr& job_ptr) {
    m_id_map.insert(make_pair(job_ptr->GetJobId(), job_ptr));
}

bool JobPool::FindById(int32_t job_id) {
    map<int, JobPtr>::iterator it = m_id_map.find(job_id);
    return it != m_id_map.end();
}

void JobPool::CheckJobs() {
    ReadLocker locker(m_map_lock);
    for (map<int, JobPtr>::iterator it = m_id_map.begin();
         it != m_id_map.end(); ++it)
    {
        int32_t state = (it->second)->GetState();
        if (state == JOB_SCHEDULING || state == JOB_RUNNING)
            (it->second)->CheckTasks();
    }
}

void JobPool::UpdateJobState() {
    ReadLocker locker(m_map_lock);
    for (map<int, JobPtr>::iterator it = m_id_map.begin();
         it != m_id_map.end(); ++it)
    {
        int32_t state = (it->second)->GetState();
        if (state == JOB_RUNNING)
            (it->second)->UpdateState();
    }
}


void JobPool::PrintAll() {
    printf("-------- JobMap --------\n");
    for (map<int, JobPtr>::iterator it = m_id_map.begin();
         it != m_id_map.end(); ++it)
    {
        printf("jod_id: %d\n", it->first);
    }
    printf("------------------------\n");
}

void JobPool::HistoryJobListPushBack(const JobPtr& job_ptr) {
    WriteLocker locker(m_history_lock);
    m_history_job_list.push_back(job_ptr);
}

JobPtr JobPool::HistoryJobListPopFront() {
    WriteLocker locker(m_history_lock);
    JobPtr job_ptr;
    if (m_history_job_list.empty())
        return job_ptr;
    job_ptr = m_history_job_list.front();
    m_history_job_list.pop_front();
    return job_ptr;
}

void JobPool::GetJobIdList(vector<int32_t>& job_id_list) {
    ReadLocker locker(m_map_lock);
    for(map<int, JobPtr>::iterator it = m_id_map.begin(); it != m_id_map.end(); ++it) {
	job_id_list.push_back(it->first);
    }
}

void JobPool::GetTaskStateInfo(vector<TaskStateInfo>& task_state_info_list, int32_t job_id) {
    JobPtr job_ptr = GetJobPtr(job_id);
    job_ptr->GetTaskStateInfo(task_state_info_list);	    
}
