/********************************
 FileName: executor/register.cpp
 Author:   Tangxuehai
 Date:     2014-05-25
 Version:  0.1
 Description: register
*********************************/
#include <stdlib.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>
#include <iostream>
#include <string>

#include "include/proxy.h"
#include "common/clynn/rpc.h"
#include "executor/executor_register.h"
#include "executor/system.h"

DECLARE_bool(debug);
DECLARE_string(resource_scheduler_endpoint);
DECLARE_string(lynn_version);
DECLARE_string(interface);


using log4cplus::Logger;
using namespace std;

static Logger logger = Logger::getInstance("executor");

bool ExecutorRegister::RegistMachine() {
    string ip = System::GetIP(FLAGS_interface.c_str());
    try {
        Proxy<ResourceSchedulerClient> proxy = RpcClient<ResourceSchedulerClient>::GetProxy(FLAGS_resource_scheduler_endpoint);
        if(proxy().RegistMachine(ip, FLAGS_lynn_version) == -1) {
	    LOG4CPLUS_ERROR(logger, "Failed to regist machine , lynn version  error");
	    return false;
	}
    } catch (TException &tx) {
        LOG4CPLUS_ERROR(logger, "rpc error: regist machine failed " << tx.what());
        return false;
    }
    return true;
}

