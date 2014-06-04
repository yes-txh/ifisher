/********************************
 FileName: ResourceScheduler/resource_scheduler_service.cpp
 Author:   ZhangZhang 
 Date:     2014-05-11
 Version:  0.1
 Description: resource scheduler service
*********************************/


#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "include/proxy.h"
#include "common/clynn/rpc.h"

#include "ResourceScheduler/resource_scheduler_service.h"
#include "ResourceScheduler/resource_scheduler_engine.h"
#include "ResourceScheduler/match_maker.h"
#include "ResourceScheduler/dispatcher.h"

#include <iostream>
#include <sstream>
#include <string>

using std::string;
using std::stringstream;
using log4cplus::Logger;
static Logger logger = Logger::getInstance("ResourceScheduler");

void ResourceSchedulerService::GetCurVersion(string& new_version) {
    return COLLECTOR_ENGINE::Instance()->GetCurVersion(new_version);
}

int ResourceSchedulerService::RegistMachine(const string& ip, const string& version) {
    LOG4CPLUS_INFO(logger, "regist machine ip:" << ip);
    return COLLECTOR_ENGINE::Instance()->RegistMachine(ip, version);
}


int ResourceSchedulerService::UpdateMachine(const string& heartbeat_ad){
    LOG4CPLUS_INFO(logger, "a new update:" << heartbeat_ad);
    return COLLECTOR_ENGINE::Instance()->UpdateMachine(heartbeat_ad);
}

int ResourceSchedulerService::NewUpdateMachine(const string& heartbeat_ad, const vector<string>& task_list){
    // new MachineUpdateEvent
    EventPtr event(new MachineUpdateEvent(heartbeat_ad, task_list));
    // Push event into Queue
    EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    return 0;
}


int ResourceSchedulerService::DeleteMachine(const string& ip){
     // new MachineUpdateEvent
    EventPtr event(new MachineDeleteEvent(ip));
    // Push event into Queue
    EventDispatcherI::Instance()->Dispatch(event->GetType())->PushBack(event);
    return 0;
}

void ResourceSchedulerService::GetTotalResource(MultiD_Resource& md_r) {
    md_r = COLLECTOR_ENGINE::Instance()->GetTotalResource();
    return;
}

void ResourceSchedulerService::MatchMachine(string& result, const string& task_ad){
    LOG4CPLUS_INFO(logger, "MatchMachine");
    MATCH_MAKER::Instance()->NegotiationTask(task_ad, result);
}

void ResourceSchedulerService::MatchMachineConstraint(string& result, const string& task_ad, const vector<string>& soft_list) {
    LOG4CPLUS_INFO(logger, "MatchMachineConstraint");
    MATCH_MAKER::Instance()->NegotiationTaskConstraint(task_ad, soft_list, result);
}

void ResourceSchedulerService::QueryMachine(string& result, const string& machine){
    LOG4CPLUS_INFO(logger, "QueryMachine");
}

void ResourceSchedulerService::ListAllMachines(vector<string>& result){ 
    LOG4CPLUS_INFO(logger, "ListAllMachines");
}

void ResourceSchedulerService::Negotiate(vector<string>& result, const string& request){
    MATCH_MAKER::Instance()->Negotiation(request, result);
}

void ResourceSchedulerService::GetMachineListByImageInfo(vector<string>& machine_list, const string& user, const string& name, const int32_t size, bool is_update_all) {
    LOG4CPLUS_INFO(logger, "GetMachineListByImageInfo");
    return COLLECTOR_ENGINE::Instance()->GetMachineListByImageInfo(machine_list, user, name, size, is_update_all);
}
