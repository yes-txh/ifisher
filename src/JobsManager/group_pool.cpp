/********************************
 FileName: JobsManager/group_pool.cpp
 Author:   WangMin
 Date:     2014-03-29
 Version:  0.1
 Description: Pool of Groups
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "common/rapidxml/rapidxml.hpp"
#include "common/rapidxml/rapidxml_utils.hpp"
#include "common/rapidxml/rapidxml_print.hpp"
#include "JobsManager/group_pool.h"

using std::vector;
using log4cplus::Logger;
using clynn::ReadLocker;
using clynn::WriteLocker;
using namespace rapidxml;

static Logger logger = Logger::getInstance("JobsManager");

// rapidxml过程中，不能操作存储其他指针，虽然看上去二者毫无联系。为了避免奇怪的bug
struct sGroup {
    string name;
    int32_t quota;
    double maxlimit; 
};

// TODO just for test
void GroupPool::PrintAll() {
    ReadLocker locker(m_map_lock);
    printf("job_group_pool.cpp\n");
    printf("************ Groups ************\n");
    printf("privilege factor: %f\n", m_privilege_factor);
    for (map<string, GroupPtr>::iterator it = m_group_map.begin();
         it != m_group_map.end(); ++it) {
        (it->second)->Print();
    }
    printf("********************************\n");
}

bool GroupPool::InitGroups(const string& conf) {
    // init xml file
    xml_document<> doc;
    try {
        file<> fdoc  (conf.c_str());
        doc.parse<0>(fdoc.data());
    } catch (parse_error& ex) {
        LOG4CPLUS_ERROR(logger, "synex error in " << ex.what() << ".");
        return false;
    } catch (std::runtime_error& ex) {
        LOG4CPLUS_ERROR(logger, "task xml error:" << ex.what() << ".");
        return false;
    }
 
    // xml_root groups
    xml_node<> *root_node = doc.first_node("groups");
    if (!root_node) {
        LOG4CPLUS_ERROR(logger, "no groups specified.");
        return false;
    }

    xml_node<char> *factor_node = root_node->first_node("privilege_factor");
    if (!factor_node) {
        LOG4CPLUS_ERROR(logger, "Failed to parse group privilege_factor ");
        return false;
    }
    m_privilege_factor = atof(factor_node->value()); // illegal -> 0
    if (m_privilege_factor <= 1) {
         LOG4CPLUS_ERROR(logger, "Privilege_factor is illegal.");
         m_privilege_factor = 3;
    }

    // traversal(bian li) all groups
    vector<sGroup> tmp_groups;
    for (xml_node<char> *group_node = root_node->first_node("group");
        group_node != NULL;  
        group_node = group_node->next_sibling())  
    { 
        // parse xml doc
        xml_node<char> *name_node, *quota_node, *maxlimit_node;
        string group_name;
        int32_t quota = -1;
        double maxlimit = -1;

        name_node = group_node->first_node("name");
        if (!name_node) {
            LOG4CPLUS_ERROR(logger, "Failed to parse group name");
            return false;
        }
        group_name = name_node->value();

        quota_node = group_node->first_node("quota_shares");
        if (!quota_node) {
            LOG4CPLUS_ERROR(logger, "Failed to parse group quota_shares");
            return false;
        }
        quota = atoi(quota_node->value()); // illegal -> 0

        maxlimit_node = group_node->first_node("max_limit");
        if (!maxlimit_node) {
            LOG4CPLUS_ERROR(logger, "Failed to parse group max_limit");
            maxlimit = 1;
        } else {
            maxlimit = atof(maxlimit_node->value());
            if (maxlimit >1 || maxlimit <= 0)
                maxlimit = 1;
        }
         
        // define a struct sGroup, tmp store it
        sGroup tmp;
        tmp.name = group_name;
        tmp.quota = quota;
        tmp.maxlimit = maxlimit;
        tmp_groups.push_back(tmp);
    }      

    // insert groups into GroupPool
    m_total_quota = 0; 
    vector<sGroup>::iterator it = tmp_groups.begin();
    for (; it != tmp_groups.end(); ++it) {
        // new group
        GroupPtr ptr(new Group(it->name));
        ptr->InitQuotaShares(it->quota, it->maxlimit);
        m_total_quota += it->quota;
        // insert group into GroupPool
        if (!InsertIfAbsent(ptr)) {
            LOG4CPLUS_ERROR(logger, "The group had exist in the config file, group name:" << ptr->GetGroupName());
            return false;
        }
    } 
 
    UpdateGroupsQuotaPct(); 
 
    return true;
}

// calculate m_total_quota
void GroupPool::SetTotalQuotaShares() {
    WriteLocker locker(m_map_lock);
    m_total_quota = 0;
    map<string, GroupPtr>::iterator it = m_group_map.begin();
    for (; it != m_group_map.end(); ++it ) {
        m_total_quota += (it->second)->GetQuotaShares();
    }
}

// update m_quota_pct of every group
void GroupPool::UpdateGroupsQuotaPct() {
    ReadLocker locker(m_map_lock); // GroupPool is reading; and Group is writing.
    map<string, GroupPtr>::iterator it = m_group_map.begin();
    for (; it != m_group_map.end(); ++it) {
        (it->second)->InitQuotaPct(m_total_quota);
    }
}

bool GroupPool::Find(const GroupPtr& ptr) {
    ReadLocker locker(m_map_lock);
    return FindByName(ptr->GetGroupName());
}

bool GroupPool::IsExistByName(const string& name) {
    ReadLocker locker(m_map_lock);
    return FindByName(name);
}

bool GroupPool::InsertIfAbsent(const GroupPtr& ptr) {
    WriteLocker locker(m_map_lock);
    if (FindByName(ptr->GetGroupName()))
        return false;
    Insert(ptr);
    return true;
}

bool GroupPool::FindByName(const string& name) {
    map<string, GroupPtr>::iterator it = m_group_map.find(name);
    return it != m_group_map.end();
}

void GroupPool::Insert(const GroupPtr& ptr) {
    m_group_map[ptr->GetGroupName()] = ptr;
}

void GroupPool::Delete(const string& name) {
    WriteLocker locker(m_map_lock);
    // erase() will invoke destructor(xi gou) func
    m_group_map.erase(name);
}

GroupPtr GroupPool::GetGroupPtr(const string& name) {
    ReadLocker locker(m_map_lock);
    map<string, GroupPtr>::iterator it = m_group_map.find(name);
    if (it != m_group_map.end()) {
        return it->second;
    }
    // not find then return NULL
    return GroupPtr();
}

// add job to wait queue
bool GroupPool::AddJobToWaitQueue(const JobPtr& job) {
    int priority = job->GetRawPriority();
    if (JOB_RAW_PRIO_PRIVILEGE == priority) {
        if (!AddJobToPrivilegeQueue(job)) {
            LOG4CPLUS_ERROR(logger, "Failed to AddJobToPrivilegeQueue.");
            return false;
        }
    } else {
        if (!AddJobToWaitQueueByGroup(job)) {
            // test
            printf("No this group, %s\n", job->GetGroupName().c_str());
            LOG4CPLUS_ERROR(logger, "No this group, group name: " << job->GetGroupName()); 
            return false; 
        }         
    }
    return true;
}

bool GroupPool::AddJobToPrivilegeQueue(const JobPtr& job, bool push_front) {
    WriteLocker locker(m_privilege_job_queue_lock);
    // TODO test
    if (!FindByName(job->GetGroupName())) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << job->GetGroupName()); 
        return false;
    }

    printf("privilege!\n");
    if (push_front)
        m_privilege_wait_job_queue.push_front(job);
    else
        m_privilege_wait_job_queue.push_back(job);
    return true;
}

bool GroupPool::AddJobToWaitQueueByGroup(const JobPtr& job) {
    string group_name = job->GetGroupName();
    GroupPtr group_ptr = GetGroupPtr(group_name);
    if (NULL == group_ptr) {
        return false;
    }
    
    // group_ptr->AddJobToWaitQueueAutoLock(job);
    group_ptr->AddJobToWaitQueue(job);
    return true;
}

// select job to navigating job
bool GroupPool::SelectJobToNavigatingJob() {
    if (MoveJobToNavigatingJobFromPrivilege()) {
        return true;
    }

    // update total resource
    if (!SetTotalResource()) {
        LOG4CPLUS_ERROR(logger, "Failed to set Total Resource.");
        return false;
    }

    // filter GroupPtr by MaxLimit
    m_group_sort_list.clear();
    
    m_map_lock.ReadLock();
    map<string, GroupPtr>::iterator it = m_group_map.begin();
    for (; it != m_group_map.end(); ++it ) {
        GroupPtr group_ptr = it->second;
        group_ptr->SetDominantPct();
        // not over limit
        if (!group_ptr->IsOverLimit()) {
            group_ptr->SetRelativeRatio();
            m_group_sort_list.push_back(group_ptr);
        }
    }
    m_map_lock.Unlock();

    // sort GroupPtr by RelativeRatio
    m_group_sort_list.sort(GroupCompare);

    list<GroupPtr>::iterator it2 = m_group_sort_list.begin();
    for (; it2 != m_group_sort_list.end(); ++it2 ) {
        GroupPtr group_ptr = *it2;
        if (MoveJobToNavigatingJobFromGroup(group_ptr->GetGroupName())) {
            return true;
        }
    }

    return false; 
}

bool GroupPool::MoveJobToNavigatingJobFromPrivilege() {
    if (m_privilege_wait_job_queue.empty()) {
        return false;
    }

    // set total resource
    if (!SetTotalResource()) {
        LOG4CPLUS_ERROR(logger, "Failed to set Total Resource.");
        return false;
    }

    bool ret = false;
    WriteLocker navi_locker(m_navigating_job_lock);
    WriteLocker locker(m_privilege_job_queue_lock);
    // select one job from privilege queue
    for (list<JobPtr>::iterator it = m_privilege_wait_job_queue.begin();
         it != m_privilege_wait_job_queue.end(); ) {
        JobPtr job_ptr = *it;
        GroupPtr group_ptr = GetGroupPtr(job_ptr->GetGroupName());
        group_ptr->SetDominantPct();
        // check 
        if (!group_ptr->IsOverLimit()) {
            // is not over limit, this job
            m_navigating_job = job_ptr;
            // delete job from queue 
            it = m_privilege_wait_job_queue.erase(it);
            ret = true;
            break;
        } else {
            // over limit, next one
            it++; 
        }
         
    }

    return ret;
}

bool GroupPool::MoveJobToNavigatingJobFromGroup(const string& name) {
    GroupPtr group_ptr = GetGroupPtr(name);
    if (!group_ptr) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << name);         
        return false;
    }
  
    if (!group_ptr->SelectJobToNavigatingJob()) {
        return false;
    }
    WriteLocker navi_locker(m_navigating_job_lock);
    m_navigating_job = group_ptr->GetNavigatingJob();
    return true;
}

// Move navigating job -> queue
bool GroupPool::MoveNavigatingJobToWaitQueue(bool push_front) {
    int priority = m_navigating_job->GetRawPriority();
    if (JOB_RAW_PRIO_PRIVILEGE == priority) {
        if (!MoveNavigatingJobToPrivilegeQueue(push_front)) {
            LOG4CPLUS_ERROR(logger, "Failed to MoveNavigatingJobToPrivilegeQueue");
            return false;
        }
    } else {
        if (!MoveNavigatingJobToGroupWaitQueue(m_navigating_job->GetGroupName(), push_front)) {
            LOG4CPLUS_ERROR(logger, "No this group, group name: " << m_navigating_job->GetGroupName());
            return false;
        }
    }
    return true; 
}

bool GroupPool::MoveNavigatingJobToPrivilegeQueue(bool push_front) {
    WriteLocker locker(m_navigating_job_lock);
    if (!m_navigating_job) {
        return false;
    }

    AddJobToPrivilegeQueue(m_navigating_job, push_front);
    m_navigating_job = JobPtr();
    return true;
}

bool GroupPool::MoveNavigatingJobToGroupWaitQueue(const string& name, 
                                                     bool push_front) {
    GroupPtr group_ptr = GetGroupPtr(name);
    if (!group_ptr) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << name);
        return false;
    }

    group_ptr->MoveNavigatingJobToWaitQueue(push_front);
   
    WriteLocker locker(m_navigating_job_lock);
    if (!m_navigating_job) {
        return false;
    } 
    m_navigating_job = JobPtr();
    return true;
}

bool GroupPool::MoveNavigatingJobToRunQueue() {
    WriteLocker locker(m_navigating_job_lock);
    JobPtr job_ptr = m_navigating_job;
    GroupPtr group_ptr = GetGroupPtr(job_ptr->GetGroupName());        
    if (!group_ptr) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << job_ptr->GetGroupName());
        return false;
    }

    int priority = m_navigating_job->GetRawPriority();
    if (JOB_RAW_PRIO_PRIVILEGE == priority) {
        if (!group_ptr->MoveJobToRunQueue(job_ptr)) {
            LOG4CPLUS_ERROR(logger, "Failed to MoveJobToRunQueue");
            return false;
        }
        m_navigating_job = JobPtr();
        return true;
    } else {
        if (!group_ptr->MoveNavigatingJobToRunQueue()) {
            LOG4CPLUS_ERROR(logger, "MoveNavigatingJobToRunQueue");
            return false;
        }
        m_navigating_job = JobPtr();
        return true;
    }
    return false;
}

JobPtr GroupPool::GetNavigatingJob() {
    ReadLocker locker(m_navigating_job_lock);
    return m_navigating_job;
}

void GroupPool::EmptyNavigatingJob() {
    WriteLocker locker(m_navigating_job_lock);
    m_navigating_job = JobPtr();
}

int32_t GroupPool::GetWaitingJobNumByGroup(const string& group) {
    GroupPtr group_ptr = GetGroupPtr(group);
    if (!group_ptr) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << group);
        return -1;
    }
    return group_ptr->GetWaitingJobNum();
}

GroupStateInfo GroupPool::GetGroupStateInfo(const string& group) {
    GroupPtr group_ptr = GetGroupPtr(group);
    if (!group_ptr) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << group);
        return GroupStateInfo();
    }
    return group_ptr->GetGroupStateInfo();
}


// add m_used_cpu and m_used_memory of group
bool GroupPool::AddUsedResource() {
    GroupPtr group_ptr = GetGroupPtr(m_navigating_job->GetGroupName());
    if (!group_ptr) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << m_navigating_job->GetGroupName());
        return false;
    }

    double factor = 1.0;
    int32_t priority = m_navigating_job->GetRawPriority();
    if (JOB_RAW_PRIO_PRIVILEGE == priority) {
        factor = m_privilege_factor;
    }

    group_ptr->SetUsedCPU(m_navigating_job->GetNeedCpu() * factor);
    group_ptr->SetUsedMemory(m_navigating_job->GetNeedMemory() * factor);
    return true;
}

// add m_used_cpu and m_used_memory of group
bool GroupPool::AddUsedResource(const TaskPtr& task_ptr) {
    if (task_ptr->GetTaskState() != TASK_WAITING) {
        // restart
        return true;    
    }

    GroupPtr group_ptr = GetGroupPtr(task_ptr->GetGroupName());
    if (!group_ptr) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << task_ptr->GetGroupName());
        return false;
    }

    double factor = 1.0;
    int32_t priority = task_ptr->GetRawPriority();
    if (JOB_RAW_PRIO_PRIVILEGE == priority) {
        factor = m_privilege_factor;
    }
    
    group_ptr->SetUsedCPU(task_ptr->GetNeedCpu() * factor);
    group_ptr->SetUsedMemory(task_ptr->GetNeedMemory() * factor);
    return true;
}

// release m_used_cpu and m_used_memory of group
bool GroupPool::ReleaseUsedResource(const TaskPtr& task_ptr) {
    GroupPtr group_ptr = GetGroupPtr(task_ptr->GetGroupName());
    if (!group_ptr) {
        LOG4CPLUS_ERROR(logger, "No this group, group name: " << task_ptr->GetGroupName());
        return false;
    }

    double factor = 1.0;
    int32_t priority = task_ptr->GetRawPriority();
    if (JOB_RAW_PRIO_PRIVILEGE == priority) {
        factor = m_privilege_factor;
    }

    group_ptr->SetUsedCPU(task_ptr->GetNeedCpu() * factor, true);
    group_ptr->SetUsedMemory(task_ptr->GetNeedMemory() * factor, true);
    return true;
}
