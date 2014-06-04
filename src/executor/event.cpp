/********************************
 FileName: executor/event.cpp
 Author:   WangMin & TangXuehai
 Date:     2013-09-24 & 2014-05-10
 Version:  0.1 & 0.2
 Description: event, and its handler
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "executor/event.h"
#include "executor/task_entity_pool.h"
#include "executor/task_action.h"
#include "executor/vm_pool.h"
#include "executor/image_manager.h"

using log4cplus::Logger;

static Logger logger = Logger::getInstance("executor");

DECLARE_bool(debug);

/* task action handle*/
// start task
bool StartActionEvent::Handle() {
    TaskID id = GetID();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of start task action, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
    if (!TaskPoolI::Instance()->StartTaskByID(id)) {
        LOG4CPLUS_ERROR(logger, "Failed to start task, job_id:" << id.job_id << ", task_id:" << id.task_id);
        return false;
    }

    LOG4CPLUS_INFO(logger, "Handle event of start task action, job_id:" << id.job_id << ", task_id:" << id.task_id);
    return true;
}

// kill task
bool KillActionEvent::Handle() {
    TaskID id = GetID();
    if(FLAGS_debug) {
    LOG4CPLUS_DEBUG(logger, "Trigger event of kill task action, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
    if (!TaskPoolI::Instance()->KillTaskByID(id)) {
        LOG4CPLUS_ERROR(logger, "Failed to kill task, job_id:" << id.job_id << ", task_id:" << id.task_id);
        return false;
    }

    TaskPoolI::Instance()->Delete(id);
    LOG4CPLUS_INFO(logger, "Handle event of kill task action, job_id:" << id.job_id << ", task_id:" << id.task_id);
    return true;
}

//stop task TODO

bool StopActionEvent::Handle() {
    return true;
}

/* task state handle */
// task starting
bool StartingTaskEvent::Handle() {
    TaskID id = GetID();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of taskstarting state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
    TaskPoolI::Instance()->GetTaskPtr(id)->TaskStarting();
    LOG4CPLUS_INFO(logger, "Handle event of taskstarting state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    return true;
}

// task running
bool RunningTaskEvent::Handle() {
    TaskID id = GetID();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of taskrunning state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
    //TaskPoolI::Instance()->GetTaskPtr(id)->TaskRunning();
    //LOG4CPLUS_INFO(logger, "Handle event of taskrunning state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    TaskActionI::Instance()->TaskRunning(id);
    return true;
}

// task finished
bool FinishedTaskEvent::Handle() {
    TaskID id = GetID();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of taskfinished state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
    //TaskPoolI::Instance()->GetTaskPtr(id)->TaskFinished();
    //LOG4CPLUS_INFO(logger, "Handle event of taskfinished state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    TaskActionI::Instance()->TaskFinished(id);
    return true;
}


// task failed
bool FailedTaskEvent::Handle() {
    TaskID id = GetID();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of taskfailed state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
    //TaskPoolI::Instance()->GetTaskPtr(id)->TaskFailed();
    //LOG4CPLUS_INFO(logger, "Handle event of taskfailed state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    TaskActionI::Instance()->TaskFailed(id);
    return true;
}


// task missed
bool MissedTaskEvent::Handle() {
    TaskID id = GetID();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of taskmissed state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
    //TaskPoolI::Instance()->GetTaskPtr(id)->TaskMissed();
    //LOG4CPLUS_INFO(logger, "Handle event of taskmissed state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    TaskActionI::Instance()->TaskMissed(id);
    return true;
}

// task timeout
bool TimeoutTaskEvent::Handle() {
    TaskID id = GetID();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of task timeout state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    }
    //TaskPoolI::Instance()->GetTaskPtr(id)->TaskTimeout();
    //LOG4CPLUS_INFO(logger, "Handle event of task timeout state, job_id:" << id.job_id << ", task_id:" << id.task_id);
    TaskActionI::Instance()->TaskTimeout(id);
    return true;
}


/* image update event handle */
// update image
bool ImageEvent::Handle() {
    string name = GetName();
    string user = GetUser();
    int32_t size = GetSize();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of updating image, user:" << user << ", name:" << name << ", size:"<< size);
    }
    if(!(ImageMgrI::Instance()->ExecutorUpdateImage(user, name, size))) {
	LOG4CPLUS_ERROR(logger, "Failed to update image, user:" << user << ", name:" << name << ", size:"<< size);
        return false;
    }
    LOG4CPLUS_INFO(logger, "Handle event of updating image, user:" << user << ", name:" << name << ", size:"<< size);
    return true;
}

/* VM heartbeat event handle */
// update heartbeat
bool HeartbeatEvent::Handle() {
    string hb_vm_info_ad = GetHbInfo();
    if(FLAGS_debug) {
        LOG4CPLUS_DEBUG(logger, "Trigger event of updating vm heartbeat, vm_info:" << hb_vm_info_ad);
    }
    VMPoolI::Instance()->ProcessHbVMInfo(hb_vm_info_ad);
    //LOG4CPLUS_INFO(logger, "Handle event of updating vm heartbeat, vm_info:" << hb_vm_info_ad);
    return true;
}

// update heartbeat
bool ExitExecutorEvent::Handle() {
    LOG4CPLUS_INFO(logger, "Handle event of exit executor");
    exit(1);
}

