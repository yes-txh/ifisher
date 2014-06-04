/********************************
 FileName: JobsManager/work_thread.cpp
 Author:   WangMin
 Date:     2014-04-29
 Version:  0.2
 Description: work threads
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <tr1/functional>

#include "JobsManager/job_pool.h"
#include "JobsManager/group_pool.h"
#include "JobsManager/job_master.h"

using log4cplus::Logger;
using std::tr1::bind;
using std::tr1::placeholders::_1;

static Logger logger = Logger::getInstance("JobsManager");

void* NewJobThread(void* ununsed) {
    while (true) {
        JobPtr job;
        NewJobList::Instance()->PopFront(&job);
        job->SetWaitTime();
        if (!GroupPoolI::Instance()->AddJobToWaitQueue(job)){
            job->SetState(JOB_FAILED);
            JobPoolI::Instance()->HistoryJobListPushBack(job);
        }
    }
    return NULL;
}

/* void* JobSelectorThread(void* unused) {
    while(true) {
        JobSelector selector;
        if (selector.SelectJob() == false) {
            // TODO
            usleep(1000);
            continue;
        }
    }
    return NULL;
}*/

void* JobProcessorThread(void* unused) {
    while (true) {
        // select one job -> navigating job
        if (!GroupPoolI::Instance()->SelectJobToNavigatingJob()) {
            // no job
            usleep(1000); 
            continue;
        }
        // add used resource according to group
        // GroupPoolI::Instance()->AddUsedResource();

        // new JobMaster & schedule
        JobMaster job_master(GroupPoolI::Instance()->GetNavigatingJob());
        int32_t rc = job_master.Schedule();
        if (rc != 0) {
            //  0: all tasks success
            // -1: partly tasks success
            // -2: all tasks no success
            LOG4CPLUS_ERROR(logger, "Failed to Make Resource Request.");
            if (rc == -1)
                GroupPoolI::Instance()->MoveNavigatingJobToWaitQueue(true);
            else
                GroupPoolI::Instance()->MoveNavigatingJobToWaitQueue(); 
            usleep(1000);
            continue;
        }

        // move navigating job to run queue
        GroupPoolI::Instance()->MoveNavigatingJobToRunQueue();

        // test TODO
        GroupPoolI::Instance()->PrintAll();
    }
    return NULL;
}

void* TaskCheckerThread(void* ununsed) {
    while (true) {        
        JobPoolI::Instance()->CheckJobs();
        sleep(600);
    }
    return NULL;
}

void* JobStateUpdaterThread(void* ununsed) {
    while (true) {
        JobPoolI::Instance()->UpdateJobState();
        sleep(60*2);
    }
    return NULL;
}

