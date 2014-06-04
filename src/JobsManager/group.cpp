/********************************
FileName: JobsManager/group.cpp
Author: Zhangzhang & Wangmin
Date: 2014-03-29
Version: 0.2
Description: group: quota & queues
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "proxy/ResourceScheduler/gen-cpp/ResourceScheduler.h"
#include "common/clynn/rpc.h"
#include "JobsManager/group.h"

using log4cplus::Logger;
using clynn::WriteLocker;
using clynn::ReadLocker;

static Logger logger = Logger::getInstance("JobsManager");

// gflag
DECLARE_string(resource_scheduler_endpoint);

// static data member
double Group::s_total_cpu = 1; // 防止为0
double Group::s_total_memory = 1;

Group::Group(const string& name) {
    Init(name);
}

void Group::Init(const string& name) {
    m_group_name = name;
}

string Group::GetGroupName() {
    return m_group_name;
}

// Add Job to Wait Queue
bool Group::AddJobToWaitQueue(const JobPtr& job, bool push_front) {
    JobQueueNum job_queue_num;
    if (!GetNewJobWaitQueueNum(job, job_queue_num)) {
        LOG4CPLUS_ERROR(logger, "No this priority.");
        return false;
    }

    if (!AddJobToQueueByQueueNum(job, job_queue_num, push_front)) {
        LOG4CPLUS_ERROR(logger, "No this queue num.");
        return false;
    }
    return true;
}

// Add Job to Wait Queue Bu Queue Num
bool Group::AddJobToQueueByQueueNum(const JobPtr& job, const JobQueueNum& num, bool push_front){
    switch (num) {
        case JOB_QUEUE_HIGH_WAIT:
            if (push_front) {
                WriteLocker locker(m_high_wait_job_queue_lock);
                m_high_wait_job_queue.push_front(job);
            } else {
                WriteLocker locker(m_high_wait_job_queue_lock);
                m_high_wait_job_queue.push_back(job);
                // TODO test
                printf("%s, High\n", GetGroupName().c_str());
            }
            return true;
        case JOB_QUEUE_ORDINARY_WAIT:
            if (push_front) {
                WriteLocker locker(m_ordinary_wait_job_queue_lock);
                m_ordinary_wait_job_queue.push_front(job);
            } else {
                WriteLocker locker(m_ordinary_wait_job_queue_lock);
                m_ordinary_wait_job_queue.push_back(job);
                printf("%s, Ordinary\n", GetGroupName().c_str());
            }
            return true;
        case JOB_QUEUE_LOW_WAIT:
            if (push_front) {
                WriteLocker locker(m_low_wait_job_queue_lock);
                m_low_wait_job_queue.push_front(job);
            } else {
                WriteLocker locker(m_low_wait_job_queue_lock);
                m_low_wait_job_queue.push_back(job);
                printf("%s, Low\n", GetGroupName().c_str());
            }
            return true;
        default:
            return false;
    }
}

// Get Job Queue Num -> queue_num
bool Group::GetNewJobWaitQueueNum(const JobPtr& job, JobQueueNum& queue_num) {
    int priority = job->GetRawPriority();
    if (priority == JOB_RAW_PRIO_HIGH) {
        queue_num = JOB_QUEUE_HIGH_WAIT;
        return true;
    } else if (priority == JOB_RAW_PRIO_ORDINARY) {
        queue_num = JOB_QUEUE_ORDINARY_WAIT;
        return true;
    } else if (priority == JOB_RAW_PRIO_LOW) {
        queue_num = JOB_QUEUE_LOW_WAIT;
        return true;
    }
    return false;
}

// select a job to navigating job
bool Group::SelectJobToNavigatingJob() {
    if (MoveJobToNavigatingJobByQueueNum(JOB_QUEUE_HIGH_WAIT)) {
        return true;
    }
    if (MoveJobToNavigatingJobByQueueNum(JOB_QUEUE_ORDINARY_WAIT)) {
        return true;
    }
    if (MoveJobToNavigatingJobByQueueNum(JOB_QUEUE_LOW_WAIT)) {
        return true;
    }
    return false;
}

// get first job -> navigating job, then delete the job from queue
bool Group::MoveJobToNavigatingJobByQueueNum(const JobQueueNum& num) {
    switch (num) {
        case JOB_QUEUE_HIGH_WAIT:
           if (m_high_wait_job_queue.empty()) {
               return false;
           } else {
               WriteLocker navi_locker(m_navigating_job_lock);
               WriteLocker locker(m_high_wait_job_queue_lock);
               // set navigating job
               m_navigating_job = *m_high_wait_job_queue.begin();
               // delete job from queue
               m_high_wait_job_queue.pop_front();
               return true;
           }
           break;
        case JOB_QUEUE_ORDINARY_WAIT:
           if (m_ordinary_wait_job_queue.empty()) {
               return false;
           } else {
               WriteLocker navi_locker(m_navigating_job_lock);
               WriteLocker locker(m_ordinary_wait_job_queue_lock);
               // set navigating job
               m_navigating_job = *m_ordinary_wait_job_queue.begin();
               // delete job from queue
               m_ordinary_wait_job_queue.pop_front();
                return true;
           }
           break;
        case JOB_QUEUE_LOW_WAIT:
           if (m_low_wait_job_queue.empty()) {
               return false;
           } else {
               WriteLocker navi_locker(m_navigating_job_lock);
               WriteLocker locker(m_low_wait_job_queue_lock);
               // set navigating job
               m_navigating_job = *m_low_wait_job_queue.begin();
               // delete job from queue
               m_low_wait_job_queue.pop_front();
               return true;
           }
           break;
        default:
           break;
    }
    return false;
}

bool Group::CheckJobQueueIsEmptyByQueueNum(const JobQueueNum& num) {
    bool is_empty = true;
    switch (num) {
        case JOB_QUEUE_HIGH_WAIT:
            if(!m_high_wait_job_queue.empty()) {
                is_empty = false;
            }
            break;
        case JOB_QUEUE_ORDINARY_WAIT:
            if(!m_ordinary_wait_job_queue.empty()) {
                is_empty = false;
            }
            break;
        case JOB_QUEUE_LOW_WAIT:
            if(!m_low_wait_job_queue.empty()) {
                is_empty = false;
            }
            break;
        default:
            is_empty = true;
            break;
    }
    return is_empty;
}

JobPtr Group::GetNavigatingJob() {
    ReadLocker locker(m_navigating_job_lock);
    return m_navigating_job;
}

void Group::EmptyNavigatingJob() {
    WriteLocker locker(m_navigating_job_lock);
    m_navigating_job = JobPtr();
}

int32_t Group::GetWaitingJobNum() {
    ReadLocker locker_high(m_high_wait_job_queue_lock);
    ReadLocker locker_ordinary(m_ordinary_wait_job_queue_lock);
    ReadLocker locker_low(m_low_wait_job_queue_lock);
    return m_high_wait_job_queue.size() + m_ordinary_wait_job_queue.size() + m_low_wait_job_queue.size();
}

GroupStateInfo Group::GetGroupStateInfo() {
    GroupStateInfo group_info;
    group_info.name = m_group_name;
    group_info.quota_shares = m_quota_shares;
    group_info.quota_pct = m_quota_pct;
    group_info.maxlimit_pct = m_maxlimit_pct;
    group_info.used_cpu = m_used_cpu;
    group_info.used_memory = m_used_memory;
    group_info.total_cpu = s_total_cpu;
    group_info.total_memory = s_total_memory;
    return group_info;
}


bool Group::MoveNavigatingJobToWaitQueue(bool push_front) {
    WriteLocker locker(m_navigating_job_lock);
    if (!m_navigating_job) {
        return false;
    }

    AddJobToWaitQueue(m_navigating_job, push_front);
    m_navigating_job = JobPtr();
    return true;
}

bool Group::MoveNavigatingJobToRunQueue() {
    WriteLocker locker(m_navigating_job_lock);
    if (!m_navigating_job) {
        return false;
    }
    WriteLocker lock(m_run_job_queue_lock);
    m_run_job_queue.push_back(m_navigating_job);
    m_navigating_job = JobPtr();
    printf("%s, RunQueue\n", GetGroupName().c_str());
    return true;
}

bool Group::MoveJobToRunQueue(const JobPtr& job_ptr) {
    if (!job_ptr) {
        return false;
    }
    WriteLocker lock(m_run_job_queue_lock);
    m_run_job_queue.push_back(job_ptr);
    printf("%s, RunQueue\n", GetGroupName().c_str());
    return true;
}

void Group::Print() {
    ReadLocker lock(m_quota_lock);
    printf("Group:\n");
    printf("--name: %s\n", m_group_name.c_str());
    printf("--m_quota_shares: %d\n", m_quota_shares);
    printf("--m_quota_pct: %f\n", m_quota_pct);
    printf("--m_maxlimit_pct: %f\n", m_maxlimit_pct);
    printf("--m_dominant_pct: %f\n", m_dominant_pct);
    printf("--m_used_cpu: %f\n", m_used_cpu);
    printf("--m_used_memory: %f\n", m_used_memory);
}

/* quota */
// init quota with shares format
void Group::InitQuotaShares(const int32_t quota, const double max_limit) {
    WriteLocker lock(m_quota_lock);
    m_quota_shares = quota;
    m_maxlimit_pct = max_limit;
   
    m_quota_pct = 0.0;
    m_used_cpu = 0.0;
    m_used_memory = 0.0;
    m_dominant_pct = 0.0;

    return;
}

