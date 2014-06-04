/********************************
 FileName: JobsManager/task.h
 Author:   ZhangZhang & WangMin
 Date:     2014-04-10
 Version:  0.1
 Description: task, include ClassAd m_taskad_hard_constraint and list<ClassAd> m_soft_constraint_list
*********************************/

#ifndef JOBS_MANAGER_TASK_H
#define JOBS_MANAGER_TASK_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "common/classad/classad_complement.h"
#include "common/clynn/rwlock.h"
#include "include/proxy.h"
#include "JobsManager/job_enum.h"

using std::list;
using boost::shared_ptr;
using boost::enable_shared_from_this;
using clynn::RWLock;

class TaskAd {
public:
    TaskAd() {
        taskad_hard_constraint = ClassAdPtr(new ClassAd());
    }

    // deep copy
    TaskAd(const TaskAd& ads) {
        // hard constraint
        taskad_hard_constraint = ClassAdPtr(new ClassAd(*ads.taskad_hard_constraint));
        // soft constraint list
        for (list<ClassAdPtr>::const_iterator it = ads.soft_constraint_list.begin();  
             it != ads.soft_constraint_list.end(); ++it) 
        {
            ClassAdPtr soft_ad = ClassAdPtr(new ClassAd(*(*it)));
            soft_constraint_list.push_back(soft_ad);  
        } 
    }

    ClassAdPtr taskad_hard_constraint;  // 任务属性，包含硬约束需求
    list<ClassAdPtr> soft_constraint_list; // 软约束列表
};

typedef shared_ptr<TaskAd> TaskAdPtr;

class AdjEdge {
public:
    AdjEdge(int32_t e, int32_t w) {
        end = e;
        weight = w;
    }
    int32_t end;
    int32_t weight;
}; 

typedef shared_ptr<AdjEdge> AdjEdgePtr;

class Task : public enable_shared_from_this<Task> {
public:
    virtual void InitTaskInfo(TaskAdPtr ad, int32_t job_id, int32_t task_id, int32_t submit_time = 0);
    void SetTaskHardClassAd();

    TaskState GetTaskState();
    void SetTaskState(int32_t state);
    //void GetTaskStateInfo(const TaskStateInfo& task_state_info);
    bool StartTask(const string& endpoint, const string& taskad);
    void TaskStarting();
    void TaskStarted();
    void TaskFinished();
    void TaskFailed();
    void TaskTimeout();
    void TaskMissed();
    void RestartTask();
    bool IsTimeout();
    bool KillforFT();
    int32_t GetTaskId();
    int32_t GetJobId();
    string GetGroupName();
    int32_t GetRawPriority();
    int32_t GetSchedModel();
    double GetNeedCpu();
    int32_t GetNeedMemory();

    ClassAdPtr GetTaskHardClassAd();
    list<ClassAdPtr> GetTaskSoftList();

    void AdjEdgeListPushBack(const AdjEdgePtr& edge); 

    void GetTaskStateInfo(TaskStateInfo& task_state_info);

    void SetEsIp(const string& es_ip);

    // TODO test
    void PrintClassAds();

protected:
    ClassAdPtr m_taskad_hard_constraint;      // 任务属性，包含硬约束需求
    list<ClassAdPtr> m_soft_constraint_list;  // 软约束列表
    TaskState m_task_state;
    RWLock m_lock; 

    list<AdjEdgePtr> m_adj_edge_list;
    int m_job_id;
    int m_task_id;
    time_t m_start_time;
    int32_t m_timeout;
    int32_t m_restart_times;
    int32_t m_failed_times;   
    int32_t m_timeout_times;
    int32_t m_missed_times;
    static int32_t s_max_times;
    string m_es_ip;

    string m_group_name;
    int32_t m_raw_priority;
    int32_t m_sched_model;
    double m_need_cpu;
    int32_t m_need_memory;   
};

typedef shared_ptr<Task> TaskPtr;
#endif
