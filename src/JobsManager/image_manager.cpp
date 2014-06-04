/********************************
 FileName: JobsManager/image_manager.cpp
 Author:   Tangxuehai
 Date:     2014-03-20
 Version:  0.1
 Description: image maneger
*********************************/
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <gflags/gflags.h>
#include <iostream>
#include <vector>
#include <string>

#include "JobsManager/image_manager.h"
#include "common/clynn/rpc.h"
#include "include/proxy.h"

DECLARE_string(image_path);
DECLARE_string(resource_scheduler_endpoint);
using log4cplus::Logger;
using namespace std;

static Logger logger = Logger::getInstance("JobsManager");

bool ImageManager::UpdateImage(const string& user, const string& name, const int32_t size, bool is_update_all) {
    vector<string> executor_endpoint_list;
    //test
    //executor_endpoint.push_back("192.168.11.51:9997");
    try {
    	Proxy<ResourceSchedulerClient> proxy = Rpc<ResourceSchedulerClient, ResourceSchedulerClient>::GetProxy(FLAGS_resource_scheduler_endpoint);
    	proxy().GetMachineListByImageInfo(executor_endpoint_list, user, name, size, is_update_all);
    	} catch (TException &tx) {
        	LOG4CPLUS_ERROR(logger, "can't get machine by image info");
        	return false;
    	  }
    if(executor_endpoint_list.size() == 0) {
	LOG4CPLUS_INFO(logger, "no machine need update the image");
        return true;

    }

    for(vector<string>::iterator it = executor_endpoint_list.begin(); it != executor_endpoint_list.end(); ++it) {
        try {
              Proxy<ExecutorClient> proxy = Rpc<ExecutorClient, ExecutorClient>::GetProxy(*it);
              if(!proxy().UpdateImage(user, name, size)) {
		  LOG4CPLUS_ERROR(logger, "can't update image on executor:"+ *it);
                  continue;
              }
            } catch (TException &tx) {
                 LOG4CPLUS_ERROR(logger, "can't update image on executor:"+ *it);
                 continue;
             }
    }
    return true;
}
