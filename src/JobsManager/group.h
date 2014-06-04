/********************************
 FileName: JobsManager/group.h
 Author:   Zhangzhang & Wangmin
 Date:     2014-03-29
 Version:  0.2
 Description: group: quota & queues
*********************************/

#ifndef JOBS_MANAGER_GROUP_H
#define JOBS_MANAGER_GROUP_H

#include <list>
#include <boost/shared_ptr.hpp>
#include "common/clynn/singleton.h"
#include "common/clynn/rwlock.h"
#include "proxy/JobsManager/gen-cpp/JobsManager.h"
#include "JobsManager/job.h"

using boost::shared_ptr;
using std::list;
using clynn::RWLock;
using std::string;

class Group {
public:
    Group(const string& name);
    string GetGroupName();
    // string GetGroupName(const JobQueueNum& queue_num);
    
    // Add Job -> Queue
    bool AddJobToWaitQueue(const JobPtr& job, bool push_front = false);
    bool GetNewJobWaitQueueNum(const JobPtr& job, JobQueueNum& job_queue_num);
    bool AddJobToQueueByQueueNum(const JobPtr& job, const JobQueueNum& num, bool push_front);

    // Select Job From Queue -> Navigating Job
    bool SelectJobToNavigatingJob();
    bool MoveJobToNavigatingJobByQueueNum(const JobQueueNum& num);

    // get Navigating job
    JobPtr GetNavigatingJob();

    // get Waiting Job Number
    int32_t GetWaitingJobNum();
    // get group state info
    GroupStateInfo GetGroupStateInfo();

    // Move navigating job -> queue
    bool MoveNavigatingJobToWaitQueue(bool push_front = false);
    bool MoveNavigatingJobToRunQueue();
    bool MoveJobToRunQueue(const JobPtr& job_ptr);
    
    // others
    bool CheckJobQueueIsEmptyByQueueNum(const JobQueueNum& num);
    void EmptyNavigatingJob();

    // TODO just for test
    void Print();    

    /* quota */
    // init quota with shares format
    void InitQuotaShares(const int32_t quota, const double max_limit);
    int32_t GetQuotaShares();
    // init quota with percent format
    void InitQuotaPct(const int32_t total_quota);
    // set usage(percent) of dominant resource
    void SetDominantPct();
    bool IsOverLimit();
    // calculate relative ratio
    void SetRelativeRatio();
    double GetRelativeRatio();
    void SetUsedCPU(const double delta_cpu, bool minus = false);
    void SetUsedMemory(const double delta_memory, bool minus = false);

    // get cluster global resource view
    double GetTotalCPU();
    double GetTotalMemory();

    // total resource of cluster 
    static double s_total_cpu;
    static double s_total_memory;

private:
    RWLock m_navigating_job_lock;
    RWLock m_high_wait_job_queue_lock;
    RWLock m_ordinary_wait_job_queue_lock;
    RWLock m_low_wait_job_queue_lock;
    RWLock m_run_job_queue_lock;

private:
    void Init(const string& name);
    string m_group_name;
    //正在匹配的作业 & queues
    JobPtr m_navigating_job;
    list<JobPtr> m_high_wait_job_queue;
    list<JobPtr> m_ordinary_wait_job_queue;
    list<JobPtr> m_low_wait_job_queue;
    list<JobPtr> m_run_job_queue;

    // quota & max limit
    int32_t m_quota_shares;
    // int32_t m_maxlimit_shares;
    double m_quota_pct;
    double m_maxlimit_pct;
    double m_dominant_pct;

    double m_used_cpu; 
    double m_used_memory;
    double m_relative_ratio;
 
    RWLock m_quota_lock;
};

typedef shared_ptr<Group> GroupPtr;
// typedef Singleton<JobGroup> MainJobGroup;

bool SetTotalResource();
#endif
