#include "JobsManager/job.h"
#include "JobsManager/job_num.h"
#include "JobsManager/task.h"
#include "include/classad_attr.h"
#include "common/classad/classad_complement.h"

using clynn::ReadLocker;
using clynn::WriteLocker;

Job::Job() {
    m_next_task_id = 0;
    m_job_id = 0;
    m_raw_priority = JOB_RAW_PRIO_ORDINARY;
    m_total_running_time = 0; 
    m_total_waiting_time = 0;
}

void Job::SetWaitTime() {
    m_wait_time = time(NULL);
}

void Job::SetRunTime() {
    m_run_time = time(NULL);
}

void Job::UpdateTimeByState(const TaskState& state){
    if (state == TASK_RUNNING) {
        m_total_running_time += (time(NULL) - m_run_time);
    } else if (state == TASK_WAITING){
        m_total_waiting_time += (time(NULL) - m_wait_time);
    }
}

string Job::GetGroupName() {
    return m_group_name;
}

int32_t Job::GetRawPriority() {
    return m_raw_priority;
}

int32_t Job::GetSchedModel() {
    return m_sched_model;
}

int32_t Job::GetState() {
    return m_state;
}

void Job::SetState(int state) {
    m_state = state;
}

int32_t Job::GetJobId(){
    return m_job_id;
}

double Job::GetNeedCpu() {
    return m_need_cpu;
}

int32_t Job::GetNeedMemory() {
    return m_need_memory;
}

void Job::Init(const ClassAdPtr& classad_ptr, bool constraints_among_tasks) {
    WriteLocker locker(m_lock);
    m_job_classad_ptr = ClassAdPtr(new ClassAd(*classad_ptr));
    m_job_id = JobNumI::Instance()->GetNewJobId();
    m_job_classad_ptr->InsertAttr(ATTR_JOB_ID, m_job_id);
    m_job_classad_ptr->EvaluateAttrString(ATTR_GROUP, m_group_name);
    m_job_classad_ptr->EvaluateAttrNumber(ATTR_PRIO, m_raw_priority);
    if (m_raw_priority > 3)
        m_raw_priority = 2;
    m_job_classad_ptr->EvaluateAttrNumber(ATTR_SCHED_MODEL, m_sched_model);
    m_job_classad_ptr->EvaluateAttrNumber(ATTR_NEED_CPU, m_need_cpu);
    m_job_classad_ptr->EvaluateAttrNumber(ATTR_NEED_MEMORY, m_need_memory);
    m_constraints_among_tasks = constraints_among_tasks;
}

void Job::InitTasks(const list<TaskAdPtr>& task_ad_list) {
    WriteLocker locker(m_lock);
    for (list<TaskAdPtr>::const_iterator it = task_ad_list.begin();
         it != task_ad_list.end(); ++it)
    {
            //这里就直接是BatchTask了
            //做一个按Taskg工厂类 TODO
            //TaskPtr task_ptr(new BatchTask());
            TaskPtr task_ptr = NewTask();
            task_ptr->InitTaskInfo(*it, m_job_id, m_next_task_id, m_submit_time);
            m_next_task_id++;
            m_task_list.push_back(task_ptr);
    }
    m_task_num = task_ad_list.size();
    m_job_classad_ptr->InsertAttr(ATTR_TOTAL_TASKS, m_task_num);
}

void Job::InitTasks(const list<TaskAdPtr>& task_ad_list, const list<EdgePtr>& edge_list) {
    WriteLocker locker(m_lock);
    for (list<TaskAdPtr>::const_iterator it = task_ad_list.begin();
         it != task_ad_list.end(); ++it) 
    {
            //TaskPtr task_ptr(new BatchTask());
            TaskPtr task_ptr = NewTask();
            task_ptr->InitTaskInfo(*it, m_job_id, m_next_task_id, m_submit_time);
            m_next_task_id++;
            m_task_list.push_back(task_ptr);
    }
    m_task_num = task_ad_list.size();
    m_job_classad_ptr->InsertAttr(ATTR_TOTAL_TASKS, m_task_num);

    // edge
    for (list<EdgePtr>::const_iterator it = edge_list.begin();
         it != edge_list.end(); ++it)
    {
        // start add
        TaskPtr task1 = GetTaskPtr((*it)->GetStart());
        AdjEdgePtr adj_edge1 = AdjEdgePtr(new AdjEdge((*it)->GetEnd(), (*it)->GetWeight()));
        task1->AdjEdgeListPushBack(adj_edge1);
        // end add   
        TaskPtr task2 = GetTaskPtr((*it)->GetEnd());
        AdjEdgePtr adj_edge2 = AdjEdgePtr(new AdjEdge((*it)->GetStart(), (*it)->GetWeight()));
        task2->AdjEdgeListPushBack(adj_edge2);
    }
}

