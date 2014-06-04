/********************************
 FileName: JobsManager/job_master.cpp
 Author:   ZhangZhang & WangMin
 Date:     2014-04-08
 Version:  0.1
 Description: master of navigating job, making resource request for job
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "include/proxy.h"
#include "include/classad_attr.h"
#include "common/clynn/rpc.h"
#include "common/clynn/rwlock.h"
#include "common/clynn/string_utility.h"
#include "common/classad/classad_complement.h"

#include "JobsManager/job_enum.h"
#include "JobsManager/job_master.h"
#include "JobsManager/job_pool.h"
#include "JobsManager/group.h"
#include "JobsManager/group_pool.h"
#include "JobsManager/event.h"
#include "JobsManager/dispatcher.h"

using clynn::ReadLocker;
using clynn::WriteLocker;
using log4cplus::Logger;

static Logger logger = Logger::getInstance("JobsManager");

JobMaster::JobMaster(const JobPtr& job_ptr) : m_navigating_job(job_ptr) {
    switch (m_navigating_job->GetSchedModel()) {
        case DEFAULT_MODEL:
            m_scheduler = SchedulerPtr(new DefaultScheduler());
            break;
        case WIDE_DISTRIBUTION:
            m_scheduler = SchedulerPtr(new WDScheduler());
            break;
        default:
            m_scheduler = SchedulerPtr(new DefaultScheduler());
    }
}

int32_t JobMaster::Schedule() {
    return m_scheduler->ScheduleOneJob(m_navigating_job);
}
