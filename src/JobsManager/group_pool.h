/********************************
 FileName: JobsManager/group_pool.h
 Author:   WangMin
 Date:     2014-03-29
 Version:  0.1
 Description: Pool of Groups
*********************************/

#ifndef JOBS_MANAGER_GROUP_POOL_H
#define JOBS_MANAGER_GROUP_POOL_H

#include <map>

#include "common/clynn/rwlock.h"
#include "common/clynn/singleton.h"
#include "JobsManager/group.h"

using std::map;
using clynn::RWLock;

class GroupPool {
public:
    // TODO print all for test
    void PrintAll();

    // quota
    bool InitGroups(const string& conf);

    void SetTotalQuotaShares();

    void UpdateGroupsQuotaPct();

    bool IsExistByName(const string& name);

    // @brief: insert a group in map
    bool InsertIfAbsent(const GroupPtr& ptr);

    // @brief: delete a group from map by name 
    void Delete(const string& name);

    GroupPtr GetGroupPtr(const string& name);

    // add job to wait queue
    bool AddJobToWaitQueue(const JobPtr& job);

    // select job to navigating job
    bool SelectJobToNavigatingJob();

    // Set used resource
    bool AddUsedResource();
    bool AddUsedResource(const TaskPtr& task_ptr);
    bool ReleaseUsedResource(const TaskPtr& task_ptr);

    // Move navigating job -> queue
    bool MoveNavigatingJobToWaitQueue(bool push_front = false);
    bool MoveNavigatingJobToRunQueue();

    JobPtr GetNavigatingJob();
    void EmptyNavigatingJob();

    int32_t GetWaitingJobNumByGroup(const string& group);
    GroupStateInfo GetGroupStateInfo(const string& group);

private:
    // @brief: find a group in map
    bool Find(const GroupPtr& ptr);
    bool FindByName(const string& name);
    // @brief: insert a group in map
    void Insert(const GroupPtr& ptr);

private:
    // add job to wait queue
    bool AddJobToPrivilegeQueue(const JobPtr& job, bool push_front = false);
    bool AddJobToWaitQueueByGroup(const JobPtr& job);

    // select job to navigating job
    bool MoveJobToNavigatingJobFromPrivilege();
    bool MoveJobToNavigatingJobFromGroup(const string& name);

    // move navigating job -> queue
    bool MoveNavigatingJobToPrivilegeQueue(bool push_front = false);
    bool MoveNavigatingJobToGroupWaitQueue(const string& name, bool push_front = false);

    // sort 
    static bool GroupCompare(const GroupPtr first, const GroupPtr second) {
        return first->GetRelativeRatio() <= second->GetRelativeRatio();
    }

private:
    // <group_name, group ptr>
    map<string, GroupPtr> m_group_map;
    int32_t m_total_quota;
    double m_privilege_factor;
    list<JobPtr> m_privilege_wait_job_queue;
    list<GroupPtr> m_group_sort_list;
    JobPtr m_navigating_job;

    RWLock m_map_lock;
    RWLock m_privilege_job_queue_lock;
    RWLock m_group_sort_lock;
    RWLock m_navigating_job_lock;
};

typedef Singleton<GroupPool> GroupPoolI;

#endif
