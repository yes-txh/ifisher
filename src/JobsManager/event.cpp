/********************************
 FileName: JobsManager/event.cpp
 Author:   Tangxuehai
 Date:     2014-03-20
 Version:  0.1
 Description: event, and its handler
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "JobsManager/event.h"
#include "JobsManager/image_manager.h"
#include "JobsManager/app_manager.h"
#include "JobsManager/job_pool.h"


using log4cplus::Logger;

static Logger logger = Logger::getInstance("JobsManager");

// update image
bool ImageEvent::Handle() {
    string name = GetName();
    string user = GetUser();
    int32_t size = GetSize();
    bool is_update_all = GetIs_update_all();
    return ImageMgrI::Instance()->UpdateImage(user, name, size, is_update_all);
}

// Create app
bool AppEvent::Handle() {
    string name = GetName();
    string user = GetUser();
    return AppMgrI::Instance()->CreateApp(user, name);
}

// start task
bool StartTaskEvent::Handle() {
    // TaskPtr task_ptr = JobPoolI::Instance()->GetTask(m_job_id, m_task_id);
    JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(m_job_id);
    if (NULL == job_ptr) {
        return false;
    }

    TaskPtr task_ptr = job_ptr->GetTaskPtr(m_task_id);
    if (NULL == task_ptr) {
        return false;
    }

    return task_ptr->StartTask(m_executor_endpoint, m_taskad);
}

// task started
bool TaskStartedEvent::Handle() {
    // TaskPtr task_ptr = JobPoolI::Instance()->GetTask(m_job_id, m_task_id);

    JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(m_job_id);
    if (NULL == job_ptr) {
        return false;
    }

    TaskPtr task_ptr = job_ptr->GetTaskPtr(m_task_id);
    if (NULL == task_ptr) {
        return false;
    }
    task_ptr->TaskStarted();

    /* if (job->GetRunTaskNum() == 1) {
        job->SetRunTime();
        job->UpdateTimeByState(TASK_WAITING);
    } */
    return true;
}

// task finished
bool TaskFinishedEvent::Handle() {
    JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(m_job_id);
    if (NULL == job_ptr) {
        return false;
    }

    TaskPtr task_ptr = job_ptr->GetTaskPtr(m_task_id);
    if (NULL == task_ptr) {
        return false;
    }
    task_ptr->TaskFinished();

    return true;
}

// task failed
bool TaskFailedEvent::Handle() {
    JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(m_job_id);
    if (NULL == job_ptr) {
        return false;
    }

    TaskPtr task_ptr = job_ptr->GetTaskPtr(m_task_id);
    if (NULL == task_ptr) {
        return false;
    }
    task_ptr->TaskFailed();

    return true;
}

// task timeout
bool TaskTimeoutEvent::Handle() {
    JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(m_job_id);
    if (NULL == job_ptr) {
        return false;
    }

    TaskPtr task_ptr = job_ptr->GetTaskPtr(m_task_id);
    if (NULL == task_ptr) {
        return false;
    }
    task_ptr->TaskTimeout();

    return true;
}

// task missed
bool TaskMissedEvent::Handle() {
    JobPtr job_ptr = JobPoolI::Instance()->GetJobPtr(m_job_id);
    if (NULL == job_ptr) {
        return false;
    }

    TaskPtr task_ptr = job_ptr->GetTaskPtr(m_task_id);
    if (NULL == task_ptr) {
        return false;
    }
    task_ptr->TaskMissed();

    return true;
}


