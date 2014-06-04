/********************************
 FileName: resourcescheduler/event.cpp
 Author:   TangXuehai
 Date:     2014-05-16
 Version:  0.1 
 Description: event, and its handler
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "ResourceScheduler/event.h"
#include "ResourceScheduler/resource_scheduler_engine.h"

using log4cplus::Logger;

static Logger logger = Logger::getInstance("ResourceScheduler");


/*machine event handle*/
bool MachineUpdateEvent::Handle() {
    string heartbeat_ad = GetHeartbeatAd();
    vector<string> task_list = GetTaskList();
    if(COLLECTOR_ENGINE::Instance()->NewUpdateMachine(heartbeat_ad, task_list) != 0) {
	LOG4CPLUS_ERROR(logger, "Failed to update a machine:" <<heartbeat_ad );
    }
}


bool MachineDeleteEvent::Handle() {
    string ip = GetIp();
    if(COLLECTOR_ENGINE::Instance()->DeleteMachine(ip) != 0) {
	LOG4CPLUS_ERROR(logger, "Failed to delete a machine:" <<ip );
    }
}
