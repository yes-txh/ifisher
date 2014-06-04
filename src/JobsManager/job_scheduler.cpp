/********************************
 FileName: JobsManager/job_scheduler.cpp
 Author:   WangMin
 Date:     2014-05-21
 Version:  0.2
 Description: scheduler of navigating job, making resource request for job
*********************************/

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>

#include "common/clynn/timer.h"
#include "common/clynn/rpc.h"
#include "common/classad/classad_complement.h"
#include "proxy/ResourceScheduler/gen-cpp/ResourceScheduler.h"

#include "JobsManager/job_scheduler.h"

DECLARE_string(resource_scheduler_endpoint);

using log4cplus::Logger;
using clynn::Timer;

static Logger logger = Logger::getInstance("JobsManager");

int32_t JobScheduler::SendRequest(const string& classad, string& match_result) {
    LOG4CPLUS_INFO(logger, "SendRequest " << classad);
    Timer timer;
    try {
        Proxy<ResourceSchedulerClient> proxy = RpcClient<ResourceSchedulerClient>::GetProxy(FLAGS_resource_scheduler_endpoint);
        proxy().MatchMachine(match_result, classad); 
    } catch (TException &tx) {
        LOG4CPLUS_ERROR(logger, "Send Request UnSuccess.");
        return 1;
    } 
    LOG4CPLUS_INFO(logger, "match time: " << timer.ElapsedSeconds() << "s");
    return 0; 
}

int32_t JobScheduler::SendRequestConstraint(const string& classad, const vector<string>& soft_list, string& match_result) {
    LOG4CPLUS_INFO(logger, "SendRequest " << classad);
    Timer timer;
    try {
        Proxy<ResourceSchedulerClient> proxy = RpcClient<ResourceSchedulerClient>::GetProxy(FLAGS_resource_scheduler_endpoint);
        proxy().MatchMachineConstraint(match_result, classad, soft_list);
    } catch (TException &tx) {
        LOG4CPLUS_ERROR(logger, "Send Request UnSuccess.");
        return -1;
    }
    LOG4CPLUS_INFO(logger, "match time: " << timer.ElapsedSeconds() << "s");
    return 0;
}
 