bool Job::GetConstraintsAmongTasks() {
    return m_constraints_among_tasks;
}

TaskPtr Job::GetTaskPtr(int32_t task_id) {
    ReadLocker locker(m_lock);
    for(list<TaskPtr>::iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it) {
        if((*it)->GetTaskId() == task_id) {
            return *it;
        }
    }
    return TaskPtr();
}

int Job::GetRunTaskNum() {
    ReadLocker locker(m_lock);
    int rc = 0;
    for (list<TaskPtr>::const_iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it) {
            if ((*it)->GetTaskState() == TASK_RUNNING) {
               rc++;
            }
    }
    return rc;
}

int Job::GetTaskNum() {
    ReadLocker locker(m_lock);
    int rc = 0;
    for (list<TaskPtr>::const_iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it) {
            rc++;
    }
    return rc;
}

list<TaskPtr> Job::GetTaskList() {
    ReadLocker locker(m_lock);
    return m_task_list;
}
 
void Job::GetTaskStateInfo(vector<TaskStateInfo>& task_state_info_list) {
    ReadLocker locker(m_lock);
    for(list<TaskPtr>::iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it) {
        TaskStateInfo task_state_info;
        (*it)->GetTaskStateInfo(task_state_info);
	task_state_info_list.push_back(task_state_info);
    }
}

void Job::PrintAllTasks() {
    ReadLocker locker(m_lock);
    printf("-------- Job --------\n");
    string str_classad = ClassAdComplement::AdTostring(m_job_classad_ptr);
    printf("job classad: %s\n", str_classad.c_str());
    for (list<TaskPtr>::iterator it = m_task_list.begin();
         it != m_task_list.end(); ++it) 
    {
         (*it)->PrintClassAds();
    }
    printf("---------------------\n");
}
 
JobQueueNum Job::GetJobQueueNum() {
    return m_job_queue_num;
}

string Job::ReadMap(const int32_t task_id) {
    ReadLocker locker(m_map_lock);
    map<int32_t, string>::iterator it = m_id_to_ip_map.find(task_id);
    if (it != m_id_to_ip_map.end()) {
        return it->second;  
    } else {
        return "";
    } 
}

list<string> Job::ReadMapAndGetSoftList() {
    ReadLocker locker(m_map_lock);
    list<string> wd_soft_list;
    for (map<int32_t, string>::iterator it = m_id_to_ip_map.begin();
         it != m_id_to_ip_map.end(); ++it)
    {
        string soft = "";
        soft = soft + "[ " + ATTR_SOFT_REQUIREMENT + " = " +
               ATTR_Machine_IP + " != \"" + it->second + "\"" +
               "; " +  ATTR_SOFT_VALUE + " = 3.000000000000000E+01 ]";
        wd_soft_list.push_back(soft);
    }

    return wd_soft_list;
}

list<string> Job::ReadMapAndGetSoftList(int32_t task_id) {
    ReadLocker locker(m_map_lock);
    list<string> wd_soft_list;
    for (map<int32_t, string>::iterator it = m_id_to_ip_map.begin();
         it != m_id_to_ip_map.end(); ++it)
    {
        if (task_id == it->first)
            continue;

        string soft = "";
        soft = soft + "[ " + ATTR_SOFT_REQUIREMENT + " = " +
               ATTR_Machine_IP + " == \"" + it->second + "\"" +
               "; " +  ATTR_SOFT_VALUE + " = -3.000000000000000E+01 ]";
        wd_soft_list.push_back(soft);
    }

    return wd_soft_list;
}


void Job::WriteMap(const int32_t task_id, const string& ip ) {
    WriteLocker locker(m_map_lock);
    m_id_to_ip_map.insert(make_pair(task_id, ip));
}

void Job::CheckTasks() {
    ReadLocker locker(m_lock);
    for (list<TaskPtr>::iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it) 
    {
        if ((*it)->IsTimeout()) 
            (*it)->KillforFT();
    }
}

void Job::UpdateState() {
    ReadLocker locker(m_lock);
    bool is_failed = false;
    for (list<TaskPtr>::iterator it = m_task_list.begin();
        it != m_task_list.end(); ++it)
    {
        int32_t state = (*it)->GetTaskState();
        if (state == TASK_STARTING || state == TASK_RUNNING)
            return;
        if (state == TASK_WAITING || state == TASK_MISSED)
            return; 
        if (state == TASK_FAILED) 
            is_failed = true; 
    }
    if (is_failed)
        m_state = JOB_FAILED;
    else 
        m_state = JOB_FINISHED;        
}