int32_t Group::GetQuotaShares() {
    ReadLocker lock(m_quota_lock);
    return m_quota_shares;
}

// init quota with percent format
void Group::InitQuotaPct(const int32_t total_quota) {
    WriteLocker lock(m_quota_lock);
    m_quota_pct = (double)m_quota_shares / total_quota;
}

// set usage(percent) of dominant resource
void Group::SetDominantPct() {
    WriteLocker lock(m_quota_lock);
    double cpu_pct = (double)m_used_cpu / s_total_cpu;
    double memory_pct = (double)m_used_memory / s_total_memory;
    if (cpu_pct > memory_pct)
        m_dominant_pct = cpu_pct;
    else
        m_dominant_pct = memory_pct;
}

bool Group::IsOverLimit() {
    return m_dominant_pct > m_maxlimit_pct;
}

// calculate relative ratio
void Group::SetRelativeRatio() {
    m_relative_ratio = m_dominant_pct / m_quota_pct;
}

double Group::GetRelativeRatio() {
    return m_relative_ratio;
}

void Group::SetUsedCPU(const double delta_cpu, bool minus) {
    if (minus) {
        // minus
        m_used_cpu -= delta_cpu;
        if (m_used_cpu < 0) {
            LOG4CPLUS_ERROR(logger, m_group_name << ": m_used_cpu < 0");
            m_used_cpu = 0;
        }
    } else {
        m_used_cpu += delta_cpu;
    }
}
 
void Group::SetUsedMemory(const double delta_memory, bool minus) {
    if (minus) {
        // minus
        m_used_memory -= delta_memory;
        if (m_used_memory < 0) {
            LOG4CPLUS_ERROR(logger, m_group_name << ": m_used_memory < 0");
            m_used_memory = 0;
        }
    } else {
        m_used_memory += delta_memory;
    }

}

// get cluster global resource view
double Group::GetTotalCPU() {
   return s_total_cpu;
}

double Group::GetTotalMemory() {
    return s_total_memory;
}


// set cluster global resource view
bool SetTotalResource() {
    MultiD_Resource md_r;
    try {
        Proxy<ResourceSchedulerClient> proxy = RpcClient<ResourceSchedulerClient>::GetProxy(FLAGS_resource_scheduler_endpoint);
        proxy().GetTotalResource(md_r);
    } catch (TException &tx) {
        LOG4CPLUS_ERROR(logger, " get total resource error: " << tx.what());
        return false;
    }
    Group::s_total_cpu = md_r.total_cpu;
    Group::s_total_memory = md_r.total_memory;
    return true;
}
