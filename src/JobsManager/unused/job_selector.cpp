/********************************
 FileName: JobsManager/job_selector.cpp
 Author:   ZhangZhang & WangMin
 Date:     2014-04-08
 Version:  0.1
 Description: selector one job from Queues, then make resource request
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "JobsManager/job_selector.h"
#include "JobsManager/group.h"
#include "JobsManager/group_pool.h"

using log4cplus::Logger;
using clynn::ReadLocker;
using clynn::WriteLocker;

static Logger logger = Logger::getInstance("JobsManager");

bool JobSelector::SelectJob() {
    // select one job -> navigating job
    if (!GroupPoolI::Instance()->SelectJobToNavigatingJob()) {
        // no job        
        return false;
    }
    GroupPoolI::Instance()->AddUsedResource();
    // test TODO
    GroupPoolI::Instance()->PrintAll();

    // new JobMaster
    JobMaster job_master(GroupPoolI::Instance()->GetNavigatingJob());     

    // make resource request for navigating job 
    if (job_master.MakeMatch() != 0) { 
        LOG4CPLUS_ERROR(logger, "Failed to Make Resource Request.");
        GroupPoolI::Instance()->MoveNavigatingJobToWaitQueue();
        return false;    
    }

    if (job_master.StartTasks() != 0) {
        LOG4CPLUS_ERROR(logger, "Failed to start tasks.");
        return false;
    } 

    // TODO
    // move navigating job to run queue
    GroupPoolI::Instance()->MoveNavigatingJobToRunQueue(); 

    return true;
} 
