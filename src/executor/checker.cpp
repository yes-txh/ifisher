/********************************
 FileName: executor/image_manager.cpp
 Author:   Tangxuehai
 Date:     2014-03-16
 Version:  0.1
 Description: image maneger
*********************************/
#include <stdlib.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>
#include <iostream>
#include <string>

#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "executor/checker.h"

DECLARE_bool(debug);
DECLARE_string(resource_scheduler_endpoint);
DECLARE_string(lynn_version);


using log4cplus::Logger;
using namespace std;

static Logger logger = Logger::getInstance("executor");

bool Checker::CheckVersion() {
    // check executor version
    string new_lynn_version;
    try {
        Proxy<ResourceSchedulerClient> proxy = RpcClient<ResourceSchedulerClient>::GetProxy(FLAGS_resource_scheduler_endpoint);
        proxy().GetCurVersion(new_lynn_version);
	if(new_lynn_version !=FLAGS_lynn_version) {
             LOG4CPLUS_ERROR(logger, "executor version is error");
             string lynn_version = "--lynn_version=" + new_lynn_version;
             string cmd = "../script/update_executor.sh";
             system(cmd.c_str());
             exit(1);
        }
    } catch (TException &tx) {
        LOG4CPLUS_ERROR(logger, "rpc error: check version is failed " << tx.what());
        return false;
    }

}

